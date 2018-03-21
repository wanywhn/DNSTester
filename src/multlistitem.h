#ifndef MULTLISTITEM_H
#define MULTLISTITEM_H

#include <DSimpleListItem>
#include <QObject>

DWIDGET_USE_NAMESPACE
class MultiListItem : public DSimpleListItem {
  Q_OBJECT
public:
  MultiListItem(QString DNSAddr, QString result);

  // DSimpleListItem interface
public:
  bool sameAs(DSimpleListItem *item) override;
  void setTextColor(bool f, QColor color);

  QString mDnsServer;
  QString mresult;
  bool needTextColor;
  QColor mcolor;

  // DSimpleListItem interface
public:
  virtual void drawBackground(QRect rect, QPainter *painter, int index,
                              bool isSelect, bool isHover) override;
  virtual void drawForeground(QRect rect, QPainter *painter, int column,
                              int index, bool isSelect, bool isHover) override;
};

#endif // MULTLISTITEM_H
