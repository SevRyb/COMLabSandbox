
#include "signal_plotter.h"

#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include <QFontMetrics>
#include <QStaticText>
#include <QDebug>


SignalPlotter::SignalPlotter(QWidget *parent)
    : QScrollArea(parent)
{
    m_lbl = new QLabel(this);
    m_lbl->resize(0, 0);

    m_nPacketBits = 0;

    m_plotPos = QPointF(50, 60);
    m_squareSize = QSizeF(60, 100);

    setWidget(m_lbl);
}

SignalPlotter::~SignalPlotter() {}

//void SignalPlotter::paintEvent(QPaintEvent *event)
//{
//    QScrollArea::paintEvent(event);
//    QPainter painter(this);
//    painter.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::red));
//}

void SignalPlotter::visualizeMsg(const QSerialPort *serial_port, int words_delay, const QByteArray &data)
{
    if (data.isEmpty())
    {
        m_image = QImage();
        m_lbl->resize(m_image.size());
        m_lbl->setPixmap(QPixmap::fromImage(m_image));
        return;
    }

    m_serialPort = serial_port;
    m_wordsDelay = words_delay;

    m_nPacketBits = 1 + 8 + m_wordsDelay;  // Start bit + 8 data bits + idle delay tacts
    if (m_serialPort->parity() != QSerialPort::NoParity)
        m_nPacketBits++;

    if (m_serialPort->stopBits() == QSerialPort::OneStop)
        m_nPacketBits++;
    else if (m_serialPort->stopBits() == QSerialPort::TwoStop)
        m_nPacketBits += 2;


    int right_margin = 50;
    QSize img_size = QSize(m_plotPos.x() + (m_nPacketBits * m_squareSize.width() * data.length()) + right_margin,
                           300);
    m_image = QImage(img_size, QImage::Format_RGB888);
    m_image.fill(QColor(Qt::white));

    QPainter painter(&m_image);

    QString hex_data = data.toHex();
    qDebug() << "Message: " << data;
    qDebug() << "Data: " << hex_data;

    Bit prev_bit('1');
    for (int index = 0; index < data.length(); ++index)
    {
        quint8 byte = data.at(index);
        QVector<Bit> packet_data;

        // Start bit
        packet_data.append(Bit('0', BIT_TYPE::START_BIT));

        // Data bits
        QString payload_bin = QString("%1").arg(QString::number(byte, 2), 8, '0');
        for (QChar bin_value : payload_bin)
        {
            packet_data.append(Bit(bin_value, BIT_TYPE::DATA_BIT));
        }

        // Parity bit
        QSerialPort::Parity parity = m_serialPort->parity();
        if (parity != QSerialPort::NoParity)
        {
            qint8 high_bits = payload_bin.count('1');
            if (parity == QSerialPort::EvenParity)
                packet_data.append(Bit(QString::number(high_bits & 1).at(0), BIT_TYPE::PARITY_BIT));
            else if (parity == QSerialPort::OddParity)
                packet_data.append(Bit(QString::number(!(high_bits & 1)).at(0), BIT_TYPE::PARITY_BIT));
        }

        // Stop bits
        packet_data.append(Bit('1', BIT_TYPE::STOP_BIT));  // Always must be at least 1 bit
        if (m_serialPort->stopBits() == QSerialPort::TwoStop)
            packet_data.append(Bit('1', BIT_TYPE::STOP_BIT));

        // Idle tacts
        for (int i = 0; i < m_wordsDelay; ++i)
        {
            packet_data.append(Bit('1', BIT_TYPE::IDLE_BIT));
        }

        QPointF segment_pos;
        segment_pos.rx() = m_plotPos.x() + index * (m_nPacketBits * m_squareSize.width());
        segment_pos.ry() = m_plotPos.y();
        visualizePacket(painter, packet_data, prev_bit, segment_pos, "0x" + hex_data.mid(index * 2, 2).toUpper());
        //qDebug() << "Segment pos: " << segment_pos;
        prev_bit = packet_data.back();
    }



    painter.end();


    m_lbl->resize(m_image.size());
    m_lbl->setPixmap(QPixmap::fromImage(m_image));


    //qDebug() << "Packet bits: " << m_nPacketBits;


}

