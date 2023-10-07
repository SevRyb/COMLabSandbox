
#ifndef COMBOBOX_H
#define COMBOBOX_H


#include <QComboBox>


class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void pressed();
};

#endif // COMBOBOX_H
