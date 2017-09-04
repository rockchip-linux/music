#include "mainwindow.h"
#include "global_value.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :BaseWindow(parent)
  ,mediaHasUpdate(false)
{
    initData();
    initLayout();
    initConnection();
    slot_updateMedia();
}

void MainWindow::initData()
{
    // Initialize global main class of 'MainWindow' for other widgets invokes.
    mainWindow = this;
    // Start media source update thread.
    // Uevent for usb and inotify for file modify.
    ueventThread = new UeventThread(this);
    ueventThread->start();
    inotifyThread = new InotifyThread(this);
    inotifyThread->start();
}

void MainWindow::initLayout(){    
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_musicWid = new MusicWidgets(this);

    mainLayout->addWidget(m_musicWid);
    mainLayout->setContentsMargins(0,0,0,0);
    setLayout(mainLayout);
}

void MainWindow::initConnection()
{
    connect(this,SIGNAL(beginUpdateMediaResource()),this,SLOT(slot_setUpdateFlag()));
    connect(this,SIGNAL(updateUiByRes(QFileInfoList)),this,SLOT(slot_updateUiByRes(QFileInfoList)));
}

void MainWindow::slot_setUpdateFlag()
{
    /*
     * This operation setted because that inotify event send no more one siganl.
     * So set a 500ms duration to ignore theres no-use siganls.
     * Note: it is expected to optimize.
     */
    if(!mediaHasUpdate)
    {
        mediaHasUpdate = true;
        QTimer::singleShot(500,this,SLOT(slot_updateMedia()));
    }
}

void MainWindow::slot_updateMedia()
{
    qDebug()<<"Update media resource.";
    MediaUpdateThread *thread = new MediaUpdateThread(this,this);
    thread->start();
    mediaHasUpdate =false;
}

void MainWindow::slot_updateUiByRes(QFileInfoList musicFileList)
{
    m_musicWid->updateUiByRes(musicFileList);
}

void MainWindow::disableApplication()
{
    qDebug("disable music application");
    this->setVisible(false);
}

void MainWindow::enableApplication()
{
    qDebug("enable music application");
    this->setVisible(true);
}

void MainWindow::onApplicationClose()
{
    this->close();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_VolumeDown:
        m_musicWid->updateVolume(false);
        break;
    case Qt::Key_VolumeUp:
        m_musicWid->updateVolume(true);
        break;
    case Qt::Key_PowerOff:
        // When key_power enter
        QTimer::singleShot(100, this, SLOT(slot_standby()));
        break;
    default:
        break;
    }
}

void MainWindow::slot_standby()
{
    system("echo mem > /sys/power/state");
}

MainWindow::~MainWindow()
{
}

MediaUpdateThread::MediaUpdateThread(QObject *parent,MainWindow *mainWindow):QThread(parent)
{
    m_parent = mainWindow;
    qRegisterMetaType<QFileInfoList>("QFileInfoList");
}

void MediaUpdateThread::run()
{
    QFileInfoList musicFileList = m_parent->getMusicWidget()->findMusicFiles(MUSIC_SEARCH_PATH);
    emit m_parent->updateUiByRes(musicFileList);
}
