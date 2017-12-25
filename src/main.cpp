#include "mainwindow.h"

#include <DApplication>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    a.setAttribute(Qt::AA_EnableHighDpiScaling,true);
    a.setTheme("light");

    if(!a.setSingleInstance("DNSTester_wanywhn")){
        qDebug()<<"another instance is running";
    }

    MainWindow w;
    w.show();
    w.setMinimumSize(w.sizeHint());
    w.setMaximumSize(w.sizeHint());

    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
