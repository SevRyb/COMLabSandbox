
#ifndef SIGNALPLOTTER_H
#define SIGNALPLOTTER_H

#include <QScrollArea>
#include <QMenu>
#include <QLabel>
#include <QImage>
#include <QPainter>
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
    void visualizeMsg(const QSerialPort *serial_port, int words_delay, const QByteArray &data);

private slots:
    void onSaveImg();

private:
    void visualizePacket(QPainter &painter, QVector<Bit> packet_data, Bit prev_bit,
                         const QPointF &segment_pos, const QString &data_note = "");

    QAction *m_saveImgAction;
    QMenu *m_contextMenu;
    QLabel *m_lbl;
    QImage m_image;

    const QSerialPort *m_serialPort;
    int m_wordsDelay;
    int m_nPacketBits;

    /* Graphics setup */
    QPointF m_plotPos;
    QSizeF m_squareSize;
    int m_rightMargin;
    int m_imgHeight;
};

#endif // SIGNALPLOTTER_H
