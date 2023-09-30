
#include "signal_plotter.h"

#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include <QGraphicsItem>


SignalPlotter::SignalPlotter(QWidget *parent)
    : QScrollArea(parent)
{

    m_lbl = new QLabel(this);

    QBrush greenBrush(Qt::green);
    QBrush redBrush(Qt::red);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    m_image = QImage(2000, 300, QImage::Format_RGB888);
    m_image.fill(QColor(Qt::white));

    QPainter painter(&m_image);
    painter.fillRect(20, 20, 1000, 100, greenBrush);
    painter.end();


    m_lbl->setPixmap(QPixmap::fromImage(m_image));
    setWidget(m_lbl);


}

SignalPlotter::~SignalPlotter() {}

void SignalPlotter::paintEvent(QPaintEvent *event)
{
    QScrollArea::paintEvent(event);

}
