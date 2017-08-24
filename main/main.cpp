#include "mainwindow.h"
#include <QApplication>

#include "ueventthread.h"
#include "inotifythread.h"
#include "global_value.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Start media source update thread.
    // Uevent for usb and inotify for file modify.
    UeventThread thread;
    thread.start();
    InotifyThread thread2;
    thread2.start();

    // Change app font family and size to supprot all device.
    QFont appFont = app.font();
    appFont.setPixelSize(font_size);
    app.setFont(appFont);

    MainWindow w;
#ifdef DEVICE_EVB
    w.showMaximized();
#else
    w.showFullScreen();
#endif
    return app.exec();
}
