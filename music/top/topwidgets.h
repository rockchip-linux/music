#ifndef TOPWIDGETS_H
#define TOPWIDGETS_H
#include <base/basewidget.h>
#include "base/basepushbutton.h"

class TopWidgets:public BaseWidget
{
    Q_OBJECT
public:
    TopWidgets(QWidget *parent = 0);
    ~TopWidgets();
private:
    FourStateButton *m_btnreturn;
private:
    void initLayout();
    void initConnection();
signals:
    void returnClick();
};

#endif // TOPWIDGETS_H
