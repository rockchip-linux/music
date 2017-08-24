#ifndef MIDDLEWIDGETRIGHT_H
#define MIDDLEWIDGETRIGHT_H

#include "basewidget.h"
#include "QLabel"

class LyricWidget:public BaseWidget
{
public:
    LyricWidget(QWidget *parent =0);
    ~LyricWidget(){}

    void setOriginState();
    void setSongText(QString text);
private:
    QLabel *m_lblTip;

    void initLayout();
};

#endif // MIDDLEWIDGETRIGHT_H
