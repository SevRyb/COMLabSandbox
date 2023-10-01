
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <oclero/qlementine/widgets/FramelessWindow.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>
#include <QGridLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>

#include <QList>

#include <QSerialPortInfo>
#include <QSerialPort>

#include "signal_plotter.h"
#include "combo_box.h"

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
    void onRefreshPorts();
    void onOpenClosePort();
    void onSendMsg();
    //void onReceivedMsg();
private:
    void setupDefaults();
    void enableSetupFields(bool enabled);

    QWidget *m_rootWidget;
    QGridLayout *m_rootLay;


    QGroupBox *m_sendGroupBox;
    QGroupBox *m_receivedGroupBox;
    QGroupBox *m_setupGroupBox;
    QGroupBox *m_plotGroupBox;

    // Send area
    QGridLayout *m_sendGridLay;
    //qlementine::LineEdit *m_msgToSendEdit;
    QPlainTextEdit *m_msgToSendEdit;
    QPushButton *m_sendBtn;
    //qlementine::Switch *m_asciiSwitch;
    QComboBox *m_encodingComboBox;

    // Received area
    QGridLayout *m_receivedGridLay;
    QPlainTextEdit *m_receivedMsgsEdit;

    // Setup area
    QGridLayout *m_setupGridLay;

    ComboBox *m_portComboBox;
    QSpinBox *m_baudRateSpinBox;
    QComboBox *m_parityComboBox;
    QSpinBox *m_stopBitsSpinBox;
    QSpinBox *m_dataBitsSpinBox;
    QSpinBox *m_wordsDelaySpinBox;
    QPushButton *m_openCloseBtn;
    qlementine::StatusBadgeWidget *m_portOpenStatusWidget;


    // Plot area
    QGridLayout *m_plotGridLay;

    SignalPlotter *m_signalPlotter;


    QString m_baseWindowTitle;

    bool m_isPortOpened;
    QSerialPort m_serialPort;




};

#endif // MAINWINDOW_H
