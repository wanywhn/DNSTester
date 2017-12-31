#include "mainwindow.h"

#include <DApplication>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[]) {
  DApplication::loadDXcbPlugin();
  DApplication a(argc, argv);
  a.setAttribute(Qt::AA_EnableHighDpiScaling, true);
  a.setApplicationName("DNSTester");
  a.setApplicationVersion("1.2");
  a.setTheme("light");

  const char *descriptionText =
      QT_TRANSLATE_NOOP("MainWindow", "DNSTester is...");
  a.setApplicationDescription(
      DApplication::translate("MainWindow", descriptionText) + "\n");
  const QString HomePage = "https://gitee.com/wanywhn/DNSTester";
  a.setApplicationHomePage(HomePage);

  a.setOrganizationName("WANYWHN");
  a.setOrganizationDomain("https://gitee.com/wanywhn");
  if (!a.setSingleInstance("DNSTester_wanywhn")) {
    qDebug() << "another instance is running";
    return 0;
  }

  MainWindow w;
  w.show();
  w.setMinimumSize(w.sizeHint());
  w.setMaximumSize(w.sizeHint());

  Dtk::Widget::moveToCenter(&w);
  return a.exec();
}
