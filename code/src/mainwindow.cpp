
#include "mainwindow.h"

using namespace oclero;

MainWindow::MainWindow(QWidget *parent)
    : qlementine::FramelessWindow(parent)
    , m_baseWindowTitle("COMLabSandbox")
{
    setWindowTitle(m_baseWindowTitle);
    resize(1200, 800);
    m_rootLay = new QGridLayout;
    m_rootWidget = new QWidget(this);
    m_rootWidget->setLayout(m_rootLay);
    setContentWidget(m_rootWidget);


    m_sendGroupBox = new QGroupBox(this);
    m_sendGroupBox->setTitle("Send");
    m_receivedGroupBox = new QGroupBox(this);
    m_receivedGroupBox->setTitle("Received");
    m_setupGroupBox = new QGroupBox(this);
    m_setupGroupBox->setTitle("Setup");
    m_plotGroupBox = new QGroupBox(this);
    m_plotGroupBox->setTitle("Plot");

    // Send area
    m_sendGridLay = new QGridLayout(m_sendGroupBox);

    m_msgToSendEdit = new qlementine::LineEdit(m_sendGroupBox);
    m_asciiSwitch = new qlementine::Switch(m_sendGroupBox);
    m_asciiSwitch->setText("ASCII");
    m_asciiSwitch->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_sendBtn = new QPushButton("Send", m_sendGroupBox);
    m_sendBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    m_sendGridLay->addWidget(m_msgToSendEdit, 0, 0, 1, 2);
    m_sendGridLay->addWidget(m_asciiSwitch, 1, 0, 1, 1);
    m_sendGridLay->addWidget(m_sendBtn, 1, 1, 1, 1);

    // Received area
    m_receivedGridLay = new QGridLayout(m_receivedGroupBox);

    m_receivedMsgsEdit = new QPlainTextEdit(m_receivedGroupBox);

    m_receivedGridLay->addWidget(m_receivedMsgsEdit, 0, 0);


    // Setup area
    m_setupGridLay = new QGridLayout(m_setupGroupBox);

    m_portComboBox = new QComboBox(m_setupGroupBox);
    m_baudRateSpinBox = new QSpinBox(m_setupGroupBox);
    m_parityComboBox = new QComboBox(m_setupGroupBox);
    m_stopBitsComboBox = new QComboBox(m_setupGroupBox);
    m_dataBitsComboBox = new QComboBox(m_setupGroupBox);
    m_wordsDelaySpinBox = new QSpinBox(m_setupGroupBox);
    m_openCloseBtn = new QPushButton("Open", m_setupGroupBox);

    m_setupGridLay->addWidget(new QLabel("Port", m_setupGroupBox), 0, 0);
    m_setupGridLay->addWidget(new QLabel("Baud rate", m_setupGroupBox), 1, 0);
    m_setupGridLay->addWidget(new QLabel("Parity", m_setupGroupBox), 2, 0);
    m_setupGridLay->addWidget(new QLabel("Stop bits", m_setupGroupBox), 3, 0);
    m_setupGridLay->addWidget(new QLabel("Data bits", m_setupGroupBox), 4, 0);
    m_setupGridLay->addWidget(new QLabel("Words delay", m_setupGroupBox), 5, 0);
    m_setupGridLay->addWidget(m_portComboBox, 0, 1);
    m_setupGridLay->addWidget(m_baudRateSpinBox, 1, 1);
    m_setupGridLay->addWidget(m_parityComboBox, 2, 1);
    m_setupGridLay->addWidget(m_stopBitsComboBox, 3, 1);
    m_setupGridLay->addWidget(m_dataBitsComboBox, 4, 1);
    m_setupGridLay->addWidget(m_wordsDelaySpinBox, 5, 1);

    m_setupGridLay->addWidget(m_openCloseBtn, 6, 0, 1, 2);


    // Plot area
    m_plotGridLay = new QGridLayout(m_plotGroupBox);

    m_signalPlotter = new SignalPlotter(m_plotGroupBox);

    m_plotGridLay->addWidget(m_signalPlotter, 0, 0);



    m_rootLay->addWidget(m_sendGroupBox, 0, 0, 1, 1);
    m_rootLay->addWidget(m_receivedGroupBox, 1, 0, 1, 1);
    m_rootLay->addWidget(m_setupGroupBox, 0, 1, 2, 1);
    m_rootLay->addWidget(m_plotGroupBox, 2, 0, 1, 2);


    /**/
    m_msgToSendEdit->setClearButtonEnabled(true);
    m_msgToSendEdit->setPlaceholderText("Data");
    m_asciiSwitch->setChecked(true);
    m_receivedMsgsEdit->setPlaceholderText("Received");

    /* Connections */
    connect(&m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);

}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_serialPort.close();
    qlementine::FramelessWindow::closeEvent(event);
}

void MainWindow::onReadyRead()
{

}




