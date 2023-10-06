
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <oclero/qlementine/widgets/FramelessWindow.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>
#include <oclero/qlementine/widgets/SegmentedControl.hpp>
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

#include <functional>

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
    void onRefreshPorts();
    void onOpenClosePort();
    void onReadyRead();
    void onSend();
    void onSendOne();
    //void onReceivedMsg();
private:
    void setupDefaults();
    void enableSetupFields(bool enabled);
    void triggerMsgPlot();
    void updateCalculations();

    QWidget *m_rootWidget;
    QGridLayout *m_rootLay;


    QGroupBox *m_sendGroupBox;
    QGroupBox *m_receivedGroupBox;
    QGroupBox *m_setupGroupBox;
    QGroupBox *m_calcGroupBox;
    QGroupBox *m_viewGroupBox;
    QGroupBox *m_plotGroupBox;

    // Send area
    QGridLayout *m_sendGridLay;
    QPlainTextEdit *m_msgToSendEdit;
    QComboBox *m_encodingComboBox;
    QPushButton *m_sendOneBtn;
    QPushButton *m_sendBtn;

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

    // Calculations area
    QGridLayout *m_calcGridLay;

    QLabel *m_calcLbl;


    // View area
    QGridLayout *m_viewGridLay;

    qlementine::SegmentedControl *m_viewSentRecvSwitch;
    QPushButton *m_prevBitsBtn;
    QPushButton *m_nextBitsBtn;
    QSpinBox *m_plotBitsSpinBox;


    // Plot area
    QGridLayout *m_plotGridLay;

    SignalPlotter *m_signalPlotter;


    QString m_baseWindowTitle;

    bool m_isPortOpened;
    QSerialPort m_serialPort;

    int m_charToSendIndex;

    int m_plotBitsIndexLeft;
    int m_plotBitsIndexRight;

    bool m_plotRecvSent;
    QByteArray m_recvMsgBytes;
    QByteArray m_sentMsgBytes;
};

#endif // MAINWINDOW_H
