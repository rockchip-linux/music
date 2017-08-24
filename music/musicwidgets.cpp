#include "musicwidgets.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>
#include <QMediaMetaData>

#include "global_value.h"

MusicWidgets::MusicWidgets(QWidget *parent):BaseWidget(parent)
{
    setObjectName("MusicWidgets");
    setStyleSheet("#MusicWidgets{border-image: url(:/image/music/music_bg.jpg);}"
                  "QLabel{color:white;}");

    initData();
    initLayout();
    initPlayerAndConnection();
    readSetting();
    setOriginState();
}

void MusicWidgets::readSetting()
{
    QSettings setting("config.ini",QSettings::IniFormat,0);
    setting.beginGroup("musicConfig");
    // Read play mode.
    int playModeIndex = 0;
    playModeIndex = setting.value("playmode").toInt();
    MediaList *playList = m_middlewid->getListWidget()->getMediaList();
    playList->setPlayMode((PlayMode)playModeIndex);
    m_bottomwid->updatePlayModeIcon(playList->getCurrentPlayMode());

    // Set volume saved in configration file.
    QFile *volumnFile;
    QDir settingsDir("/data/");
    if(settingsDir.exists()){
        volumnFile = new QFile("/data/volumn");
    }else{
        volumnFile = new QFile("/etc/volumn");
    }

    volumnFile->open(QFile::ReadOnly | QIODevice::Truncate);
    QByteArray readAll= volumnFile->readAll();
    QString volumnString(readAll);
    long volumnInt= volumnString.toInt();

    m_player->setVolume(volumnInt);
    m_bottomwid->updateVolumeSliderValue(m_player->volume());

    setting.endGroup();
}

void MusicWidgets::initData()
{
    m_refreshSuffixList.append("mp3");
    m_refreshSuffixList.append("wave");
    m_refreshSuffixList.append("wma");
    m_refreshSuffixList.append("ogg");
    m_refreshSuffixList.append("midi");
    m_refreshSuffixList.append("ra");
    m_refreshSuffixList.append("mod");
}

void MusicWidgets::initLayout()
{
    m_topwid = new TopWidgets(this);
    m_bottomwid = new BottomWidgets(this);
    m_middlewid = new MiddleWidgets(this);

    QVBoxLayout *vmainlyout=new QVBoxLayout;
    vmainlyout->addWidget(m_topwid);
    vmainlyout->addWidget(m_middlewid);
    vmainlyout->addWidget(m_bottomwid);
    vmainlyout->setSpacing(0);
    vmainlyout->setContentsMargins(0,0,0,0);
    setLayout(vmainlyout);
}

void MusicWidgets::initPlayerAndConnection()
{
    m_player = new QMediaPlayer;

    connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(slot_onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player,SIGNAL(error(QMediaPlayer::Error)),this,SLOT(slot_onErrorOn(QMediaPlayer::Error)));
    connect(m_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(slot_onStateChanged(QMediaPlayer::State)));
    connect(m_player,SIGNAL(metaDataAvailableChanged(bool)),this,SLOT(slot_onMetaDataAvailableChanged(bool)));
    connect(m_player,SIGNAL(positionChanged(qint64)),this,SLOT(slot_onPositonChanged(qint64)));
    connect(m_player,SIGNAL(durationChanged(qint64)),this,SLOT(slot_onDuratuonChanged(qint64)));

    connect(m_topwid,SIGNAL(returnClick()),this,SLOT(slot_exit()));

    connect(m_bottomwid,SIGNAL(nextClick()),this,SLOT(slot_nextSong()));
    connect(m_bottomwid,SIGNAL(lastClick()),this,SLOT(slot_preSong()));
    connect(m_bottomwid,SIGNAL(nextLongPressed()),this,SLOT(slot_fastForward()));
    connect(m_bottomwid,SIGNAL(lastLongPressed()),this,SLOT(slot_fastBackward()));
    connect(m_bottomwid,SIGNAL(playPauseClick()),this,SLOT(slot_playOrPause()));
    connect(m_bottomwid,SIGNAL(progressSliderPositionChanged(int)),this,SLOT(slot_changePlayerProgress(int)));
    connect(m_bottomwid,SIGNAL(volumeChanged(int)),this,SLOT(slot_volumeChanged(int)));
    connect(m_bottomwid,SIGNAL(playModeClick()),this,SLOT(slot_changePlayMode()));
    connect(m_bottomwid,SIGNAL(refreshClick()),this,SLOT(slot_refreshMediaResource()));

    connect(m_middlewid->getListWidget(),SIGNAL(tableClick(int,int)),this,SLOT(slot_onTableItemClicked(int,int)));
    connect(m_middlewid->getListWidget(),SIGNAL(tableLongPressed(int)),this,SLOT(slot_deleteTableItem(int)));
}

void MusicWidgets::slot_volumeChanged(int value)
{
    m_player->setVolume(value);
}

void MusicWidgets::slot_onErrorOn(QMediaPlayer::Error)
{
    m_player->setMedia(NULL);
    if(QMessageBox::Yes == QMessageBox::critical(mainWindow,"Format Error","It has encountered an error.",
                                                 QMessageBox::Yes | QMessageBox::Yes))
    {
        setOriginState();
    }
}

void MusicWidgets::slot_onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch(status){
    case QMediaPlayer::EndOfMedia:
        slot_nextSong();
        break;
    default:
        break;
    }
}

void MusicWidgets::setOriginState()
{
    m_bottomwid->setOriginState();
    m_middlewid->getListWidget()->tableUnfocus();
    m_middlewid->getLyricWidget()->setOriginState();
}

