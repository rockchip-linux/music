#include "bottomwidgets.h"
#include <QHBoxLayout>

#include "global_value.h"

#ifdef DEVICE_EVB
int playButton_size = 100;
int bottom_height = 150;
int bottom_spacing = 25;
int layout3_size = 70;
int layout3_temp = 25;
int progress_slider_height = 20;
#else
int playButton_size = 50;
int bottom_height = 80;
int bottom_spacing = 20;
int layout3_size = 40;
int layout3_temp = 10;
int progress_slider_height = 10;
#endif


BottomWidgets::BottomWidgets(QWidget *parent) : BaseWidget(parent)
{
    // Set background color.
    setObjectName("BottomWidgets");
    setStyleSheet("#BottomWidgets{background:rgb(54,54,54)}");

    initLayout();
    initConnection();
}

void BottomWidgets::initLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *controlLayout=new QHBoxLayout;

    // Control layout1: position label.
    m_labPosition=new QLabel("00:00/00:00",this);
    m_labPosition->setAlignment(Qt::AlignCenter);
    QFont font = m_labPosition->font();
    font.setPixelSize(font_size_large);
    m_labPosition->setFont(font);
    /* Set the whole widget height = this label height + BaseSlider height */
    m_labPosition->setFixedHeight(bottom_height);

    // Control layout2: play control button.
    m_btnNext=new FlatButton(this);
    m_btnPrevious=new FlatButton(this);
    m_btnPlay=new FlatButton(this);

    m_btnNext->setFixedSize(playButton_size,playButton_size);
    m_btnPrevious->setFixedSize(playButton_size,playButton_size);
    m_btnPlay->setFixedSize(playButton_size,playButton_size);

    m_btnNext->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_next (1).png);}"
                             "QPushButton::hover{border-image:url(:/image/music/btn_next (2).png);}"
                             "QPushButton::pressed{border-image:url(:/image/music/btn_next (3).png);}");
    m_btnPrevious->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_previous (1).png);}"
                                 "QPushButton::hover{border-image:url(:/image/music/btn_previous (2).png);}"
                                 "QPushButton::pressed{border-image:url(:/image/music/btn_previous (3).png);}");
    m_btnPlay->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_play (1).png);}"
                             "QPushButton::hover{border-image:url(:/image/music/btn_play (2).png);}"
                             "QPushButton::pressed{border-image:url(:/image/music/btn_play (3).png);}");

    QHBoxLayout *playControlLayout = new QHBoxLayout;
    playControlLayout->addWidget(m_btnPrevious);
    playControlLayout->addWidget(m_btnPlay);
    playControlLayout->addWidget(m_btnNext);
    playControlLayout->setSpacing(bottom_spacing);
    playControlLayout->setContentsMargins(0,0,0,0);

    // control layout3: volume、playmode
    m_volWid = new VolWidget(this);
    m_btnPlayMode = new FlatButton(this);
    m_btnPlayMode->setFixedSize(layout3_size,layout3_size-layout3_temp);
    m_btnRefresh = new FlatButton(this);
    m_btnRefresh->setFixedSize(layout3_size,layout3_size);
    m_btnRefresh->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_refresh.png);}");

    QHBoxLayout *layout3 = new QHBoxLayout;
    layout3->addStretch(0);
    layout3->addWidget(m_btnPlayMode);
    layout3->addWidget(m_volWid);
    layout3->addWidget(m_btnRefresh);
    layout3->addStretch(0);
    layout3->setContentsMargins(0,0,0,0);
    layout3->setSpacing(bottom_spacing);

    controlLayout->addWidget(m_labPosition,1);
    controlLayout->addLayout(playControlLayout,1);
    controlLayout->addLayout(layout3,1);
    controlLayout->setContentsMargins(0,0,0,0);
    controlLayout->setSpacing(0);

    m_progressSlider = new BaseSlider(Qt::Horizontal,this);
    m_progressSlider->setFixedHeight(progress_slider_height);

    mainLayout->addSpacing(0);
    mainLayout->addWidget(m_progressSlider);
    mainLayout->addLayout(controlLayout);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
}

void BottomWidgets::initConnection()
{
    connect(m_btnPrevious,SIGNAL(longPressedEvent()),this,SIGNAL(lastLongPressed()));
    connect(m_btnNext,SIGNAL(longPressedEvent()),this,SIGNAL(nextLongPressed()));
    connect(m_btnNext,SIGNAL(clicked(bool)),this,SIGNAL(nextClick()));
    connect(m_btnPrevious,SIGNAL(clicked(bool)),this,SIGNAL(lastClick()));
    connect(m_btnPlay,SIGNAL(clicked(bool)),this,SIGNAL(playPauseClick()));
    connect(m_progressSlider,SIGNAL(sig_sliderPositionChanged(int)),this,SIGNAL(progressSliderPositionChanged(int)));
    connect(m_volWid,SIGNAL(sig_valueChanged(int)),this,SIGNAL(volumeChanged(int)));
    connect(m_btnPlayMode,SIGNAL(clicked(bool)),this,SIGNAL(playModeClick()));
    connect(m_btnRefresh,SIGNAL(clicked(bool)),this,SIGNAL(refreshClick()));
}

void BottomWidgets::setPauseStyle()
{
    m_btnPlay->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_pause (1).png);}"
                             "QPushButton::hover{border-image:url(:/image/music/btn_pause (2).png);}"
                             "QPushButton::pressed{border-image:url(:/image/music/btn_pause (3).png);}");
}

void BottomWidgets::setPlayStyle()
{
    m_btnPlay->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_play (1).png);}"
                             "QPushButton::hover{border-image:url(:/image/music/btn_play (2).png);}"
                             "QPushButton::pressed{border-image:url(:/image/music/btn_play (3).png);}");
}

void BottomWidgets::setPositionLabel(const QString &str)
{
    m_labPosition->setText(str);
}

void BottomWidgets::updateVolumeSliderValue(int value)
{
    m_volWid->updateSlider(value);
}

void BottomWidgets::onPlayerDurationChanged(qint64 duration)
{
    m_progressSlider->setRange(0, duration);
}

void BottomWidgets::onPlayerPositionChanged(qint64 position,qint64 duration)
{
    QTime currentTime((position % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),
                      (position % (1000 * 60 * 60)) / (1000 * 60),
                      (position % (1000 * 60)) / 1000);
    QTime totalTime((duration % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),
                    (duration % (1000 * 60 * 60)) / (1000 * 60),
                    (duration % (1000 * 60)) / 1000);
    setPositionLabel(QString(currentTime.toString("mm:ss").append("/").append(totalTime.toString("mm:ss"))));
    m_progressSlider->setRange(0, duration);
    m_progressSlider->setValue(position);
}

void BottomWidgets::updatePlayModeIcon(PlayMode playMode)
{
    switch(playMode){
    case PlayRandom:
        m_btnPlayMode->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_mode_random.png);}");
        break;
    case PlayOneCircle:
        m_btnPlayMode->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_mode_single.png);}");
        break;
    case PlayInOrder:
        m_btnPlayMode->setStyleSheet("QPushButton{border-image:url(:/image/music/btn_mode_list.png);}");
        break;
    }
}

void BottomWidgets::setOriginState()
{
    m_progressSlider->setValue(0);
    setPositionLabel("00:00/00:00");
}
