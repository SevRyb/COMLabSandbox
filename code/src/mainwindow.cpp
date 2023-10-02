
#include "mainwindow.h"

#include <QDebug>
#include <QTextCodec>
#include <QByteArray>

using namespace oclero;

MainWindow::MainWindow(QWidget *parent)
    : qlementine::FramelessWindow(parent)
    , m_baseWindowTitle("COMLabSandbox")
    , m_isPortOpened(false)
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

    m_msgToSendEdit = new QPlainTextEdit(m_sendGroupBox);
    m_encodingComboBox = new QComboBox(m_sendGroupBox);
    m_encodingComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_sendBtn = new QPushButton("Send", m_sendGroupBox);
    m_sendBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    QLabel *lbl = new QLabel("Message encoding", m_msgToSendEdit);
    //lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    m_sendGridLay->addWidget(m_msgToSendEdit, 0, 0, 1, 3);
    m_sendGridLay->addWidget(lbl, 1, 0);
    m_sendGridLay->addWidget(m_encodingComboBox, 1, 1);
    m_sendGridLay->addWidget(m_sendBtn, 1, 2);
    m_sendGridLay->setAlignment(lbl, Qt::AlignRight);

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


    // Plot area
    m_plotGridLay = new QGridLayout(m_plotGroupBox);

    m_signalPlotter = new SignalPlotter(m_plotGroupBox);
    //m_signalPlotter->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_plotGridLay->addWidget(m_signalPlotter, 0, 0);



    m_rootLay->addWidget(m_sendGroupBox, 0, 0, 1, 1);
    m_rootLay->addWidget(m_receivedGroupBox, 1, 0, 1, 1);
    m_rootLay->addWidget(m_setupGroupBox, 0, 1, 2, 1);
    m_rootLay->addWidget(m_plotGroupBox, 2, 0, 1, 2);

    m_rootLay->setRowStretch(0, 1);
    m_rootLay->setRowStretch(1, 2);
    m_rootLay->setRowStretch(2, 2);


    /**/
    //m_msgToSendEdit->setClearButtonEnabled(true);
    m_msgToSendEdit->setPlaceholderText("Data");
    m_receivedMsgsEdit->setPlaceholderText("Received");

    /* Connections */
    connect(&m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    connect(m_openCloseBtn, &QPushButton::clicked, this, &MainWindow::onOpenClosePort);
    connect(m_portComboBox, &ComboBox::pressed, this, &MainWindow::onRefreshPorts);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMsg);

    setupDefaults();

    onRefreshPorts();

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
    QByteArray data = m_serialPort.readAll();


    QTextCodec *codec = QTextCodec::codecForName(m_encodingComboBox->currentData().toByteArray());
    m_receivedMsgsEdit->setPlainText(codec->toUnicode(data));
}

void MainWindow::onRefreshPorts()
{
    QString prev_port_name = m_portComboBox->currentText();
    m_portComboBox->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo &port : ports)
    {
        m_portComboBox->addItem(port.portName());
    }
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

        qDebug() << "Boud rate: " << m_serialPort.baudRate();
        qDebug() << "Parity: " << m_serialPort.parity();
        qDebug() << "Stop bits: " << m_serialPort.stopBits();
        qDebug() << "Data bits: " << m_serialPort.dataBits();

        QByteArray b("a");
        m_signalPlotter->visualizeMsg(m_serialPort, b);

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

void MainWindow::onSendMsg()
{
    if (m_isPortOpened)
    {
        QString text = m_msgToSendEdit->toPlainText();
        QTextCodec *codec = QTextCodec::codecForName(m_encodingComboBox->currentData().toByteArray());
        QByteArray data = codec->fromUnicode(text);
        quint64 written = m_serialPort.write(data);
    }
//    int i = 0;
//    //QString text = m_msgToSendEdit->text();
//    char c = static_cast<char>(247);
//    QChar ch(c);
//    QString text;
//    text.append(ch);
//    QList lst = QTextCodec::availableCodecs();
//    for (auto &e : lst)
//    {
//        QTextCodec *codec = QTextCodec::codecForName(e);
//        QByteArray data = codec->fromUnicode(text);
//        m_receivedMsgsEdit->appendPlainText(e);
//        m_receivedMsgsEdit->appendPlainText(codec->toUnicode(data));
//        m_receivedMsgsEdit->appendPlainText(data.toHex());
//        qDebug() << data.toHex();
//        i++;
//    }
}

void MainWindow::setupDefaults()
{
    // Send area
    QList available_codecs = QTextCodec::availableCodecs();
    for (QByteArray &codec_name : available_codecs)
    {
        m_encodingComboBox->addItem(codec_name, codec_name);
    }
    int index = m_encodingComboBox->findText("UTF-8");
    if (index != -1)
        m_encodingComboBox->setCurrentIndex(index);

    // Config area
    m_baudRateSpinBox->setRange(0, 115200);
    m_baudRateSpinBox->setValue(100);
    QStringList parity_options = {"NoParity", "EvenParity", "OddParity"};
    m_parityComboBox->addItems(parity_options);
    m_parityComboBox->setItemData(0, QSerialPort::NoParity);
    m_parityComboBox->setItemData(1, QSerialPort::EvenParity);
    m_parityComboBox->setItemData(2, QSerialPort::OddParity);
    m_parityComboBox->setCurrentIndex(2);

    m_stopBitsSpinBox->setRange(1, 2);
    m_stopBitsSpinBox->setValue(1);
    m_dataBitsSpinBox->setRange(5, 8);
    m_dataBitsSpinBox->setValue(8);

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