void SignalPlotter::visualizePacket(QPainter &painter, QVector<Bit> packet_data, Bit prev_bit,
                                    const QPointF &segment_pos, const QString &data_note)
{

    int nIdleBits = 0;
    qreal lowLevelY = segment_pos.y() + m_squareSize.height();
    qreal highLevelY = segment_pos.y();

    /* Path */
    QPainterPath signalPath;

    /* Pens */
    QPen signalPen(QColor(2, 156, 221));
    signalPen.setWidth(4);
    signalPen.setJoinStyle(Qt::MiterJoin);
    QPen notesPen(Qt::black);
    QPen dashLinePen(QColor(134, 134, 134), 2, Qt::DashLine);
    dashLinePen.setDashPattern(QVector<qreal>{3, 4, 3, 4});

    /* Brushes */
    QBrush parityBitBrush(QColor(255, 0, 0), Qt::Dense5Pattern);
    QBrush startStopBitsBrush(signalPen.color(), Qt::Dense5Pattern);
    QBrush idleBitsBrush(QColor(199, 199, 199));

    /* Fonts */
    QFont bitFont;
    bitFont.setPixelSize(40);
    QFontMetrics bitFontMetrics(bitFont);
    QFont normalFont;
    normalFont.setPixelSize(20);
    QFontMetrics normalFontMetrics(normalFont);
    QFont boldFont;
    boldFont.setPixelSize(20);
    boldFont.setBold(true);
    QFontMetrics boldFontMetrics(boldFont);

    /* Rects */
    QRect dataBitsRect;
    QRect stopBitsRect;
    QRect idleBitsRect;

    auto _updateRect = [&](QRect &rect, int index)
    {
        if (rect.isEmpty())
        {
            rect.setX(segment_pos.x() + (index * m_squareSize.width()));
            rect.setY(segment_pos.y());
            rect.setHeight(m_squareSize.height());
            rect.setWidth(0);
        }
        rect.setWidth(rect.width() + m_squareSize.width());
    };


    QPoint point(segment_pos.x(), highLevelY);  // Left high corner of signal
    for (int bit_index = 0; bit_index < packet_data.length(); ++bit_index)
    {
        Bit bit = packet_data.at(bit_index);
        // First bit
        if (bit_index == 0)
        {
            if (prev_bit.value() == QChar('0'))
            {
                point.ry() = lowLevelY;
                signalPath.moveTo(point);
            }
            else if (prev_bit.value() == QChar('1'))
                signalPath.moveTo(point);
        }
        else
            prev_bit = packet_data.at(bit_index-1);
        // Next bits
        if (prev_bit.value() == QChar('0') && bit.value() == QChar('0'))
        {
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prev_bit.value() == QChar('1') && bit.value() == QChar('1'))
        {
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prev_bit.value() == QChar('0') && bit.value() == QChar('1'))
        {
            point.ry() = highLevelY;
            signalPath.lineTo(point);
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prev_bit.value() == QChar('1') && bit.value() == QChar('0'))
        {
            point.ry() = lowLevelY;
            signalPath.lineTo(point);
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }

        painter.setPen(notesPen);
        painter.setFont(normalFont);
        QRect text_rect;
        QRectF rect;
        if (bit.type() == BIT_TYPE::START_BIT)
        {
            /* Draw start bit */
            rect = QRectF(segment_pos.x() + (bit_index * m_squareSize.width()), segment_pos.y(),
                          m_squareSize.width(), m_squareSize.height());  // Always one bit
            painter.fillRect(rect, startStopBitsBrush);
            /* Draw note */
            const char text[] = "start";
            text_rect = normalFontMetrics.boundingRect(text);
            QPoint text_pos(segment_pos.x() + (bit_index * m_squareSize.width()) - text_rect.width() / 2 + m_squareSize.width() / 2,
                            segment_pos.y() - text_rect.height() / 2);
            painter.drawText(text_pos, text);
        }
        if (bit.type() == BIT_TYPE::DATA_BIT)
        {
            _updateRect(dataBitsRect, bit_index);
        }
        if (bit.type() == BIT_TYPE::PARITY_BIT)
        {
            rect = QRectF(segment_pos.x() + (bit_index * m_squareSize.width()), segment_pos.y(),
                          m_squareSize.width(), m_squareSize.height());
            painter.fillRect(rect, parityBitBrush);
            /* Draw note */
            const char text[] = "parity";
            text_rect = normalFontMetrics.boundingRect(text);
            QPoint text_pos(segment_pos.x() + (bit_index * m_squareSize.width()) - text_rect.width() / 2 + m_squareSize.width() / 2,
                            segment_pos.y() - text_rect.height() / 2);
            painter.drawText(text_pos, text);
        }
        if (bit.type() == BIT_TYPE::STOP_BIT)
        {
            _updateRect(stopBitsRect, bit_index);
        }
        if (bit.type() == BIT_TYPE::IDLE_BIT)
        {
            nIdleBits++;
            _updateRect(idleBitsRect, bit_index);
        }
    }

    painter.setFont(normalFont);
    QRect text_rect;
    /* Stop bits highlight & text */
    if (!stopBitsRect.isEmpty())
    {
        painter.fillRect(stopBitsRect, startStopBitsBrush);
        /* Draw note */
        const char text[] = "stop";
        text_rect = normalFontMetrics.boundingRect(text);
        QPoint text_pos(stopBitsRect.center().x() - text_rect.width() / 2,
                        stopBitsRect.y() - text_rect.height() / 2);
        painter.drawText(text_pos, text);
    }
    /* Idle bits highlight & text */
    if (!idleBitsRect.isEmpty())
    {
        painter.fillRect(idleBitsRect, idleBitsBrush);
        /* Draw note */
        const char text[] = "idle";
        text_rect = normalFontMetrics.boundingRect(text);
        QPoint text_pos(idleBitsRect.center().x() - text_rect.width() / 2,
                        idleBitsRect.y() - text_rect.height() / 2);
        painter.drawText(text_pos, text);
    }

    /* Drawing bit signs */
    painter.setFont(bitFont);
    for (int index = 0; index < packet_data.length(); ++index)
    {
        Bit bit = packet_data.at(index);
        painter.setFont(bitFont);
        text_rect = bitFontMetrics.boundingRect(bit.value());
        QPoint text_pos(segment_pos.x() + index * m_squareSize.width() + m_squareSize.width() / 2 - text_rect.width() / 2,
                        segment_pos.y() + m_squareSize.height() / 2  + text_rect.height() / 2);
        painter.drawText(text_pos, bit.value());
    }

    /* Draw signal */
    painter.setPen(signalPen);
    painter.drawPath(signalPath);

    /* Data bits highlight & text */
    if (!dataBitsRect.isEmpty())
    {
        painter.setPen(dashLinePen);
        int line_len = dataBitsRect.height() + 55;
        int line_voffset = line_len / 2 - dataBitsRect.height() / 2;
        int line_hoffset = 3;
        painter.drawLine(dataBitsRect.x() + line_hoffset + 1, dataBitsRect.y() - line_voffset,
                         dataBitsRect.x() + line_hoffset + 1, dataBitsRect.bottomLeft().y() + line_voffset);
        painter.drawLine(dataBitsRect.topRight().x() - line_hoffset, dataBitsRect.y() - line_voffset,
                         dataBitsRect.topRight().x() - line_hoffset, dataBitsRect.bottomLeft().y() + line_voffset);
        /* Draw note */
        painter.setPen(notesPen);
        const char text[] = "data";
        text_rect = boldFontMetrics.boundingRect(text);
        QPoint text_pos(dataBitsRect.center().x() - text_rect.width() / 2,
                        dataBitsRect.y() - text_rect.height() / 2);
        painter.setFont(boldFont);
        painter.drawText(text_pos, text);
        /* Hex value */
        text_rect = boldFontMetrics.boundingRect(data_note);
        text_pos = QPoint(dataBitsRect.center().x() - text_rect.width() / 2,
                 dataBitsRect.bottom() + text_rect.height() + 10);
        painter.drawText(text_pos, data_note);
    }

    /* Draw packet sign */
    painter.setPen(QPen(Qt::black, 1));
    int line_len = 40;
    int line_y = segment_pos.y() + m_squareSize.height() + 10;
    int line2_x = segment_pos.x() + (packet_data.length() - nIdleBits) * m_squareSize.width();
    painter.drawLine(segment_pos.x(), line_y, segment_pos.x(), line_y + line_len);
    painter.drawLine(line2_x, line_y, line2_x, line_y + line_len);
    painter.drawLine(segment_pos.x(), line_y + line_len, line2_x, line_y + line_len);
    /* Text */
    const char text[] = "packet";
    text_rect = normalFontMetrics.boundingRect(text);
    QPoint text_pos = QPoint(segment_pos.x() + ((packet_data.length() - nIdleBits) * m_squareSize.width()) / 2 - text_rect.width() / 2,
                      line_y + line_len + text_rect.height());
    painter.setFont(normalFont);
    painter.drawText(text_pos, text);
}
