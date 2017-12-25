#ifndef MULTILISTVIEW_H
#define MULTILISTVIEW_H

#include <DSimpleListView>
#include <QObject>

#include "multlistitem.h"

DWIDGET_USE_NAMESPACE
class MultiListView : public DSimpleListView
{
public:
    MultiListView(DSimpleListView *parent=0);
};

#endif // MULTILISTVIEW_H
