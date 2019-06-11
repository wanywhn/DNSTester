//
// Created by tender on 19-6-11.
//

#ifndef DNSTESTER_DNSTABLEMODEL_H
#define DNSTESTER_DNSTABLEMODEL_H


#include <QtCore/QAbstractTableModel>
#include <QtCore/QProcess>
#include <QNetworkAccessManager>

class DnsTableModel : public QAbstractTableModel {
Q_OBJECT
public:
    DnsTableModel(QObject *parent);

    int rowCount(const QModelIndex &parent=QModelIndex()) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void updateDNS();

    void startTest();

    bool isTestStarted() {
        return testStarted;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;
signals:

    void pingFinished();

    void AllCompleted();
    void progressFinished(int i);



private:

    void processFinished(int index);

    void store(int index);

    void continueNext(QString program);

    void startPing(QString program, int index);

    void initRes();

    void reserveSpace(int count);

    int timeouted;
    int PingTimes{3};
    QStringList DnsList;
    bool testStarted{false};

    QMetaObject::Connection conn;
    QVector<QMetaObject::Connection> mConn;
    QNetworkAccessManager *netManager;

    QVector<QProcess *> vProcess;
    QVector<QString> DnsFinalResult;
    QStringList headers{tr("服务器地址"), tr("最优/平均/最差/方差")};

};


#endif //DNSTESTER_DNSTABLEMODEL_H
