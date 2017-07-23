#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMutex>
#include <QNetworkInterface>
#include <QTextStream>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowTitle(tr("DNS优化器"));
  ui->next_Intro_Btn->setText(tr("下一步"));
  ui->pre_Test_Btn->setText(tr("前一页"));
  ui->set_Result_Btn->setText(tr("进行设置"));
  ui->start_Test_Btn->setText(tr("开始测试"));
  dnsSelected = "";
  dnsSelectedId=0;
  testStarted=false;
  ui->next_Intro_Btn->setGeometry(ui->set_Result_Btn->geometry());

  ui->label_DNS_exp->setPixmap(QPixmap(":/image/resource/image/DNS_exp.jpg"));
  ui->label_DNS_exp->setScaledContents(true);
  ui->label_ip->setText(tr("请选择当前网卡"));

  ui->label_intro->setWordWrap(true);
  ui->label_intro->setText("<font size='+2'><p align='center'>优选适合的DNS"
                           "服务器可以提高您的上网体验</p></font>\n"
                           "<font size='+0'>"
                           "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                           " DNS,全称叫做Domain Name System，即域名解析系统。"
                           "在Internet上域名与IP地址之间是一一对应的，"
                           "域名虽然便于人们记忆，但机器之间只能互相认识IP地址，"
                           "他们之间的转换工作称之为域名解析，"
                           "域名解析需要由专门的服务器来完成，"
                           "DNS就是进行域名解析的服务器。<br/>"
                           "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                           " 如果您遇到网页打开慢，网页打不开，页面被劫持，经常弹广告，"
                           "很有可能是DNS服务器较差，不稳定，或者服务器被劫持导致。"
                           "</font>");
  //统计线程个数
  numProc = 0;

  ui->stackedWidget->setCurrentIndex(0);

  connect(ui->next_Intro_Btn, &QPushButton::clicked,
          [this] { ui->stackedWidget->setCurrentWidget(ui->page_Test); });
  connect(ui->pre_Test_Btn, &QPushButton::clicked,
          [this] { ui->stackedWidget->setCurrentWidget(ui->page_Intro); });
  connect(ui->start_Test_Btn, &QPushButton::clicked, this,
          &MainWindow::startTest);
  connect(ui->set_Result_Btn, &QPushButton::clicked, [this] {
    if (dnsSelected == "") {
      QMessageBox::warning(ui->centralWidget, tr("错误"), tr("请先进行测试"));
      return;
    }
    int ret = QMessageBox::warning(
        this, tr("警告"), tr("将重启网络以生效，确认？"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret == QMessageBox::Yes)
      setDns();
    else
      return;

  });
  //每次尝试次数
  PingTimes = 3;
  DnsList << "114.114.114.114"
          << "114.114.115.115"
          << "223.5.5.5"
          << "223.6.6.6"
          << "112.124.47.27"; // TODO 获取数据
  // Dns个数
  DnsCount = DnsList.size();

//  DnsNumResult = new QList<double>[Count];
  DnsNumResult.resize(DnsCount);
  QString labelName = "label_dns";
  QString labelSecName = "label_sec";
  QLabel *item;
  QLabel *secitem;
  for (int i = 0; i != DnsCount; ++i) {
    item = new QLabel(ui->centralWidget);
    item->setObjectName(labelName + QString::number(i));
    item->setText("DNS服务器地址：" + DnsList[i]);
    ui->verticalLayout_dns->addWidget(item);
    secitem = new QLabel(ui->centralWidget);
    secitem->setObjectName(labelSecName + QString::number(i));
    secitem->setText(tr("待测试"));
    ui->verticalLayout_sec->addWidget(secitem);
  }

  mQProgressbar = ui->progressBar;
  mQProgressbar->setMinimum(0);
  mQProgressbar->setMaximum(DnsCount*3);
  mQProgressbar->setValue(0);

  QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
  foreach (auto interface, list) {
    qDebug() << interface.name();
    QList<QNetworkAddressEntry> entryList = interface.addressEntries();
    foreach (auto entry, entryList) {
      if (entry.ip().toString() == "" || entry.broadcast().toString() == "")
        continue;
      qDebug() << entry.ip().toString() << entry.netmask().toString()
               << entry.broadcast().toString();
      ui->comboBox_hw->addItem(interface.name() + ":" + entry.ip().toString());
    }
  }
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::startTest() {
    if(testStarted){
        return;
    }
    testStarted=true;
    vProcess.clear();
    ui->verticalLayout_sec->itemAt(dnsSelectedId)->widget()->setStyleSheet("");
  QString program = QString("ping -W 1 -c %L1 ").arg(PingTimes);

    ui->progressBar->setValue(1);
  for (QString mDns : DnsList) {
    QProcess *mPing = new QProcess(this);
    vProcess.push_back(mPing);
    ++numProc;
    QString a = program + mDns;
    connect(mPing,
            static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
                &QProcess::finished),
            [this]() {
              --numProc;
              int val = DnsCount - numProc;
              ui->progressBar->setValue(val > DnsCount ? DnsCount*2 : val+DnsCount);
              qDebug() << "当前值：" << val;
              if (numProc == 0)
                this->store();
            });
    mPing->start(a);
    ui->progressBar->setValue(numProc);
    qDebug() << a;
  }
}

