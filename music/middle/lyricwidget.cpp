#include "lyricwidget.h"
#include <QVBoxLayout>

LyricWidget::LyricWidget(QWidget *parent):BaseWidget(parent)
{
    initLayout();
}

void LyricWidget::initLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;

    m_lblTip=new QLabel("==-歌词界面-==",this);
    m_lblTip->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_lblTip);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

void LyricWidget::setOriginState()
{
    m_lblTip->setText("==-歌词界面-==");
}

void LyricWidget::setSongText(QString text)
{
    m_lblTip->setText("当前歌曲:" + text);
}
