
#include "mainwindow.h"

#include <QDebug>
#include <QTextCodec>
#include <QByteArray>

using namespace oclero;

MainWindow::MainWindow(QWidget *parent)
    : qlementine::FramelessWindow(parent)
    , m_baseWindowTitle("COMLabSandbox")
    , m_isPortOpened(false)
    , m_charToSendIndex(0)
    , m_plotBitsIndexLeft(0)
    , m_plotBitsIndexRight(0)
    , m_plotRecvSent(0)
{
    setWindowTitle(m_baseWindowTitle);
    resize(1200, 800);
    setMinimumHeight(900);
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
    m_calcGroupBox = new QGroupBox(this);
    m_calcGroupBox->setTitle("Calculations");
    m_viewGroupBox = new QGroupBox(this);
    m_viewGroupBox->setTitle("View");
    m_plotGroupBox = new QGroupBox(this);
    m_plotGroupBox->setTitle("Plot");

    // Send area
    m_sendGridLay = new QGridLayout(m_sendGroupBox);

    m_msgToSendEdit = new QPlainTextEdit(m_sendGroupBox);
    m_encodingComboBox = new QComboBox(m_sendGroupBox);
    m_encodingComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_sendOneBtn = new QPushButton("Send char at 0", m_sendGroupBox);
    m_sendOneBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_sendBtn = new QPushButton("Send", m_sendGroupBox);
    m_sendBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    QLabel *msgEncodingLbl = new QLabel("Message encoding", m_msgToSendEdit);

    m_sendGridLay->addWidget(m_msgToSendEdit, 0, 0, 1, 4);
    m_sendGridLay->addWidget(msgEncodingLbl, 1, 0);
    m_sendGridLay->addWidget(m_encodingComboBox, 1, 1);
    m_sendGridLay->addWidget(m_sendOneBtn, 1, 2);
    m_sendGridLay->addWidget(m_sendBtn, 1, 3);
    m_sendGridLay->setAlignment(msgEncodingLbl, Qt::AlignRight);

    // Received area
    m_receivedGridLay = new QGridLayout(m_receivedGroupBox);
    m_receivedMsgsEdit = new QPlainTextEdit(m_receivedGroupBox);
    m_receivedGridLay->addWidget(m_receivedMsgsEdit, 0, 0);

    // Setup area
    m_setupGridLay = new QGridLayout(m_setupGroupBox);

    m_portComboBox = new ComboBox(m_setupGroupBox);
    m_baudRateSpinBox = new QSpinBox(m_setupGroupBox);
    m_parityComboBox = new QComboBox(m_setupGroupBox);
    m_stopBitsSpinBox = new QSpinBox(m_setupGroupBox);
    m_dataBitsSpinBox = new QSpinBox(m_setupGroupBox);
    m_wordsDelaySpinBox = new QSpinBox(m_setupGroupBox);
    m_portOpenStatusWidget = new qlementine::StatusBadgeWidget(qlementine::StatusBadge::Error, qlementine::StatusBadgeSize::Medium, m_setupGroupBox);
    m_openCloseBtn = new QPushButton("Open", m_setupGroupBox);

    m_setupGridLay->addWidget(new QLabel("Port", m_setupGroupBox), 0, 0);
    m_setupGridLay->addWidget(new QLabel("Baud rate\n(bit/s)", m_setupGroupBox), 1, 0);
    m_setupGridLay->addWidget(new QLabel("Parity", m_setupGroupBox), 2, 0);
    m_setupGridLay->addWidget(new QLabel("Stop bits", m_setupGroupBox), 3, 0);
    m_setupGridLay->addWidget(new QLabel("Data bits", m_setupGroupBox), 4, 0);
    m_setupGridLay->addWidget(new QLabel("Words delay\n(tacts)", m_setupGroupBox), 5, 0);
    m_setupGridLay->addWidget(m_portComboBox, 0, 1);
    m_setupGridLay->addWidget(m_baudRateSpinBox, 1, 1);
    m_setupGridLay->addWidget(m_parityComboBox, 2, 1);
    m_setupGridLay->addWidget(m_stopBitsSpinBox, 3, 1);
    m_setupGridLay->addWidget(m_dataBitsSpinBox, 4, 1);
    m_setupGridLay->addWidget(m_wordsDelaySpinBox, 5, 1);

    m_setupGridLay->addWidget(m_portOpenStatusWidget, 6, 0);
    m_setupGridLay->addWidget(m_openCloseBtn, 6, 1);

    // Calculations area
    m_calcGridLay = new QGridLayout(m_calcGroupBox);

    m_calcLbl = new QLabel("\n\n\n", m_calcGroupBox);
    m_calcLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFont font = m_calcLbl->font();
    font.setFamily("Consolas");
    font.setPixelSize(16);
    //font.setItalic(true);
    //font.setBold(true);
    m_calcLbl->setFont(font);

    m_calcGridLay->addWidget(m_calcLbl);

    // View area
    m_viewGridLay = new QGridLayout(m_viewGroupBox);

    m_viewSentRecvSwitch = new qlementine::SegmentedControl(m_viewGroupBox);
    m_viewSentRecvSwitch->addItem("Recv");
    m_viewSentRecvSwitch->addItem("Sent");

    m_prevBitsBtn = new QPushButton("Prev", m_viewGroupBox);
    m_nextBitsBtn = new QPushButton("Next", m_viewGroupBox);
    m_plotBitsSpinBox = new QSpinBox(m_viewGroupBox);

    m_viewGridLay->addWidget(m_viewSentRecvSwitch, 0, 0, 1, 3);
    m_viewGridLay->addWidget(m_prevBitsBtn, 1, 0);
    m_viewGridLay->addWidget(m_plotBitsSpinBox, 1, 1);
    m_viewGridLay->addWidget(m_nextBitsBtn, 1, 2);


    // Plot area
    m_plotGridLay = new QGridLayout(m_plotGroupBox);

    m_signalPlotter = new SignalPlotter(m_plotGroupBox);
    //m_signalPlotter->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_signalPlotter->setFixedHeight(220);

    m_plotGridLay->addWidget(m_signalPlotter, 0, 0);



    m_rootLay->addWidget(m_sendGroupBox, 0, 0);
    m_rootLay->addWidget(m_receivedGroupBox, 1, 0);
    m_rootLay->addWidget(m_setupGroupBox, 0, 1, 2, 1);
    m_rootLay->addWidget(m_calcGroupBox, 2, 0);
    m_rootLay->addWidget(m_viewGroupBox, 2, 1);
    m_rootLay->addWidget(m_plotGroupBox, 3, 0, 1, 2);

    m_rootLay->setRowStretch(0, 1);
    m_rootLay->setRowStretch(1, 1);
    m_rootLay->setRowStretch(2, 0);
    m_rootLay->setRowStretch(3, 0);


    /**/
    m_msgToSendEdit->setPlaceholderText("Data");
    m_receivedMsgsEdit->setPlaceholderText("Received");

    /* Connections */
    connect(&m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    connect(m_openCloseBtn, &QPushButton::clicked, this, &MainWindow::onOpenClosePort);
    connect(m_portComboBox, &ComboBox::pressed, this, &MainWindow::onRefreshPorts);
    connect(m_sendOneBtn, &QPushButton::clicked, this, &MainWindow::onSendOne);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSend);
    connect(m_viewSentRecvSwitch, &qlementine::SegmentedControl::currentIndexChanged, this, [this]()
            {
                m_plotRecvSent = m_viewSentRecvSwitch->currentIndex();
                triggerMsgPlot();
                updateCalculations();
            });
    connect(m_prevBitsBtn, &QPushButton::clicked, this, [this]()
            {
                if (!m_recvMsgBytes.isEmpty())
                {
                    int len = m_recvMsgBytes.length();
                    int size = _moveChunk(m_plotBitsIndexLeft, m_plotBitsIndexRight, len, m_plotBitsSpinBox->value(), false);
                    if (size != 0)
                        m_signalPlotter->visualizeMsg(&m_serialPort, m_wordsDelaySpinBox->value(), m_recvMsgBytes.mid(m_plotBitsIndexLeft, size));
                }
            });
    connect(m_nextBitsBtn, &QPushButton::clicked, this, [this]()
            {
                if (!m_recvMsgBytes.isEmpty())
                {
                    int len = m_recvMsgBytes.length();
                    int size = _moveChunk(m_plotBitsIndexLeft, m_plotBitsIndexRight, len, m_plotBitsSpinBox->value());
                    if (size != 0)
                        m_signalPlotter->visualizeMsg(&m_serialPort, m_wordsDelaySpinBox->value(), m_recvMsgBytes.mid(m_plotBitsIndexLeft, size));
                }
            });

    setupDefaults();
    onRefreshPorts();
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_serialPort.close();
    qlementine::FramelessWindow::closeEvent(event);
}

