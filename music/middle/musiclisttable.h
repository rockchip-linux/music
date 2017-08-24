#ifndef MUISCLISTTABLE_H
#define MUISCLISTTABLE_H

#include <base/basetablewidget.h>

class MusicListTable:public BaseTableWidget
{
    Q_OBJECT
public:
    MusicListTable(QWidget *parent=0);
    ~MusicListTable(){}

    void setRowTextColor(int row, const QColor &color)const;
    void insertIntoTable(QString item1Text,QString item2Text);
    void clearTable();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    void init();
    void initConnection();
};

#endif // MUISCLISTTABLE_H
