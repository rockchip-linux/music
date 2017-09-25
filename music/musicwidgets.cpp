#include "musicwidgets.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextStream>

#include "global_value.h"

MusicWidgets::MusicWidgets(QWidget *parent):BaseWidget(parent)
{
    setObjectName("MusicWidgets");
    setStyleSheet("QLabel{color:white;}");

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
    // Send current playMode back to service.
    m_player->currentPlayModeChanged(playList->getCurrentPlayMode());

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
    m_bottomwid->updateVolumeSliderValue(volumnInt);

    setting.endGroup();
}

void MusicWidgets::initData()
{
    m_player = new MusicPlayer(this);
    m_player->connectToService();

    m_refreshSuffixList.append("mp3");
    m_refreshSuffixList.append("wave");
    m_refreshSuffixList.append("wma");
    m_refreshSuffixList.append("ogg");
    m_refreshSuffixList.append("midi");
    m_refreshSuffixList.append("ra");
    m_refreshSuffixList.append("mod");
    m_refreshSuffixList.append("mp1");
    m_refreshSuffixList.append("mp2");
    m_refreshSuffixList.append("wav");
    m_refreshSuffixList.append("flac");
    m_refreshSuffixList.append("aac");
    m_refreshSuffixList.append("m4a");
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
    connect(m_player,SIGNAL(mediaStatusChanged(MusicPlayer::MediaStatus)),this,SLOT(slot_onMediaStatusChanged(MusicPlayer::MediaStatus)));
    connect(m_player,SIGNAL(error(MusicPlayer::Error)),this,SLOT(slot_onErrorOn(MusicPlayer::Error)));
    connect(m_player,SIGNAL(stateChanged(MusicPlayer::State)),this,SLOT(slot_onStateChanged(MusicPlayer::State)));
    connect(m_player,SIGNAL(metaDataAvailable()),this,SLOT(slot_onMetaDataAvailable()));
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
    saveVolume(value);
}

void MusicWidgets::slot_onErrorOn(MusicPlayer::Error)
{
    m_player->setMedia(NULL);
    if(QMessageBox::Yes == QMessageBox::critical(mainWindow,"格式问题","音频格式错误",
                                                 QMessageBox::Yes | QMessageBox::Yes))
    {
        setOriginState();
    }
}

void MusicWidgets::slot_onMediaStatusChanged(MusicPlayer::MediaStatus status)
{
    switch(status){
    case MusicPlayer::EndOfMedia:
        slot_nextSong();
        break;
    default:
        break;
    }
}

void MusicWidgets::setOriginState()
{
    m_bottomwid->setOriginState();
    m_middlewid->getListWidget()->setOriginState();
    m_middlewid->getLyricWidget()->setOriginState();
}

void MusicWidgets::slot_onStateChanged(MusicPlayer::State state)
{
    if(state == MusicPlayer::PlayingState){
        m_bottomwid->setPauseStyle();
    }else{
        m_bottomwid->setPlayStyle();
    }
}

void MusicWidgets::slot_onMetaDataAvailable()
{
    QString mediaName = m_player->currentMedia();
    m_middlewid->getLyricWidget()->currentMediaChanged(m_player->getMediaTitle(),mediaName);
    m_middlewid->getListWidget()->setPlayingMediaContent(mediaName);
}

void MusicWidgets::slot_onPositonChanged(qint64 position)
{
    m_bottomwid->onPlayerPositionChanged(position,m_player->duration());
    m_middlewid->getLyricWidget()->onCurrentPositionChanged(position);
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
    // Send current playMode back to service.
    m_player->currentPlayModeChanged(playList->getCurrentPlayMode());
}

void MusicWidgets::slot_refreshMediaResource()
{
    bool isConfirm;
    QString appendSuffix = QInputDialog::getText(mainWindow,"添加过滤后缀",
                                                 "输入新增过滤后缀,留空表示直接刷新",
                                                 QLineEdit::Normal,
                                                 "",
                                                 &isConfirm);
    if(isConfirm){
        if(!appendSuffix.isEmpty()){
            if(appendSuffix.contains(".")){
                QFileInfo fileInfo(appendSuffix);
                appendSuffix=fileInfo.suffix();
            }
            m_refreshSuffixList.append(appendSuffix);
        }
        mainWindow->slot_updateMedia();
    }
}

void MusicWidgets::slot_onTableItemClicked(int row,int)
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    QString filePath= playlist->getPathAt(row);
    if(m_player->isAvailable()){
        m_player->setMedia(filePath);
        m_player->play();
    }
}

void MusicWidgets::slot_deleteTableItem(int row)
{
    m_middlewid->getListWidget()->deleteItem(row);
}

void MusicWidgets::slot_fastForward()
{
    if(m_player->state() == MusicPlayer::PlayingState||
            m_player->state() == MusicPlayer::PausedState){
        m_player->setPosition(m_player->position() + 5000);
    }
}

void MusicWidgets::slot_fastBackward()
{
    if(m_player->state() == MusicPlayer::PlayingState||
            m_player->state() == MusicPlayer::PausedState){
        m_player->setPosition(m_player->position()-5000);
    }
}

void MusicWidgets::slot_nextSong()
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    if(m_player->isAvailable()){
        m_player->setMedia(playlist->getNextSongPath());
        m_player->play();
    }
    if(playlist->getPathList().size() == 0){
        setOriginState();
    }
}

void MusicWidgets::slot_preSong()
{
    m_player->stop();
    MediaList *playlist = m_middlewid->getListWidget()->getMediaList();
    if(m_player->isAvailable()){
        m_player->setMedia(playlist->getPreSongPath());
        m_player->play();
    }
    if(playlist->getPathList().size() == 0){
        setOriginState();
    }
}

void MusicWidgets::slot_playOrPause()
{
    if(m_player->state()==MusicPlayer::PlayingState){
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

    if(m_player->currentMedia() != ""){
        slot_onMetaDataAvailable();
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
    m_player->clientExit();
    savaSetting();
    mainWindow->onApplicationClose();
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
    int currenVolume = m_player->volume();
    if(volumeAdd){
        if(currenVolume < 95){
            m_player->setVolume((currenVolume+5));
        }else{
            m_player->setVolume(100);
        }
    }else{
        if(currenVolume > 5){
            m_player->setVolume(currenVolume - 5);
        }else{
            m_player->setVolume(0);
        }
    }
    m_bottomwid->updateVolumeSliderValue(m_player->volume());
}

void MusicWidgets::saveVolume(int volume)
{
    QDir settingsDir("/data/");
    QFile *volumeFile;

    if(settingsDir.exists()){
        volumeFile = new QFile("/data/volumn");
    }else{
        volumeFile = new QFile("/etc/volumn");
    }

    if (volumeFile->open(QFile::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(volumeFile);
        out << volume;
        volumeFile->close();
    }
}

MusicWidgets::~MusicWidgets()
{
}
