#include "mainwindow.h"
#include "global_value.h"
#include "ueventthread.h"
#include "inotifythread.h"

MainWindow::MainWindow(QWidget *parent) :baseWindow(parent),mediaHasUpdate(false)

{
    mainwid=this;
    initLayout();
    connect(m_musicWid->m_topwid->m_btnmini,SIGNAL(clicked(bool)),this,SLOT(showMinimized()));
    connect(m_musicWid->m_topwid->m_btnexit,SIGNAL(clicked(bool)),this,SLOT(slot_appQuit()));
    connect(m_musicWid->m_topwid->m_btnreturn,SIGNAL(clicked(bool)),this,SLOT(slot_returnanimation()));
    connect(this,SIGNAL(beginUpdateMediaResource()),this,SLOT(slot_updateMedia1()));
    connect(this,SIGNAL(updateUiByRes(QFileInfoList)),this,SLOT(slot_updateUiByRes(QFileInfoList)));
    slot_updateMedia2();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initLayout(){    
   QVBoxLayout *mainLayout = new QVBoxLayout;
    m_musicWid = new musicWidgets(this);    
    mainLayout->addWidget(m_musicWid);
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);
}
void MainWindow::slot_appQuit()
{
    m_musicWid->savaSetting();
    this->close();
}
void MainWindow::slot_returnanimation()
{
     this->close();
}
void MainWindow::slot_updateMedia1()
{
    if(!mediaHasUpdate)
    {
        mediaHasUpdate = true;
        QTimer::singleShot(2000,this,SLOT(slot_updateMedia2()));
    }
}
void MainWindow::slot_updateMedia2()
{
    qDebug()<<"Carmachine: Update media resource.";
    mediaUpdateThread *thread = new mediaUpdateThread(this,this);
    thread->start();
    mediaHasUpdate =false;
}
mediaUpdateThread::mediaUpdateThread(QObject *parent,MainWindow *mainWindow):QThread(parent)
{
    m_mainWindow = mainWindow;
    qRegisterMetaType<QFileInfoList>("QFileInfoList");
    qRegisterMetaType<QMap<QString,QImage>>("QMap<QString,QImage>");
}
void mediaUpdateThread::run()
{
    QFileInfoList musicFileList = m_mainWindow->m_musicWid->m_middlewid->m_leftWid->m_Swidget0->findMusicFiles(MUSIC_SEARCH_PATH);
    emit m_mainWindow->updateUiByRes(musicFileList);
}

void MainWindow::slot_updateUiByRes(QFileInfoList musicFileList)
{
    m_musicWid->m_middlewid->m_leftWid->m_Swidget0->updateResUi(musicFileList);

    if(m_musicWid->getPlayer()->currentMedia().canonicalUrl().toString()!="")
    {
        m_musicWid->slot_onCurrentMediaChanged(m_musicWid->getPlayer()->currentMedia());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"key value is:"<<event->key();
    switch(event->key())
    {
    // update musicPlayer and videoPlayer's volume by Key
    case Qt::Key_VolumeDown:

        m_musicWid->updateVolume(false);
        QWidget::keyPressEvent(event);
        break;
    case Qt::Key_VolumeUp:

        m_musicWid->updateVolume(true);

        QWidget::keyPressEvent(event);
        break;
    case Qt::Key_PowerOff:   // when key_power enter
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
