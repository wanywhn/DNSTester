#include "mainwindow.h"
#include "multilistview.h"
#include "multlistitem.h"
#include "networkutils.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMutex>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QTextStream>
#include <QThread>
#include <QTimer>

bool MainWindow::clickedSetDns = false;
void MainWindow::findNetworkInterface() {

  auto names = utils->findNetworkInterface();
  for (auto item : names) {
    ui->comboBox_hw->addItem(item);
  }
}

void MainWindow::addItemtoTableWidget() {

  listViewItems.clear();
  for (int i = 0; i != DnsCount; ++i) {
    listViewItems.append(new MultiListItem(DnsList[i], tr("待测试")));
  }
  resultWidget->refreshItems(listViewItems);
}

void MainWindow::reserveSpace(int count) { vProcess.resize(count); }

void MainWindow::initUI() {
  setWindowIcon(QIcon(":/icon/resource/icon/icons8-DNS-50.png"));

  ui->next_Intro_Btn->setText(tr("下一步"));
  ui->pre_Test_Btn->setText(tr("前一页"));
  ui->set_Result_Btn->setText(tr("进行设置"));
  ui->start_Test_Btn->setText(tr("开始测试"));
  ui->update_DNS_List->setText(tr("更新DNS列表"));
  ui->next_Intro_Btn->setGeometry(ui->set_Result_Btn->geometry());
  //  ui->label_DNS_exp->resize(900, 400);
  ui->label_DNS_exp->setPixmap(QPixmap(":/image/resource/image/DNS_exp.jpg"));
  //.scaled(ui->label_DNS_exp->size()));
  ui->label_DNS_exp->setScaledContents(true);
  ui->label_ip->setText(tr("请选择当前网卡"));

  ui->label_intro->setWordWrap(true);
  ui->label_intro->setText(
      "<font size='+2'><p align='center'>优选适合的DNS"
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
      "</font>"
      "<br/><a href=\"https://icons8.com/icon/41400/DNS\">Dns icon</a> by "
      "Icons8");

  ui->stackedWidget->setCurrentIndex(0);

  connect(ui->next_Intro_Btn, &QPushButton::clicked,
          [this] { ui->stackedWidget->setCurrentWidget(ui->page_Test); });
  connect(ui->pre_Test_Btn, &QPushButton::clicked,
          [this] { ui->stackedWidget->setCurrentWidget(ui->page_Intro); });
  connect(ui->start_Test_Btn, &QPushButton::clicked, this,
          &MainWindow::startTest);
  connect(ui->set_Result_Btn, &QPushButton::clicked, [this] {
    setDns();

  });
  //每次尝试次数
  PingTimes = 3;
  DnsList << "114.114.114.114"
          << "114.114.115.115"
          << "223.5.5.5"
          << "223.6.6.6"
          << "112.124.47.27"; // TODO 获取数据
  DnsCount = DnsList.size();
  reserveSpace(DnsCount);
  resultWidget = new MultiListView();
  ui->horizontalLayout_dns_and_sec->addWidget(resultWidget);

  mQProgressbar = ui->progressBar;
  mQProgressbar->setMinimum(0);
  mQProgressbar->setMaximum(DnsCount * PingTimes);
  mQProgressbar->setValue(0);
}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowTitle(tr("DNS优化器"));
  utils = nullptr;
  initRes();
  initUI();

  findNetworkInterface();
  addItemtoTableWidget();
}

MainWindow::~MainWindow() {
  delete ui;
  //  if (nullptr != resultWidget)
  //    delete resultWidget;
}

void MainWindow::startPing(QString program, int index) {
  QProcess *mPing = new QProcess(this);
  vProcess.insert(index, mPing);
  program += DnsList[index];
  conn = connect(
      mPing,
      static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
          &QProcess::finished),
      [this, index]() { this->processFinished(index); });
  mConn.push_back(conn);
  mPing->start(program);
  qDebug() << program;
}

void MainWindow::processFinished(int index) {
  this->store(index);
  updateListViewItems(index, DnsFinalResult[index]);
  emit pingFinished();
}

void MainWindow::setSelectItemColor(bool f, QColor color) {
  auto item = dynamic_cast<MultiListItem *>(listViewItems.at(dnsSelectedId));
  if (nullptr == item)
    return;
  item->setTextColor(f, color);
  resultWidget->repaint();
}

void MainWindow::continueNext(QString program) {
  if (++timeouted <= DnsCount) {
    ui->progressBar->setValue((timeouted - 1) * PingTimes);
    startPing(program, timeouted - 1);
  } else {
    ui->progressBar->setValue(DnsCount * PingTimes);
    updateListViewItems(DnsCount - 1, DnsFinalResult[DnsCount - 1]);
    dnsSelectedId = 0;
    for (auto i : DnsFinalResult) {
      DnsNumberResult.append(i.split('/').at(1).toDouble());
    }

    double max = DnsNumberResult.at(0);
    for (int i = 0; i != DnsCount; ++i) {
      if (DnsNumberResult.at(i) < max) {
        dnsSelectedId = i;
        max = DnsNumberResult.at(i);
      }
    }
    dnsSelected = DnsList.at(dnsSelectedId);
    setSelectItemColor(true, Qt::red);

    foreach (conn, mConn) { disconnect(conn); }
    testStarted = false;
  }
}

