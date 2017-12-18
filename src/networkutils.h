#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QHostAddress>
#include <QNetworkInterface>
#include <QObject>

class NetworkUtils : public QObject {
  Q_OBJECT
public:
  explicit NetworkUtils(QObject *parent = nullptr);
  void ChangeDNSTo(QHostAddress &addr, QString &uuid);
  QStringList findNetworkInterface();

signals:
  void restartSuccessed();

public slots:
};

#endif // NETWORKUTILS_H
