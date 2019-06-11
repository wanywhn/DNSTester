#include "mainwindow.h"
#include "networkutils.h"
#include "ui_mainwindow.h"

#include <QHeaderView>
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
#include <QtDBus/QDBusInterface>
#include <QMenu>
#include <QtWidgets/QInputDialog>

void MainWindow::findNetworkInterface() {

    auto names = utils->findNetworkInterface();
    for (const auto &item : names) {
        ui->comboBox_hw->addItem(item);
    }
}


void MainWindow::initUI() {

    setWindowIcon(QIcon(":/icon/resource/icon/dns-tester.png"));

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

    ui->next_Intro_Btn->setText(tr("下一步"));
    ui->next_Intro_Btn->setGeometry(ui->set_Result_Btn->geometry());

    ui->pre_Test_Btn->setText(tr("前一页"));
    ui->set_Result_Btn->setText(tr("进行设置"));
    ui->start_Test_Btn->setText(tr("开始测试"));
    ui->update_DNS_List->setText(tr("更新DNS列表"));

    ui->label_DNS_exp->setPixmap(QPixmap(":/image/resource/image/DNS_exp.jpg"));
    ui->label_DNS_exp->setScaledContents(true);

    ui->label_ip->setText(tr("请选择当前网卡"));

    connect(ui->next_Intro_Btn, &QPushButton::clicked,
            [this] { ui->stackedWidget->setCurrentWidget(ui->page_Test); });

    connect(ui->pre_Test_Btn, &QPushButton::clicked,
            [this] { ui->stackedWidget->setCurrentWidget(ui->page_Intro); });
    connect(ui->set_Result_Btn, &QPushButton::clicked, [this] { setDns(); });
    connect(ui->start_Test_Btn, &QPushButton::clicked, this,
            &MainWindow::startTest);


    resultWidget = new QTableView();
    resultWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    resultWidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    model = new DnsTableModel(this);

    connect(model, &DnsTableModel::progressFinished, [this](int i) {
        this->ui->progressBar->setValue(i + 1);
    });
    connect(model, &DnsTableModel::AllCompleted, [this]() {
        notifyActivator("Completed", "test completed");
        QVector<double> DnsNumberResult;
        for (auto i = 0; i != this->model->rowCount(QModelIndex()); i++) {
            DnsNumberResult.append(
                    this->model->data(this->model->index(i, 1), Qt::DisplayRole).toString().split('/').at(
                            1).toDouble());
        }
        auto minIter =
                std::min_element(DnsNumberResult.cbegin(), DnsNumberResult.cend());
        auto midx = std::distance(DnsNumberResult.cbegin(), minIter);
        auto selected = resultWidget->selectionModel();
        selected->clearSelection();
        selected->select(
                this->model->index(midx, 0),
                QItemSelectionModel::SelectionFlag::Rows |
                QItemSelectionModel::SelectionFlag::SelectCurrent
        );
        resultWidget->setSelectionModel(selected);
        qDebug() << resultWidget->currentIndex();

    });
    resultWidget->setModel(model);
    resultWidget->setAlternatingRowColors(true);
    resultWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    resultWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    resultWidget->horizontalHeader()->resizeSection(1, 300);

    resultWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(resultWidget,&QTableView::customContextMenuRequested,this,&MainWindow::menuRequest);

    ui->horizontalLayout_dns_and_sec->addWidget(resultWidget);

}

MainWindow::MainWindow(QWidget *parent)
        : DMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle(tr("DNS优化器"));
    utils = new NetworkUtils(this);
    initUI();

    findNetworkInterface();
}

MainWindow::~MainWindow() { delete ui; }


void MainWindow::startTest() {
    model->startTest();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(model->rowCount(QModelIndex()));

}