void MainWindow::nmcliCall() {
  QString output = vProcess.back()->readAllStandardOutput();
  QStringList splited = output.split(" ");
  splited.removeDuplicates();
  auto filted=splited.filter(QRegExp("^([a-z0-9]+-){4}[a-z0-9]+$"));
  if(filted.size()==0){
      QMessageBox::warning(ui->centralWidget,tr("错误"),tr("无法获取硬件UUID，请确认选择了正确的网络设备"));
      return;
  }

    ui->verticalLayout_sec->itemAt(dnsSelectedId)->widget()->setStyleSheet(QString(""));
  QString UUID = filted.at(0);
  QString cmd = "nmcli con mod " + UUID + " ipv4.dns " + dnsSelected +
                " ipv4.ignore-auto-dns yes ";
  QProcess *changeDns = new QProcess(ui->centralWidget);
  changeDns->start(cmd);
  QProcess *restartNetwork = new QProcess(ui->centralWidget);

  cmd = "nmcli con up " + UUID;
  connect(restartNetwork,
          static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
              &QProcess::finished),
          [this] {
            QMessageBox::information(ui->centralWidget, tr("成功"),
                                     tr("修改成功！"));
          });
  restartNetwork->start(cmd);
}

void MainWindow::setDns() {
  QString hw_interfaceName = ui->comboBox_hw->currentText().split(":").at(0);
  QProcess *mprocess = new QProcess(this);
  connect(
      mprocess, &QProcess::errorOccurred,
      [this](QProcess::ProcessError error) { qDebug() << "ERROR" << error; });
  connect(mprocess,
          static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
              &QProcess::finished),
          [this] { nmcliCall(); });
  QStringList options;
  options << "-c";
  options << "nmcli con show |grep " + hw_interfaceName;
  mprocess->start("/bin/bash", options);
  vProcess.push_back(mprocess);
}

//读取数据放至Result中
void MainWindow::store() {
  for (QVector<QProcess *>::const_iterator iter = vProcess.cbegin();
       iter != vProcess.cend(); ++iter) {
      ui->progressBar->setValue((iter-vProcess.cbegin())+1+DnsCount*2);
    QByteArray res = (*iter)->readAllStandardOutput();
    QTextStream in(res);
    QString line;
    while (!in.atEnd()) {
      line = in.readLine(999);
      QStringList split = line.split("=");
        static  int flag=0;
      if (split.size() == 4) {
          if(++flag==PingTimes)
              flag=0;
          qDebug()<<split<<flag;
        DnsResult.append(split.at(3));
      }else {
          if(flag!=0){
          DnsResult.append("150 ms");//丢包按150ms计算
          ++flag;
          flag=(flag%PingTimes);
          qDebug()<<flag;
          }
      }
    }
  }
  cal();
}

//计算测试结果
void MainWindow::cal() {
    DnsNumResult.clear();
  for (int i = 0; i < DnsCount*PingTimes; i += PingTimes) {
    double res = 0;
    for (int j = i; j < i + PingTimes; ++j) {
      res += DnsResult.at(j).split(" ").at(0).toDouble();
    }
    DnsNumResult.push_back(res / PingTimes);
  }
  for (int i = 0; i < DnsCount; ++i) {
    dynamic_cast<QLabel *>(ui->verticalLayout_sec->itemAt(i)->widget())
        ->setText(QString::number(DnsNumResult.at(i),'f',3) + "毫秒");
  }
  dnsSelectedId = 0;
  double max = 500;
  for (int i = 0; i < DnsCount; ++i) {
    if (DnsNumResult.at(i) < max) {
      dnsSelectedId= i;
      max = DnsNumResult.at(i);
    }
  }
  dynamic_cast<QLabel *>(ui->verticalLayout_sec->itemAt(dnsSelectedId)->widget())
      ->setStyleSheet("color:red");
  dnsSelected = DnsList.at(dnsSelectedId);
  qDebug() << dnsSelected;
  DnsResult.clear();
  testStarted=false;
}
