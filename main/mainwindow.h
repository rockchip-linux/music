#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>

#include "musicwidgets.h"
#include "base/basewindow.h"

/**
 * The main window of application.
 *
 * Used for global control.such as keypress response、media update、
 * initial interface etc.
 */
class MainWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MusicWidgets* getMusicWidget(){return m_musicWid;}
private:
    MusicWidgets *m_musicWid;
    bool mediaHasUpdate;

    void initLayout();
    void initConnection();
protected:
    void keyPressEvent(QKeyEvent *event);
public slots:
    void slot_setUpdateFlag();
    void slot_updateMedia();
    void slot_updateUiByRes(QFileInfoList musicFileList);
    void slot_standby();
signals:
    void beginUpdateMediaResource();
    void updateUiByRes(QFileInfoList musicFileList);
};

class MediaUpdateThread:public QThread
{
public:
    MediaUpdateThread(QObject *parent ,MainWindow *mainWindow);
private:
    MainWindow *m_parent;
protected:
    void run();
};
#endif // MAINWINDOW_H
