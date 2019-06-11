#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "networkutils.h"
#include "DnsTableModel.h"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QProgressBar>
#include <QQueue>
#include <QTableWidget>
#include <QVector>

#include <DMainWindow>
#include <DSimpleListView>
#include "DnsTableModel.h"

#include <QTableView>

namespace Ui {
    class MainWindow;
}

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void findNetworkInterface();


    void initUI();


private slots:
    void menuRequest(QPoint pos);

    void startTest();

    void setDns();

    void on_update_DNS_List_clicked();

    void slotActionInvoked(uint id, QString action);

private:
    void notifyActivator(QString title, QString text);

private:
    Ui::MainWindow *ui;
    int IndexStackedWidget;
    //注意更新
    int DnsCount;


    QTableView *resultWidget;
    DnsTableModel *model;

    NetworkUtils *utils;
    static bool clickedSetDns;
};

#endif // MAINWINDOW_H
