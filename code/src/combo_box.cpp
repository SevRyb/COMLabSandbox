
#include "combo_box.h"

ComboBox::ComboBox(QWidget *parent)
    : QComboBox{parent}
{

}

void ComboBox::mousePressEvent(QMouseEvent *event)
{
    QComboBox::mousePressEvent(event);
    emit pressed();
}
