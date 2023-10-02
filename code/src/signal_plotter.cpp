
#include "signal_plotter.h"

#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include <QFontMetrics>
#include <QDebug>


SignalPlotter::SignalPlotter(QWidget *parent)
    : QScrollArea(parent)
{

    m_lbl = new QLabel(this);



    setWidget(m_lbl);

}

SignalPlotter::~SignalPlotter() {}

//void SignalPlotter::paintEvent(QPaintEvent *event)
//{
//    QScrollArea::paintEvent(event);

//}

void SignalPlotter::visualizeMsg(const QSerialPort &serial_port, const QByteArray &data)
{

    QString packet_bin = "110101101100011010101";

    m_image = QImage(2000, 300, QImage::Format_RGB888);
    m_image.fill(QColor(Qt::white));


    //QBrush redBrush(Qt::red);




    /* Drawing signal */

    // Signal plot setup

    qreal tact_width = 60;
    qreal amplitude_height = 100;
    qreal signal_hoffset = 50;
    qreal signal_voffset = 50;

    qreal low_y = signal_voffset + amplitude_height;
    qreal high_y = signal_voffset;

    QPen signal_pen(QColor(2, 156, 221));
    signal_pen.setWidth(4);
    signal_pen.setJoinStyle(Qt::MiterJoin);
    QPainterPath signal_path;


    QPainter painter(&m_image);
    //painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(signal_pen);
    //painter.fillRect(20, 20, 1000, 100, greenBrush);
    //painter.drawPath(signal_path);

    // First bit
    QPoint point(signal_hoffset, high_y);  // Left high corner of signal
    if (packet_bin.at(0) == QChar('0'))
    {
        point.ry() = low_y;
        signal_path.moveTo(point);
        point.rx() += tact_width;
        signal_path.lineTo(point);
    }
    else if (packet_bin.at(0) == QChar('1'))
    {
        signal_path.moveTo(point);
        point.rx() += tact_width;
        signal_path.lineTo(point);
    }
    // Next bits
    for (int index = 1; index < packet_bin.length(); ++index)
    {
        QChar prev_bit = packet_bin.at(index-1);
        QChar bit = packet_bin.at(index);
        if (prev_bit == QChar('0') && bit == QChar('0'))
        {
            point.rx() += tact_width;
            signal_path.lineTo(point);
        }
        else if (prev_bit == QChar('1') && bit == QChar('1'))
        {
            point.rx() += tact_width;
            signal_path.lineTo(point);
        }
        else if (prev_bit == QChar('0') && bit == QChar('1'))
        {
            point.ry() = high_y;
            signal_path.lineTo(point);
            point.rx() += tact_width;
            signal_path.lineTo(point);
        }
        else if (prev_bit == QChar('1') && bit == QChar('0'))
        {
            point.ry() = low_y;
            signal_path.lineTo(point);
            point.rx() += tact_width;
            signal_path.lineTo(point);
        }
    }


    QRectF rect;
    /* Draw idle */
    QBrush idle_brush(QColor(199, 199, 199));
    rect = QRectF(signal_hoffset, signal_voffset, 2 * tact_width, amplitude_height);  // Always add two "1"
    painter.fillRect(rect, idle_brush);

    /* Draw start bit */
    QBrush pattern_brush(signal_pen.color(), Qt::Dense5Pattern);
    rect = QRectF(rect.right(), signal_voffset, tact_width, amplitude_height);  // Always one bit
    painter.fillRect(rect, pattern_brush);

    /* Draw stop bit */
    rect = QRectF(signal_hoffset + (3 + static_cast<qint8>(serial_port.dataBits())) * tact_width,
                  signal_voffset,
                  tact_width * static_cast<qint8>(serial_port.stopBits()),
                  amplitude_height);  // Always one bit
    painter.fillRect(rect, pattern_brush);

    /* Draw parity bit */
    QBrush parity_brush(QColor(230, 216, 194), Qt::Dense5Pattern);
    painter.fillRect(signal_hoffset + 12 * tact_width, signal_voffset, tact_width, amplitude_height, parity_brush);

    painter.drawPath(signal_path);


    /* Drawing bit signs */
    QFont font;
    font.setFamily("Consolas");
    font.setPixelSize(40);
    painter.setFont(font);
    QPen text_pen(Qt::black);
    painter.setPen(text_pen);

    QFontMetrics font_metrics(font);

    for (int index = 0; index < packet_bin.length(); ++index)
    {
        QChar bit = packet_bin.at(index);
        QRect text_rect = font_metrics.boundingRect(bit);
        QPoint text_pos(signal_hoffset + index * tact_width + tact_width / 2 - text_rect.width() / 2,
                        signal_voffset + amplitude_height / 2  + text_rect.height() / 2);
        painter.drawText(text_pos, bit);
    }


    painter.end();


    m_lbl->setPixmap(QPixmap::fromImage(m_image));
    m_lbl->resize(m_image.size());

    qDebug() << m_lbl->geometry();


}
