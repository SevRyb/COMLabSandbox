
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <oclero/qlementine/widgets/FramelessWindow.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <QGridLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>

#include <QSerialPort>

#include "signal_plotter.h"

using namespace oclero;

class MainWindow : public qlementine::FramelessWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:

    void onReadyRead();
private:
    QWidget *m_rootWidget;
    QGridLayout *m_rootLay;


    QGroupBox *m_sendGroupBox;
    QGroupBox *m_receivedGroupBox;
    QGroupBox *m_setupGroupBox;
    QGroupBox *m_plotGroupBox;

    // Send area
    QGridLayout *m_sendGridLay;
    qlementine::LineEdit *m_msgToSendEdit;
    QPushButton *m_sendBtn;
    qlementine::Switch *m_asciiSwitch;

    // Received area
    QGridLayout *m_receivedGridLay;
    QPlainTextEdit *m_receivedMsgsEdit;

    // Setup area
    QGridLayout *m_setupGridLay;

    QComboBox *m_portComboBox;
    QSpinBox *m_baudRateSpinBox;
    QComboBox *m_parityComboBox;
    QComboBox *m_stopBitsComboBox;
    QComboBox *m_dataBitsComboBox;
    QSpinBox *m_wordsDelaySpinBox;
    QPushButton *m_openCloseBtn;


    // Plot area
    QGridLayout *m_plotGridLay;

    SignalPlotter *m_signalPlotter;


    QString m_baseWindowTitle;

    QSerialPort m_serialPort;


};

#endif // MAINWINDOW_H
