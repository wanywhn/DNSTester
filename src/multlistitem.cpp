#include "multlistitem.h"

MultiListItem::MultiListItem(QString DNSAddr, QString result)
    : mDnsServer(DNSAddr), mresult(result) {needTextColor=false;}

bool MultiListItem::sameAs(DSimpleListItem *item) {

    auto myitem=static_cast<MultiListItem *>(item);
  if (nullptr ==myitem)
    return false;
  return (this->mresult == myitem->mresult)&&
         (this->mDnsServer == myitem->mDnsServer);
}

void MultiListItem::drawBackground(QRect rect, QPainter *painter, int index,
                                   bool isSelect) {
  QPainterPath path;
  path.addRect(QRectF(rect));

  painter->setOpacity(1);
  if (isSelect) {
    painter->fillPath(path, QColor("#2CA7F8"));
  } else if (index % 2 == 1) {
    painter->fillPath(path, QColor("#D8D8D8"));
  }
}

void MultiListItem::drawForeground(QRect rect, QPainter *painter, int column,
                                   int index, bool isSelect) {
  int padding = 10;
  painter->setOpacity(1);
  if (isSelect) {
    painter->setPen(QPen(QColor("#FFFFFF")));
  } else {
    painter->setPen(QPen(QColor("#000000")));
  }
  if(needTextColor)
      painter->setPen(mcolor);
  if (column == 1) {
    painter->drawText(QRect(rect.x() + padding, rect.y(),
                            rect.width() - padding * 2, rect.height()),
                      Qt::AlignLeft| Qt::AlignVCenter, mDnsServer);
  } else if (column == 2) {
    painter->drawText(QRect(rect.x() + padding, rect.y(),
                            rect.width() - padding * 2, rect.height()),
                      Qt::AlignLeft| Qt::AlignVCenter, mresult);
  }
}

void MultiListItem::setTextColor(bool f, QColor color)
{
    needTextColor=f;
    mcolor=color;
}