void MainWindow::onRefreshPorts()
{
    QString prev_port_name = m_portComboBox->currentText();
    m_portComboBox->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo &port : ports)
        m_portComboBox->addItem(port.portName());
    int index = m_portComboBox->findText(prev_port_name);
    if (index != -1)
        m_portComboBox->setCurrentIndex(index);
}

void MainWindow::onOpenClosePort()
{
    if (m_isPortOpened)
    {
        m_serialPort.close();
        m_isPortOpened = false;
        m_portOpenStatusWidget->setBadge(qlementine::StatusBadge::Error);
        m_openCloseBtn->setText("Open");
        enableSetupFields(true);
    }
    else
    {
        QString port_name = m_portComboBox->currentText();
        if (port_name.isEmpty())
            return;
        QSerialPortInfo port(port_name);

        /* Open port with setup */
        m_serialPort.setPortName(port.portName());
        m_serialPort.setBaudRate(m_baudRateSpinBox->value());
        m_serialPort.setParity(m_parityComboBox->currentData().value<QSerialPort::Parity>());
        m_serialPort.setStopBits(QVariant(m_stopBitsSpinBox->value()).value<QSerialPort::StopBits>());
        m_serialPort.setDataBits(QVariant(m_dataBitsSpinBox->value()).value<QSerialPort::DataBits>());
        m_isPortOpened = m_serialPort.open(QIODevice::ReadWrite);

        if (m_isPortOpened)
        {
            m_portOpenStatusWidget->setBadge(qlementine::StatusBadge::Success);
            m_openCloseBtn->setText("Close");
            enableSetupFields(false);
        }
        else
            m_portOpenStatusWidget->setBadge(qlementine::StatusBadge::Error);
    }
}

