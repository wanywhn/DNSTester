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
  DApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  DApplication::setOrganizationName("WANYWHN");
  DApplication::setApplicationName("DNSTester");
  DApplication::setApplicationVersion(D_VERSION);
  a.setTheme("light");

//  const char *descriptionText =
//      QT_TRANSLATE_NOOP("MainWindow", "DNSTester is...");
//  a.setApplicationDescription(
//      DApplication::translate("MainWindow", descriptionText) + "\n");

  const QString HomePage = "https://gitee.com/wanywhn/DNSTester";
  auto *about=new DAboutDialog();
  about->setWebsiteLink(HomePage);
  about->setWebsiteName(HomePage);
  about->setProductName("DNS Tester");
  about->setDescription(QObject::tr("DNS 测速与更换小工具"));
  about->setAcknowledgementVisible(false);
  about->setProductIcon(QIcon(":/icon/resource/icon/dns-tester.png"));
  about->setCompanyLogo(QPixmap());

  a.setAboutDialog(about);


  MainWindow w;
  w.show();
  w.setMinimumSize(w.sizeHint());
  w.setMaximumSize(w.sizeHint());

  Dtk::Widget::moveToCenter(&w);
  return Dtk::Widget::DApplication::exec();
}
