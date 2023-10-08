
#include "signal_plotter.h"

#include <QPixmap>
#include <QPainterPath>
#include <QFontMetrics>
#include <QShortcut>
#include <QFileDialog>

SignalPlotter::SignalPlotter(QWidget *parent)
    : QScrollArea(parent)
{
    m_nPacketBits = 0;

    m_plotPos = QPointF(50, 35);
    m_squareSize = QSizeF(60, 100);
    m_rightMargin = 50;
    m_imgHeight = 220;

    m_lbl = new QLabel(this);
    m_lbl->resize(0, 0);
    m_lbl->setContextMenuPolicy(Qt::CustomContextMenu);

    QKeySequence keySequence = QKeySequence(Qt::CTRL + Qt::Key_S);
    QShortcut *schortcut = new QShortcut(keySequence, this);
    m_saveImgAction = new QAction("Save as...", m_lbl);
    m_saveImgAction->setShortcut(keySequence);
    m_contextMenu = new QMenu(m_lbl);
    m_contextMenu->addAction(m_saveImgAction);

    setWidget(m_lbl);

    connect(m_lbl, &QLabel::customContextMenuRequested, this, [this](const QPoint &pos)
            {
                m_contextMenu->popup(m_lbl->mapToGlobal(pos));
            });
    connect(m_saveImgAction, &QAction::triggered, this, &SignalPlotter::onSaveImg);
    connect(schortcut, &QShortcut::activated, this, &SignalPlotter::onSaveImg);
}

SignalPlotter::~SignalPlotter() {}

