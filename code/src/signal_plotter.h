
#ifndef SIGNALPLOTTER_H
#define SIGNALPLOTTER_H

#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QByteArray>
#include <QSerialPort>
#include <QVector>

#include "utils.h"

class SignalPlotter : public QScrollArea
{
    Q_OBJECT
public:
    SignalPlotter(QWidget *parent = nullptr);
    ~SignalPlotter();
//    virtual void paintEvent(QPaintEvent *event);
    void visualizeMsg(const QSerialPort *serial_port, int words_delay, const QByteArray &data);

private:
    void visualizePacket(QPainter &painter, QVector<Bit> packet_data, Bit prev_bit,
                         const QPointF &segment_pos, const QString &data_note = "");

    QLabel *m_lbl;
    QImage m_image;

    const QSerialPort *m_serialPort;
    int m_wordsDelay;

    int m_nPacketBits;

    /* Graphics setup */
    QPointF m_plotPos;
    QSizeF m_squareSize;
    /* Brushes */


    /* Pens */

};

#endif // SIGNALPLOTTER_H
