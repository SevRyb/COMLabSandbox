
#ifndef SIGNALPLOTTER_H
#define SIGNALPLOTTER_H

#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QPainter>



class SignalPlotter : public QScrollArea
{
    Q_OBJECT
public:
    SignalPlotter(QWidget *parent = nullptr);
    ~SignalPlotter();
    virtual void paintEvent(QPaintEvent *event);
    void visualizeMsg();
private:
    QLabel *m_lbl;
    QImage m_image;

};

#endif // SIGNALPLOTTER_H