void MainWindow::onReadyRead()
{
    m_recvMsgBytes = m_serialPort.readAll();

    QTextCodec *codec = QTextCodec::codecForName(m_encodingComboBox->currentData().toByteArray());
    m_receivedMsgsEdit->setPlainText(codec->toUnicode(m_recvMsgBytes));
    triggerMsgPlot();
    updateCalculations();
}

void MainWindow::onSend()
{
    if (m_isPortOpened)
    {
        QString text = m_msgToSendEdit->toPlainText();
        QTextCodec *codec = QTextCodec::codecForName(m_encodingComboBox->currentData().toByteArray());
        m_sentMsgBytes = codec->fromUnicode(text);
        m_serialPort.write(m_sentMsgBytes);
        triggerMsgPlot();
        updateCalculations();
        m_charToSendIndex = 0;
    }
}

void MainWindow::onSendOne()
{
    if (m_isPortOpened)
    {
        QString text = m_msgToSendEdit->toPlainText();
        if (!text.isEmpty())
        {
            int len = text.length();
            if (m_charToSendIndex >= len)
                m_charToSendIndex = 0;
            QChar ch = text.at(m_charToSendIndex);
            QTextCodec *codec = QTextCodec::codecForName(m_encodingComboBox->currentData().toByteArray());
            m_sentMsgBytes = codec->fromUnicode(ch);
            m_serialPort.write(m_sentMsgBytes);
            triggerMsgPlot();
            updateCalculations();
            if (m_charToSendIndex < (len - 1))
                m_charToSendIndex++;
            else
                m_charToSendIndex = 0;
            m_sendOneBtn->setText(QString("Send \"%1\" at %2").arg(text.at(m_charToSendIndex)).arg(m_charToSendIndex));
        }
    }
}