void MusicWidgets::slot_onStateChanged(QMediaPlayer::State state)
{
    if(state==QMediaPlayer::PlayingState){
        m_bottomwid->setPauseStyle();
    }else{
        m_bottomwid->setPlayStyle();
    }
}

void MusicWidgets::slot_onMetaDataAvailableChanged(bool available)
{
    if(available){
        QString mediaName = m_player->metaData(QMediaMetaData::Title).toString();
        if(mediaName!=NULL && mediaName !=""){
            m_middlewid->getLyricWidget()->setSongText(mediaName);
        }else{
            m_middlewid->getLyricWidget()->setSongText(m_player->currentMedia().canonicalUrl().fileName());
        }
        m_middlewid->getListWidget()->setPlayingMediaContent(m_player->currentMedia());
    }
}

void MusicWidgets::slot_onPositonChanged(qint64 position)
{
    m_bottomwid->onPlayerPositionChanged(position,m_player->duration());
}

void MusicWidgets::slot_onDuratuonChanged(qint64 duration)
{
    m_bottomwid->onPlayerDurationChanged(duration);
}

void MusicWidgets::slot_changePlayerProgress(int position)
{
    if(position >= 0){
        m_player->setPosition(position);
    }
}

void MusicWidgets::slot_changePlayMode()
{
    MediaList *playList = m_middlewid->getListWidget()->getMediaList();
    playList->changePlayMode();
    m_bottomwid->updatePlayModeIcon(playList->getCurrentPlayMode());
}

void MusicWidgets::slot_refreshMediaResource()
{
    bool isConfirm;
    QString appendSuffix = QInputDialog::getText(mainWindow,"Add Refresh Suffix",
                                                 "Please input extra file suffix",
                                                 QLineEdit::Normal,
                                                 "",
                                                 &isConfirm);
    if(isConfirm){
        if(!appendSuffix.isEmpty()){
            m_refreshSuffixList.append(appendSuffix);
        }
        mainWindow->slot_updateMedia();
    }
}

void MusicWidgets::slot_onTableItemClicked(int row,int)
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    QUrl url= playlist->getUrlAt(row);
    if(m_player->isAvailable()){
        m_player->setMedia(url);
        m_player->play();
    }
}

void MusicWidgets::slot_deleteTableItem(int row)
{
    m_middlewid->getListWidget()->deleteItem(row);
    m_middlewid->getListWidget()->setPlayingMediaContent(m_player->currentMedia());
}

void MusicWidgets::slot_fastForward()
{
    if(m_player->state()==QMediaPlayer::PlayingState||
            m_player->state()==QMediaPlayer::PausedState){
        m_player->setPosition(m_player->position()+5000);
    }
}

void MusicWidgets::slot_fastBackward()
{
    if(m_player->state()==QMediaPlayer::PlayingState||
            m_player->state()==QMediaPlayer::PausedState){
        m_player->setPosition(m_player->position()-5000);
    }
}

void MusicWidgets::slot_nextSong()
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    if(m_player->isAvailable()){
        m_player->setMedia(playlist->getNextSongUrl());
        m_player->play();
    }
    if(playlist->getUrlList().size() == 0){
        setOriginState();
    }
}

void MusicWidgets::slot_preSong()
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    if(m_player->isAvailable()){
        m_player->setMedia(playlist->getPreSongUrl());
        m_player->play();
    }
    if(playlist->getUrlList().size() == 0){
        setOriginState();
    }
}

void MusicWidgets::slot_playOrPause()
{
    if(m_player->state()==QMediaPlayer::PlayingState){
        m_player->pause();
    }else{
        if(m_player->isAudioAvailable() == true){
            m_player->play();
        }
    }
}

void MusicWidgets::updateUiByRes(QFileInfoList fileInfoList)
{
    m_middlewid->getListWidget()->updateLocalList(fileInfoList);

    if(m_player->currentMedia().canonicalUrl().toString()!=""){
        slot_onMetaDataAvailableChanged(true);
    }
}

void MusicWidgets::savaSetting()
{
    //Saved playmode and use for next time used.
    QSettings setting("config.ini",QSettings::IniFormat,0);
    setting.beginGroup("musicConfig");
    setting.setValue("playmode",(int)m_middlewid->getListWidget()->getMediaList()->getPlayMode());
    setting.endGroup();
}

void MusicWidgets::slot_exit()
{
    savaSetting();
    mainWindow->close();
}

QFileInfoList MusicWidgets::findMusicFiles(const QString& path)
{
    QFileInfoList musicFiles;

    QDirIterator it(path,QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    while(it.hasNext()){
        QString name = it.next();
        QFileInfo info(name);
        if (info.isDir()){
            musicFiles.append(findMusicFiles(name));
        }else{
            for(int i=0;i<m_refreshSuffixList.count();i++){
                if(info.suffix().compare(m_refreshSuffixList.at(i),Qt::CaseInsensitive) == 0){
                    musicFiles.append(info);
                }
            }
        }
    }
    return musicFiles;
}

void MusicWidgets::updateVolume(bool volumeAdd)
{
    if(volumeAdd){
        if(m_player->volume()<95){
            m_player->setVolume(m_player->volume()+5);
        }else{
            m_player->setVolume(100);
        }
    }else{
        if(m_player->volume()>5){
            m_player->setVolume(m_player->volume()-5);
        }else{
            m_player->setVolume(0);
        }
    }
    m_bottomwid->updateVolumeSliderValue(m_player->volume());
}

MusicWidgets::~MusicWidgets()
{
}
