#include "mainwindow.h"

#include "config.h"
#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[]) {
  DApplication::loadDXcbPlugin();
  DApplication a(argc, argv);
  if (!a.setSingleInstance("DNSTester_wanywhn")) {
    qDebug() << "another instance is running";
    return 0;
  }
  Dtk::Widget::DApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  Dtk::Widget::DApplication::setApplicationName("DNSTester");
  Dtk::Widget::DApplication::setApplicationVersion(D_VERSION);
  a.setTheme("light");

  const char *descriptionText =
      QT_TRANSLATE_NOOP("MainWindow", "DNSTester is...");
  a.setApplicationDescription(
      DApplication::translate("MainWindow", descriptionText) + "\n");

  const QString HomePage = "https://gitee.com/wanywhn/DNSTester";
  auto *about=new DAboutDialog();
  about->setWebsiteLink(HomePage);
  about->setWebsiteName(HomePage);
  about->setCompanyLogo(QPixmap(":/icon/resource/icon/icons8-DNS-50.png"));

  a.setAboutDialog(about);


  MainWindow w;
  w.show();
  w.setMinimumSize(w.sizeHint());
  w.setMaximumSize(w.sizeHint());

  Dtk::Widget::moveToCenter(&w);
  return Dtk::Widget::DApplication::exec();
}