void MainWindow::setupDefaults()
{
    // Send area
    QList available_codecs = QTextCodec::availableCodecs();
    for (QByteArray &codec_name : available_codecs)
        m_encodingComboBox->addItem(codec_name, codec_name);
    int index = m_encodingComboBox->findText("CP866");  // [!] Change to "UTF-8"
    if (index != -1)
        m_encodingComboBox->setCurrentIndex(index);
    // Config area
    m_baudRateSpinBox->setRange(1, 921600);
    m_baudRateSpinBox->setValue(100);
    QStringList parity_options = {"NoParity", "EvenParity", "OddParity"};
    m_parityComboBox->addItems(parity_options);
    m_parityComboBox->setItemData(0, QSerialPort::NoParity);
    m_parityComboBox->setItemData(1, QSerialPort::EvenParity);
    m_parityComboBox->setItemData(2, QSerialPort::OddParity);
    m_parityComboBox->setCurrentIndex(2);
    m_stopBitsSpinBox->setRange(1, 2);
    m_stopBitsSpinBox->setValue(1);
    m_dataBitsSpinBox->setRange(8, 8);
    m_dataBitsSpinBox->setValue(8);
    m_wordsDelaySpinBox->setValue(1);
    // View area
    m_plotBitsSpinBox->setValue(20);
    m_plotBitsSpinBox->setRange(1, 40);
}

void MainWindow::enableSetupFields(bool enabled)
{
    m_portComboBox->setEnabled(enabled);
    m_baudRateSpinBox->setEnabled(enabled);
    m_parityComboBox->setEnabled(enabled);
    m_stopBitsSpinBox->setEnabled(enabled);
    m_dataBitsSpinBox->setEnabled(enabled);
    m_wordsDelaySpinBox->setEnabled(enabled);
}

void MainWindow::triggerMsgPlot()
{
    m_plotBitsIndexLeft = 0;
    m_plotBitsIndexRight = m_plotBitsSpinBox->value() - 1;
    if (m_plotRecvSent)
        m_signalPlotter->visualizeMsg(&m_serialPort, m_wordsDelaySpinBox->value(), m_sentMsgBytes.mid(m_plotBitsIndexLeft, m_plotBitsSpinBox->value()));
    else
        m_signalPlotter->visualizeMsg(&m_serialPort, m_wordsDelaySpinBox->value(), m_recvMsgBytes.mid(m_plotBitsIndexLeft, m_plotBitsSpinBox->value()));
}

void MainWindow::updateCalculations()
{
    if (!m_isPortOpened)
    {
        //m_calcLbl->clear();
        m_calcLbl->setText("\n\n\n");
        return;
    }

    int msgWords = 0;
    if (m_plotRecvSent)
        msgWords = m_sentMsgBytes.length();
    else
        msgWords = m_recvMsgBytes.length();

    if (msgWords == 0)
    {
        //m_calcLbl->clear();
        m_calcLbl->setText("\n\n\n");
        return;
    }

    int dataBits = m_dataBitsSpinBox->value();
    int stopBits = m_stopBitsSpinBox->value();
    int delayTacts = m_wordsDelaySpinBox->value();

    int parityBit = 0;
    if (m_serialPort.parity() != QSerialPort::NoParity)
        parityBit++;

    int baudRate = m_serialPort.baudRate();
    float T = (float) 1.0 / baudRate;

    int tacts = msgWords * (1 + dataBits + parityBit + stopBits + delayTacts) - 1;
    float msgTime = (float) T * tacts;

    QString text = QString(("words = %1\n"
                            "tacts = %2\n"
                            "T = 1 / baudRate = %3 s = %4 ms\n"
                            "time = T * tacts = %5 s = %6 ms"))
                       .arg(msgWords).arg(tacts).arg(T).arg(T * 1000).arg(msgTime).arg(msgTime * 1000);
    m_calcLbl->setText(text);
}





