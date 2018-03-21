#include "networkutils.h"

#include <QProcess>

NetworkUtils::NetworkUtils(QObject *parent) : QObject(parent) {}

void NetworkUtils::ChangeDNSTo(QHostAddress &addr, QString &uuid) {

  QString cmd = "nmcli con mod " + uuid + " ipv4.dns " + addr.toString() +
                " ipv4.ignore-auto-dns yes ";
  QProcess *changeDns = new QProcess(this);
  changeDns->start(cmd);
  QProcess *restartNetwork = new QProcess(this);

  cmd = "nmcli con up " + uuid;
  connect(restartNetwork,
          static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(
              &QProcess::finished),
          [this](int ecode, QProcess::ExitStatus stat) {
            if (stat == QProcess::NormalExit) {
              qDebug() << ecode;
              emit restartSuccessed();
            }
          });
  restartNetwork->start(cmd);
}

QStringList NetworkUtils::findNetworkInterface() {

  QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
  QStringList res;
  for (auto interface : list) {
    qDebug() << interface.name();
    QList<QNetworkAddressEntry> entryList = interface.addressEntries();
    for (auto entry : entryList) {
      if (entry.ip().toString() == "" || entry.ip().isLoopback() ||
          entry.broadcast().toString() == "") {
        entryList.removeOne(entry);
        continue;
      }
      qDebug() << entry.ip().toString() << entry.netmask().toString()
               << entry.broadcast().toString();
      res << interface.name() + ":" + entry.ip().toString();
    }
  }
  return res;
}
