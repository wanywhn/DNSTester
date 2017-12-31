#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "networkutils.h"

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

  void startPing(QString program, int index);

  void addItemtoTableWidget();

  void continueNext(QString program);

  void initRes();

  void setSelectItemColor(bool f,QColor color);

  void reserveSpace(int count);

  void initUI();

signals:
  void pingFinished();
private slots:
  void processFinished(int index);
  void startTest();
  void setDns();
  void on_update_DNS_List_clicked();
  void replyFinished(QNetworkReply *reply);
  void slotActionInvoked(uint id,QString action);

private:
  void store(int index);
  void updateListViewItems(int index, QString str);
  void notifyActivator(QString title,QString text);

private:
  Ui::MainWindow *ui;
  int IndexStackedWidget;
  int PingTimes;
  //注意更新
  int DnsCount;

  QStringList DnsList;
  QVector<QString> DnsFinalResult;
  QVector<double> DnsNumberResult;

  int timeouted;
  QString dnsSelected;
  int dnsSelectedId;
  DSimpleListView *resultWidget;
  QVector<QProcess *> vProcess;
  QVector<QMetaObject::Connection> mConn;
  QList<DSimpleListItem *> listViewItems;
  bool testStarted;
  QProgressBar *mQProgressbar;
  QNetworkAccessManager *netManager;
  QMetaObject::Connection conn;

  NetworkUtils *utils;
  static bool clickedSetDns;
};

#endif // MAINWINDOW_H
