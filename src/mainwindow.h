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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void findNetworkInterface();

  void startPing(QString program, int index);

  void addItemtoTableWidget();

  void continueNext(QString program);

  void init();

  void setSelectItemColor(QColor color);

  void reserveSpace(int count);

signals:
  void pingFinished();
private slots:
  void processFinished(int index);
  //    void readyRead();
  void startTest();
  void setDns();
  void on_update_DNS_List_clicked();
  void replyFinished(QNetworkReply *reply);

private:
  void store(int index);
  void cal(int index);

private:
  Ui::MainWindow *ui;
  int IndexStackedWidget;
  int PingTimes;
  //注意更新
  int DnsCount;

  QStringList DnsList;
  QStringList DnsResult;
  QVector<double> DnsNumResult;

  int timeouted;
  QString dnsSelected;
  int dnsSelectedId;
  QTableWidget *resultWidget;
  QVector<QProcess *> vProcess;
  QVector<QMetaObject::Connection> mConn;

  bool testStarted;
  QProgressBar *mQProgressbar;
  QNetworkAccessManager *netManager;
  QMetaObject::Connection conn;

  NetworkUtils *utils;
  static bool clickedSetDns;
};

#endif // MAINWINDOW_H
