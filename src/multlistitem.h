#ifndef MULTLISTITEM_H
#define MULTLISTITEM_H

#include <DSimpleListItem>
#include <QObject>

DWIDGET_USE_NAMESPACE
class MultiListItem : public DSimpleListItem
{
    Q_OBJECT
public:
    MultiListItem(QString DNSAddr,QString result);

    // DSimpleListItem interface
public:
    bool sameAs(DSimpleListItem *item) override;
    void drawBackground(QRect rect, QPainter *painter, int index, bool isSelect) override;
    void drawForeground(QRect rect, QPainter *painter, int column, int index, bool isSelect) override;
    void setTextColor(bool f,QColor color);

    QString mDnsServer;
    QString mresult;
    bool needTextColor;
    QColor mcolor;
};

#endif // MULTLISTITEM_H
