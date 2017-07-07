#include "mainwindow.h"
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include "ueventthread.h"
#include "inotifythread.h"
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);



    QFont appFont = a.font();
    appFont.setFamily("System");
    a.setFont(appFont);

    MainWindow w;
    //w.showFullScreen();



    w.showMaximized();
    ueventThread thread;
    thread.start();

    inotifyThread thread2;
    thread2.start();

    return a.exec();

}
