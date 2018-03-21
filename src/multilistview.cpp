#include "multilistview.h"

MultiListView::MultiListView(QWidget *parent) : DSimpleListView(parent) {

  QList<QString> headers;
  headers << "ceshi"
          << "服务器地址"
          << "最优/平均/最差/方差";
  QList<int> widths;
  widths << 300 << -1 << 300;
  // TODO WHAT???
  setColumnTitleInfo(headers, widths, 36);
}

MultiListView::~MultiListView() {}
