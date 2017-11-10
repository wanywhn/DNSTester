#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>
#include <QQueue>
#include <QTableWidget>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void nmcliCall();

    void findNetworkInterface();

    void startPing(QString program, int index);

public slots:
    void processFinished(int index);
signals:
    void pingFinished();
private slots:
    void startTest();
    void setDns();
private:
    void store(int index);
    void cal();
private:
    Ui::MainWindow *ui;
    int IndexStackedWidget;
    int PingTimes;
    int DnsCount;

    QStringList DnsList;
    QStringList DnsResult;
    QQueue<double> DnsNumResult;

    QString dnsSelected;
    int dnsSelectedId;
QTableWidget * resultWidget;
    QVector<QProcess *> vProcess;

    bool testStarted;
    QProgressBar *mQProgressbar;
};

#endif // MAINWINDOW_H
