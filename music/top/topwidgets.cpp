#include "topwidgets.h"
#include <QHBoxLayout>
#include <QLabel>
#include "global_value.h"

TopWidgets::TopWidgets(QWidget *parent):BaseWidget(parent)
{
    initLayout();
    initConnection();
}

void TopWidgets::initLayout()
{
    QHBoxLayout *hmainyout=new QHBoxLayout;

    m_btnreturn=new FourStateButton(return_resource_str,this);
    m_btnreturn->setFixedSize(return_icon_width,return_icon_height);

    QHBoxLayout *lyout1 = new QHBoxLayout;
    lyout1->addWidget(m_btnreturn);
    lyout1->addStretch(0);
    lyout1->setContentsMargins(0,0,0,0);

    QLabel *titleLabel=new QLabel("musicPlayer",this);
    QFont font = titleLabel->font();
    font.setPixelSize(font_size_big);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);

    hmainyout->addLayout(lyout1,1);
    hmainyout->addWidget(titleLabel,1);
    hmainyout->addStretch(1);
    hmainyout->setContentsMargins(0,0,0,0);
    hmainyout->setSpacing(0);
    setLayout(hmainyout);
}

void TopWidgets::initConnection()
{
    connect(m_btnreturn,SIGNAL(clicked(bool)),this,SIGNAL(returnClick()));
}

TopWidgets::~TopWidgets()
{
}
