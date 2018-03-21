#ifndef MULTILISTVIEW_H
#define MULTILISTVIEW_H

#include <DSimpleListView>
#include <QObject>

#include "multlistitem.h"

DWIDGET_USE_NAMESPACE
class MultiListView : public DSimpleListView {
public:
  MultiListView(QWidget *parent = 0);
  ~MultiListView();
};

#endif // MULTILISTVIEW_H
