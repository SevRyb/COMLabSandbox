
#include "signal_plotter.h"

#include <QImage>
#include <QPixmap>


SignalPlotter::SignalPlotter(QWidget *parent)
    : QScrollArea(parent)
{

    m_label = new QLabel(this);



    setWidget(m_label);
    //setWidgetResizable(true);

}

SignalPlotter::~SignalPlotter() {}
