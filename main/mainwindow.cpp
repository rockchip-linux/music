#include "mainwindow.h"
#include "global_value.h"
#include <QVBoxLayout>
#include <QDir>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent) : BaseWindow(parent)
  , mediaHasUpdate(false)
  , mediaUpdateThread(0)
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
    m_notificationReceiver.receive();

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
    connect(&m_notificationReceiver,SIGNAL(mediaNotification(MediaNotification*)),this,SLOT(slot_setUpdateFlag()));
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
    qDebug("Update media resource.");
    if (mediaUpdateThread) {
        delete mediaUpdateThread;
        mediaUpdateThread = 0;
    }

    mediaUpdateThread = new MediaUpdateThread(this,this);
    mediaUpdateThread->start();
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
    if (mediaUpdateThread && mediaUpdateThread->isRunning())
        mediaUpdateThread->waitForThreadFinished();

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
    default:
        break;
    }
}

MainWindow::~MainWindow()
{
}

MediaUpdateThread::MediaUpdateThread(QObject *parent, MainWindow *mainWindow):QThread(parent)
{
    m_parent = mainWindow;
    qRegisterMetaType<QFileInfoList>("QFileInfoList");

    m_searchSuffixList.append("mp3");
    m_searchSuffixList.append("wave");
    m_searchSuffixList.append("wma");
    m_searchSuffixList.append("ogg");
    m_searchSuffixList.append("midi");
    m_searchSuffixList.append("ra");
    m_searchSuffixList.append("mod");
    m_searchSuffixList.append("mp1");
    m_searchSuffixList.append("mp2");
    m_searchSuffixList.append("wav");
    m_searchSuffixList.append("flac");
    m_searchSuffixList.append("aac");
    m_searchSuffixList.append("m4a");
}

void MediaUpdateThread::waitForThreadFinished()
{
    requestInterruption();
    quit();
    wait();
}

QFileInfoList MediaUpdateThread::findMusicFiles(const QString &path)
{
    QFileInfoList musicFiles;

    QDirIterator it(path, QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    while (it.hasNext() && !isInterruptionRequested()){
        QString name = it.next();
        QFileInfo info(name);
        if (info.isDir()){
            musicFiles.append(findMusicFiles(name));
        }
        else{
            for(int i = 0; i < m_searchSuffixList.count(); i++){
                if(info.suffix().compare(m_searchSuffixList.at(i), Qt::CaseInsensitive) == 0){
                    musicFiles.append(info);
                }
            }
        }
    }
    return musicFiles;
}

void MediaUpdateThread::run()
{
    QFileInfoList musicFileList = findMusicFiles(MUSIC_SEARCH_PATH);
    if (!isInterruptionRequested())
        emit m_parent->updateUiByRes(musicFileList);
}