void SignalPlotter::visualizeMsg(const QSerialPort *serial_port, int words_delay, const QByteArray &data)
{
    if (data.isEmpty())
    {
        m_image = QImage();
        m_lbl->setPixmap(QPixmap::fromImage(m_image));
        m_lbl->adjustSize();
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

    QSize imgSize = QSize(m_plotPos.x() + (m_nPacketBits * m_squareSize.width() * data.length()) + m_rightMargin, m_imgHeight);
    m_image = QImage(imgSize, QImage::Format_RGB16);
    m_image.fill(QColor(Qt::white));

    QString hexData = data.toHex();
    Bit prevBit('1');
    QPainter painter(&m_image);
    for (int index = 0; index < data.length(); ++index)
    {
        quint8 byte = data.at(index);
        QVector<Bit> packetBits;

        // Start bit
        packetBits.append(Bit('0', BIT_TYPE::START_BIT));

        // Data bits
        QString payloadBin = QString("%1").arg(QString::number(byte, 2), 8, '0');
        for (QChar bitChar : payloadBin)
            packetBits.append(Bit(bitChar, BIT_TYPE::DATA_BIT));

        // Parity bit
        QSerialPort::Parity parity = m_serialPort->parity();
        if (parity != QSerialPort::NoParity)
        {
            qint8 nHighBits = payloadBin.count('1');
            if (parity == QSerialPort::EvenParity)
                packetBits.append(Bit(QString::number(nHighBits & 1).at(0), BIT_TYPE::PARITY_BIT));
            else if (parity == QSerialPort::OddParity)
                packetBits.append(Bit(QString::number(!(nHighBits & 1)).at(0), BIT_TYPE::PARITY_BIT));
        }

        // Stop bits
        packetBits.append(Bit('1', BIT_TYPE::STOP_BIT));  // Always must be at least 1 bit
        if (m_serialPort->stopBits() == QSerialPort::TwoStop)
            packetBits.append(Bit('1', BIT_TYPE::STOP_BIT));

        // Idle tacts
        for (int i = 0; i < m_wordsDelay; ++i)
            packetBits.append(Bit('1', BIT_TYPE::IDLE_BIT));

        QPointF segmentPos;
        segmentPos.rx() = m_plotPos.x() + index * (m_nPacketBits * m_squareSize.width());
        segmentPos.ry() = m_plotPos.y();
        visualizePacket(painter, packetBits, prevBit, segmentPos, "0x" + hexData.mid(index * 2, 2).toUpper());
        prevBit = packetBits.back();
    }
    painter.end();

    m_lbl->setPixmap(QPixmap::fromImage(m_image));
    m_lbl->adjustSize();
}

void SignalPlotter::onSaveImg()
{
    QString defaultFileName = QDir::currentPath() + "/plot.png";
    QString fileName = QFileDialog::getSaveFileName(this, "Save Plot",
                                                    defaultFileName,
                                                    "Images (*.png *.jpg)");
    m_image.save(fileName);
}

void SignalPlotter::visualizePacket(QPainter &painter, QVector<Bit> packetBits, Bit prevBit,
                                    const QPointF &segmentPos, const QString &data_note)
{
    int nIdleBits = 0;
    qreal lowLevelY = segmentPos.y() + m_squareSize.height();
    qreal highLevelY = segmentPos.y();

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

    QRect textRect;
    QPoint textPos;

    /* Rects */
    QRect dataBitsRect;
    QRect stopBitsRect;
    QRect idleBitsRect;

    auto _updateRect = [&](QRect &rect, int index)
    {
        if (rect.isEmpty())
        {
            rect.setX(segmentPos.x() + (index * m_squareSize.width()));
            rect.setY(segmentPos.y());
            rect.setHeight(m_squareSize.height());
            rect.setWidth(0);
        }
        rect.setWidth(rect.width() + m_squareSize.width());
    };


    QPoint point(segmentPos.x(), highLevelY);  // Left high corner of signal
    for (int bitIndex = 0; bitIndex < packetBits.length(); ++bitIndex)
    {
        Bit bit = packetBits.at(bitIndex);
        // First bit
        if (bitIndex == 0)
        {
            if (prevBit.value() == QChar('0'))
            {
                point.ry() = lowLevelY;
                signalPath.moveTo(point);
            }
            else if (prevBit.value() == QChar('1'))
                signalPath.moveTo(point);
        }
        else
            prevBit = packetBits.at(bitIndex - 1);
        // Next bits
        if (prevBit.value() == QChar('0') && bit.value() == QChar('0'))
        {
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prevBit.value() == QChar('1') && bit.value() == QChar('1'))
        {
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prevBit.value() == QChar('0') && bit.value() == QChar('1'))
        {
            point.ry() = highLevelY;
            signalPath.lineTo(point);
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }
        else if (prevBit.value() == QChar('1') && bit.value() == QChar('0'))
        {
            point.ry() = lowLevelY;
            signalPath.lineTo(point);
            point.rx() += m_squareSize.width();
            signalPath.lineTo(point);
        }

        painter.setPen(notesPen);
        painter.setFont(normalFont);

        QRectF rect;
        if (bit.type() == BIT_TYPE::START_BIT)
        {
            /* Draw start bit */
            rect = QRectF(segmentPos.x() + (bitIndex * m_squareSize.width()), segmentPos.y(),
                          m_squareSize.width(), m_squareSize.height());  // Always one bit
            painter.fillRect(rect, startStopBitsBrush);
            /* Draw note */
            const char text[] = "start";
            textRect = normalFontMetrics.boundingRect(text);
            textPos = QPoint(segmentPos.x() + (bitIndex * m_squareSize.width()) - textRect.width() / 2 + m_squareSize.width() / 2,
                            segmentPos.y() - textRect.height() / 2);
            painter.drawText(textPos, text);
        }
        if (bit.type() == BIT_TYPE::DATA_BIT)
            _updateRect(dataBitsRect, bitIndex);
        if (bit.type() == BIT_TYPE::PARITY_BIT)
        {
            rect = QRectF(segmentPos.x() + (bitIndex * m_squareSize.width()), segmentPos.y(),
                          m_squareSize.width(), m_squareSize.height());
            painter.fillRect(rect, parityBitBrush);
            /* Draw note */
            const char text[] = "parity";
            textRect = normalFontMetrics.boundingRect(text);
            textPos = QPoint(segmentPos.x() + (bitIndex * m_squareSize.width()) - textRect.width() / 2 + m_squareSize.width() / 2,
                            segmentPos.y() - textRect.height() / 2);
            painter.drawText(textPos, text);
        }
        if (bit.type() == BIT_TYPE::STOP_BIT)
            _updateRect(stopBitsRect, bitIndex);
        if (bit.type() == BIT_TYPE::IDLE_BIT)
        {
            nIdleBits++;
            _updateRect(idleBitsRect, bitIndex);
        }
    }

    painter.setFont(normalFont);
    /* Stop bits highlight & text */
    if (!stopBitsRect.isEmpty())
    {
        painter.fillRect(stopBitsRect, startStopBitsBrush);
        /* Draw note */
        const char text[] = "stop";
        textRect = normalFontMetrics.boundingRect(text);
        textPos = QPoint(stopBitsRect.center().x() - textRect.width() / 2,
                        stopBitsRect.y() - textRect.height() / 2);
        painter.drawText(textPos, text);
    }
    /* Idle bits highlight & text */
    if (!idleBitsRect.isEmpty())
    {
        painter.fillRect(idleBitsRect, idleBitsBrush);
        /* Draw note */
        const char text[] = "idle";
        textRect = normalFontMetrics.boundingRect(text);
        textPos = QPoint(idleBitsRect.center().x() - textRect.width() / 2,
                        idleBitsRect.y() - textRect.height() / 2);
        painter.drawText(textPos, text);
    }

    /* Drawing bit signs */
    painter.setFont(bitFont);
    for (int index = 0; index < packetBits.length(); ++index)
    {
        Bit bit = packetBits.at(index);
        painter.setFont(bitFont);
        textRect = bitFontMetrics.boundingRect(bit.value());
        textPos = QPoint(segmentPos.x() + index * m_squareSize.width() + m_squareSize.width() / 2 - textRect.width() / 2,
                        segmentPos.y() + m_squareSize.height() / 2  + textRect.height() / 2);
        painter.drawText(textPos, bit.value());
    }

    /* Draw signal */
    painter.setPen(signalPen);
    painter.drawPath(signalPath);

    /* Data bits highlight & text */
    if (!dataBitsRect.isEmpty())
    {
        painter.setPen(dashLinePen);
        int lineLen = dataBitsRect.height() + 55;
        int lineVOffset = lineLen / 2 - dataBitsRect.height() / 2;
        int lineHOffset = 3;
        painter.drawLine(dataBitsRect.x() + lineHOffset + 1, dataBitsRect.y() - lineVOffset,
                         dataBitsRect.x() + lineHOffset + 1, dataBitsRect.bottomLeft().y() + lineVOffset);
        painter.drawLine(dataBitsRect.topRight().x() - lineHOffset, dataBitsRect.y() - lineVOffset,
                         dataBitsRect.topRight().x() - lineHOffset, dataBitsRect.bottomLeft().y() + lineVOffset);
        /* Draw note */
        painter.setPen(notesPen);
        const char text[] = "data";
        textRect = boldFontMetrics.boundingRect(text);
        textPos = QPoint(dataBitsRect.center().x() - textRect.width() / 2,
                        dataBitsRect.y() - textRect.height() / 2);
        painter.setFont(boldFont);
        painter.drawText(textPos, text);
        /* Hex value */
        textRect = boldFontMetrics.boundingRect(data_note);
        textPos = QPoint(dataBitsRect.center().x() - textRect.width() / 2,
                 dataBitsRect.bottom() + textRect.height() + 10);
        painter.drawText(textPos, data_note);
    }

    /* Draw packet sign */
    painter.setPen(QPen(Qt::black, 1));
    int lineLen = 40;
    int lineY = segmentPos.y() + m_squareSize.height() + 10;
    int line2X = segmentPos.x() + (packetBits.length() - nIdleBits) * m_squareSize.width();
    painter.drawLine(segmentPos.x(), lineY, segmentPos.x(), lineY + lineLen);
    painter.drawLine(line2X, lineY, line2X, lineY + lineLen);
    painter.drawLine(segmentPos.x(), lineY + lineLen, line2X, lineY + lineLen);
    /* Text */
    const char text[] = "packet";
    textRect = normalFontMetrics.boundingRect(text);
    textPos = QPoint(segmentPos.x() + ((packetBits.length() - nIdleBits) * m_squareSize.width()) / 2 - textRect.width() / 2,
                      lineY + lineLen + textRect.height());
    painter.setFont(normalFont);
    painter.drawText(textPos, text);
}