void MainWindow::setDns() {
    if (model->isTestStarted()) {
        QMessageBox::warning(ui->centralWidget, tr("错误"), tr("请先qingdai"));
        return;
    }
//    auto idxs = resultWidget->selectionModel()->selectedIndexes();
    auto idx = resultWidget->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(
                this, tr("Warning"), tr("not select dns")
        );
        return;
    }
    int ret = QMessageBox::warning(
            this, tr("警告"), tr("将重启网络以生效，确认？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret != QMessageBox::Yes)
        return;
    QString hw_interfaceName = ui->comboBox_hw->currentText().split(":").at(0);
    auto *mprocess = new QProcess(this);
    connect(
            mprocess,
            static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
                    &QProcess::finished),
            [this, mprocess, idx] {
                QString output = mprocess->readAllStandardOutput();
                QStringList splited = output.split(" ");
                splited.removeDuplicates();
                auto filted = splited.filter(QRegExp("^([a-z0-9]+-){4}[a-z0-9]+$"));
                if (filted.empty()) {
                    QMessageBox::warning(
                            ui->centralWidget, tr("错误"),
                            tr("无法获取硬件UUID，请确认选择了正确的网络设备"));
                    return;
                }
                QString UUID = filted.at(0);
                qDebug() << "index row:" << idx.row();
                auto ip = model->index(idx.row(), 0).data(Qt::DisplayRole).toString();
                qDebug() << "IP:" << ip;

                QHostAddress addr(ip);
                qDebug() << "change to " << addr.toString();
                connect(utils, &NetworkUtils::restartSuccessed, [this]() {
                    QMessageBox::information(this, tr("成功"), tr("重启成功"));
                });
                utils->ChangeDNSTo(addr, UUID);
                // TODO 尝试DBUS方法？

            });
    QStringList options;
    options << "-c";
    options << "nmcli con show |grep " + hw_interfaceName;
    mprocess->start("/bin/bash", options);
    // TODO 考虑使用其他容器
}


void MainWindow::notifyActivator(QString title, QString text) {
    QStringList actions = QStringList() << "ShowWindow" << tr("Show Me");
    QList<QVariant> argumentList;
    argumentList << "DNSTester";
    argumentList << static_cast<uint>(0);
    argumentList << "dns-tester";
    argumentList << title;
    argumentList << tr(text.toStdString().data());
    argumentList << actions;
    argumentList << QVariantMap();
    argumentList << static_cast<int>(3000);

    static QDBusInterface notifyApp("org.freedesktop.Notifications",
                                    "/org/freedesktop/Notifications",
                                    "org.freedesktop.Notifications");
    QDBusMessage reply =
            notifyApp.callWithArgumentList(QDBus::AutoDetect, "Notify", argumentList);
    if (QDBusMessage::ErrorMessage == reply.type()) {
        qDebug() << "D-Bus Error:" << reply.errorMessage();
    }

    if (!QDBusConnection::sessionBus().isConnected()) {
        qDebug() << "QDBusConnection::sessionBus().isConnected() failed";
        return;
    }
    connect(&notifyApp, SIGNAL(ActionInvoked(uint, QString)), this,
            SLOT(slotActionInvoked(uint, QString)));
}


void MainWindow::on_update_DNS_List_clicked() {
    model->updateDNS();

}

void MainWindow::slotActionInvoked(uint id, QString action) {

    Q_UNUSED(id)
    if ("ShowWindow" == action) {
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
    //    Q_EMIT show();
}

void MainWindow::menuRequest(QPoint pos) {
    auto index=resultWidget->indexAt(pos);

    auto *menu=new QMenu(this);
    auto act=
    new QAction(tr("新增一行"),this);
    connect(act,&QAction::triggered,[this](){
        auto dns=QInputDialog::getText(this,tr("input DNS"),tr("DNS"));
        //TODO validator
        if(dns.isEmpty()){
            return;
        }
        this->model->insertRow(this->model->rowCount());
        QMap<int,QVariant> roles;
        roles.insert(Qt::DisplayRole,dns);
        qDebug()<<"roles:"<<roles;
        this->model->setItemData(model->index(model->rowCount()-1,0),roles);

    });
    menu->addAction(act);

    if(index.isValid()){
        act=new QAction(tr("删除该行"),this);
        connect(act,&QAction::triggered,[this,index](){
            this->model->removeRow(index.row());

        });
        menu->addAction(act);
    }
    menu->popup(resultWidget->viewport()->mapToGlobal(pos));


}
