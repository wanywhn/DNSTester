#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    w.resize(w.sizeHint());
    w.setMinimumSize(w.sizeHint());
    w.setMaximumSize(w.sizeHint());
    w.move((qApp->desktop()->availableGeometry().width()-w.width())/2
         +qApp->desktop()->availableGeometry().x()
             ,(qApp->desktop()->availableGeometry().height()-w.height())/2
         +qApp->desktop()->availableGeometry().y()
         );

    return a.exec();
}