void MainWindow::startTest() {
  if (testStarted) {
    return;
  }
  initRes();
  addItemtoTableWidget();
  testStarted = true;
  setSelectItemColor(false, Qt::black);
  QString program = QString("ping -W 1 -c %L1 ").arg(PingTimes);

  ui->progressBar->setValue(1);
  conn = connect(this, &MainWindow::pingFinished,
                 [=]() { continueNext(program); });
  mConn.push_back(conn);
  //  mConn[0]=conn;
  //坑，多连接了几次
  startPing(program, 0);
}

void MainWindow::setDns() {
  if (true == testStarted) {
    QMessageBox::warning(ui->centralWidget, tr("错误"), tr("请先qingdai"));
    return;
  }
  if (dnsSelected == "") {
    QMessageBox::warning(ui->centralWidget, tr("错误"), tr("请先进行测试"));
    return;
  }
  if (clickedSetDns) {
    QMessageBox::information(this, tr("请重新测试"),
                             tr("您已经设置过了，请重新测试一遍。"));
    return;
  }
  int ret = QMessageBox::warning(
      this, tr("警告"), tr("将重启网络以生效，确认？"),
      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  if (ret != QMessageBox::Yes)
    return;
  QString hw_interfaceName = ui->comboBox_hw->currentText().split(":").at(0);
  QProcess *mprocess = new QProcess(this);
  conn = connect(
      mprocess, &QProcess::errorOccurred,
      [this](QProcess::ProcessError error) { qDebug() << "ERROR" << error; });
  mConn.push_back(conn);
  conn = connect(
      mprocess,
      static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
          &QProcess::finished),
      [this] {
        QString output = vProcess.back()->readAllStandardOutput();
        QStringList splited = output.split(" ");
        splited.removeDuplicates();
        auto filted = splited.filter(QRegExp("^([a-z0-9]+-){4}[a-z0-9]+$"));
        if (filted.size() == 0) {
          QMessageBox::warning(
              ui->centralWidget, tr("错误"),
              tr("无法获取硬件UUID，请确认选择了正确的网络设备"));
          return;
        }
        QString UUID = filted.at(0);

        QHostAddress addr(dnsSelected);
        connect(utils, &NetworkUtils::restartSuccessed, [this]() {
          QMessageBox::information(this, tr("chenggong"),
                                   tr("chongqi chenggong"));
          clickedSetDns = true;
        });
        utils->ChangeDNSTo(addr, UUID);

      });
  mConn.push_back(conn);
  QStringList options;
  options << "-c";
  options << "nmcli con show |grep " + hw_interfaceName;
  mprocess->start("/bin/bash", options);
  vProcess.push_back(mprocess);
  // TODO kaolv shiyong guanlian rongqi
}

//读取数据放至Result中
void MainWindow::store(int index) {
  auto proc = vProcess.at(index);
  //  iter += index;
  QByteArray res = proc->readAllStandardOutput();
  QTextStream in(res);
  QString line;
  QRegularExpression re(
      QStringLiteral("\\d*\\.\\d*/\\d*\\.\\d*/\\d*\\.\\d*/\\d*\\.\\d* "));
  line = in.readAll();
  auto matches = re.match(line);
  if (!matches.hasMatch()) {
    DnsFinalResult.insert(index, "500/500/500/2");
  }
  auto result = matches.captured(0).trimmed();
  DnsFinalResult.insert(index, result);
}

void MainWindow::updateListViewItems(int index, QString str) {
  auto item = dynamic_cast<MultiListItem *>(listViewItems.at(index));
  if (nullptr == item)
    return;
  item->mresult = str;
  //    resultWidget->refreshItems(listViewItems);
  resultWidget->repaint();
  // TODO have a better method?
}

void MainWindow::initRes() {
  DnsNumberResult.clear();
  if (utils != nullptr)
    delete utils;
  utils = new NetworkUtils;
  for (auto vp : vProcess) {
    delete vp;
  }
  vProcess.clear();
  timeouted = 1;
  clickedSetDns = false;
  dnsSelected = "";
  dnsSelectedId = 0;
  testStarted = false;
}

void MainWindow::on_update_DNS_List_clicked() {
  if (true == testStarted) {
    return;
  }
  netManager = new QNetworkAccessManager(this);
  conn = connect(netManager, &QNetworkAccessManager::finished, this,
                 &MainWindow::replyFinished);
  mConn.push_back(conn);
  auto reply = netManager->get(
      QNetworkRequest(QUrl("http://wanywhn.com.cn:8080/DNSList")));
  conn = connect(reply, &QNetworkReply::readyRead, [this, reply]() {
    QString line(reply->readAll());
    auto lines = line.split("\n");
    lines.removeDuplicates();
    lines.removeAll("");
    DnsList.clear();
    initRes();

    DnsList.append(lines);
    DnsCount = DnsList.count();
    this->reserveSpace(DnsCount);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(DnsCount * PingTimes);
    this->addItemtoTableWidget();
    testStarted = false;
    disconnect(conn);

    qDebug() << lines;
    reply->deleteLater();

  });
  mConn.push_back(conn);
}

void MainWindow::replyFinished(QNetworkReply *reply) {
  qDebug() << "replyFinished";
  reply->deleteLater();
}
