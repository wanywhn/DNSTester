//
// Created by tender on 19-6-11.
//

#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include "DnsTableModel.h"

int DnsTableModel::rowCount(const QModelIndex &parent) const {
    return DnsList.size();
}

QVariant DnsTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    return headers.at(section);
}

int DnsTableModel::columnCount(const QModelIndex &parent) const {
    return headers.size();
}

QVariant DnsTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= DnsList.size()) {
        return {};
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    if (index.column() == 0) {
        return DnsList.at(index.row());

    } else if (index.column() == 1) {
        if (index.row() >= DnsFinalResult.size()) {
            return "Not Tested";
        } else {
            return DnsFinalResult.at(index.row());
        }

    }

}

DnsTableModel::DnsTableModel(QObject *parent) : QAbstractTableModel(parent) {
    QSettings settings;
    auto sl = settings.value("DNS").toStringList();
    if (sl.empty()) {
        sl << "114.114.114.114"
           << "114.114.115.115"
           << "223.5.5.5"
           << "223.6.6.6";
        settings.setValue("DNS", sl);
    }

    DnsList = sl;
    reserveSpace(DnsList.size());
    netManager = new QNetworkAccessManager(this);
}

void DnsTableModel::updateDNS() {
    if (testStarted) {
        return;
    }
    QNetworkRequest request(
            QUrl("https://gitee.com/wanywhn/DNSTester/raw/master/DNSList"));
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,
                      "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                      "(KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36");

    auto reply = netManager->get(request);
    connect(reply, &QNetworkReply::readyRead, [this, reply]() {
        QString line(reply->readAll());
        auto lines = line.split("\n");
        lines.removeDuplicates();
        lines.removeAll("");
        this->beginResetModel();
        DnsList.clear();
        initRes();

        DnsList.append(lines);
        QSettings settings;
        settings.setValue("DNS",DnsList);
        this->endResetModel();
        this->reserveSpace(DnsList.size());
//        ui->progressBar->setValue(0);
//        ui->progressBar->setMaximum(DnsCount * PingTimes);
//TODO clear tested result

        testStarted = false;
        disconnect(conn);

        qDebug() << lines;
        reply->deleteLater();

    });

}

void DnsTableModel::reserveSpace(int count) {

    vProcess.resize(count);
}

void DnsTableModel::initRes() {

    DnsFinalResult.clear();
    for (auto vp : vProcess) {
        delete vp;
    }
    vProcess.clear();
    timeouted = 1;

    testStarted = false;
}

void DnsTableModel::startTest() {
    auto rcnt = DnsFinalResult.size();
    DnsFinalResult.clear();
    dataChanged(index(0, 1), index(rcnt, 1));

    if (testStarted) {
        return;
    }
    initRes();
    testStarted = true;
    QString program = QString("ping -W 1 -c %L1 ").arg(PingTimes);

    conn = connect(this, &DnsTableModel::pingFinished,
                   [=]() { continueNext(program); });
    mConn.push_back(conn);
    //  mConn[0]=conn;
    //坑，多连接了几次
    startPing(program, 0);

}

/**
 * @brief MainWindow::startPing 开始测试服务器质量
 * @param program 测试语句，一般不变。还缺少服务器地址
 * @param index 服务器地址位于DNSList的索引值
 */
void DnsTableModel::startPing(QString program, int index) {

    auto *mPing = new QProcess(this);
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

/**
 * @brief MainWindow::processFinished 某一个服务器测试完成，进行数据更新
 * @param index 刚刚完成测试的服务器的索引值
 */
void DnsTableModel::processFinished(int index) {
    this->store(index);
    this->dataChanged(this->index(index, 1), this->index(index, 1));
    emit pingFinished();
    emit progressFinished(index);
}

/**
 * @brief MainWindow::store 在Ping的输出结果中截取有用信息
 * @param index
 */
void DnsTableModel::store(int index) {
    auto proc = vProcess.at(index);
    //  iter += index;
    QByteArray res = proc->readAllStandardOutput();
    QTextStream in(res);
    QString line;
    QRegularExpression re(
            QStringLiteral("\\d*\\.\\d*/\\d*\\.\\d*/\\d*\\.\\d*/\\d*\\.\\d* "));
    line = in.readAll();
    auto matches = re.match(line);
    auto result = matches.captured(0).trimmed();
    if (!matches.hasMatch() || result.isEmpty()) {
        DnsFinalResult.insert(index, "000/999/999/0");
    } else {
        DnsFinalResult.insert(index, result);
    }
}

/**
 * @brief MainWindow::continueNext
 * 前一个服务器已经测试完成，准备进行下一个（或者全部测试完成，做最终处理）
 * @param program 测试语句，一般不变。还缺少服务器地址。
 */
void DnsTableModel::continueNext(QString program) {
    if (++timeouted <= DnsList.size()) {
//            ui->progressBar->setValue((timeouted - 1) * PingTimes);
        startPing(program, timeouted - 1);
    } else {
        emit AllCompleted();
                foreach (conn, mConn) { disconnect(conn); }
        testStarted = false;
    }
}

bool DnsTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginResetModel();
    for(auto i=0;i!=count;++i){
        DnsList.insert(row,"");
    }
    endResetModel();
//    dataChanged(index(row,0),index(rowCount(),1));
    return true;

//    return QAbstractItemModel::insertRows(row, count, parent);
}

bool DnsTableModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) {
    if(!index.isValid()||index.row()>=DnsList.size()){
        return false;
    }
    for(auto item:roles.keys()){
        if(item==Qt::DisplayRole&&index.column()==0){
            DnsList[index.row()]=roles.value(item).toString();
        }else{

        }
    }
//    return QAbstractItemModel::setItemData(index, roles);
QSettings sett;
    sett.setValue("DNS",DnsList);
return true;
}

bool DnsTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginResetModel();
    for(auto i=0;i!=count;++i){
        DnsList.removeAt(row+i);
    }
    endResetModel();
    QSettings sett;
    sett.setValue("DNS",DnsList);
    return true;
//    dataChanged(index(row,0),index(rowCount(),1));
}


