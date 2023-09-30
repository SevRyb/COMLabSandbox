
#ifndef SIGNALPLOTTER_H
#define SIGNALPLOTTER_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>



class SignalPlotter : public QScrollArea
{
    Q_OBJECT
public:
    SignalPlotter(QWidget *parent = nullptr);
    ~SignalPlotter();
private:
    QLabel *m_label;
};

#endif // SIGNALPLOTTER_H
