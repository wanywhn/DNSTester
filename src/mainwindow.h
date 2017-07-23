#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>
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

private slots:
    void startTest();
    void setDns();
private:
    void store();
    void cal();
private:
    Ui::MainWindow *ui;
    int IndexStackedWidget;
    int PingTimes;
    int DnsCount;

    QStringList DnsList;
    QStringList DnsResult;
    QVector<double> DnsNumResult;

    QString dnsSelected;
    int dnsSelectedId;

    int numProc;
    QVector<QProcess *> vProcess;

    bool testStarted;
    QProgressBar *mQProgressbar;
};

#endif // MAINWINDOW_H
