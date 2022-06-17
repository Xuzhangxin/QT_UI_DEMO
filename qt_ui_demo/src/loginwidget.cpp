#include "loginwidget.h"

#include <QHostAddress>

CLoginWidget::CLoginWidget(QSharedPointer<QTcpSocket> &tcpClientPtr, QWidget *parent) : QWidget(parent)
{
    m_tcpClientPtr = tcpClientPtr;

    QLabel *ipLabel = new QLabel(tr("服务端ip"));
    QLabel *portLabel = new QLabel(tr("端口"));
    m_ipEdit = new QLineEdit();
    m_portEdit = new QLineEdit();

    QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$"); //IP正则表达式
    QRegExpValidator *m_IPValidator = new QRegExpValidator(rx, this);
    m_ipEdit->setValidator(m_IPValidator);  // 设置IP的输入规则
    m_ipEdit->setText(tr("192.168.137.144")); //先设置下默认ip，测试使用

    QRegExp regx("[0-9]+$");
    QRegExpValidator *m_Validator = new QRegExpValidator(regx, this);
    m_portEdit->setValidator(m_Validator);  //设置端口输入数字
    m_portEdit->setText(tr("8080"));

    m_connectButton = new QPushButton(tr("连接"));
    m_disConnectButton = new QPushButton(tr("断开连接"));
    m_disConnectButton->setEnabled(false);

    connect(m_connectButton, &QAbstractButton::clicked, this, &CLoginWidget::startConnect);
    connect(m_disConnectButton, &QAbstractButton::clicked, this, &CLoginWidget::disConnect);
    connect(m_tcpClientPtr.get(), &QAbstractSocket::connected, this, &CLoginWidget::connected);

    setObjectName(QString::fromUtf8("CLoginWidget"));
    // setGeometry(QRect(0, 10, 1000, 40));
    QHBoxLayout *loginLayout = new QHBoxLayout(this);
    loginLayout->setObjectName(QString::fromUtf8("serverInfoLayout"));
    loginLayout->setContentsMargins(0, 0, 0, 0);

    loginLayout->addWidget(ipLabel);
    loginLayout->addWidget(m_ipEdit);
    loginLayout->addWidget(portLabel);
    loginLayout->addWidget(m_portEdit);
    loginLayout->addWidget(m_connectButton);
    loginLayout->addWidget(m_disConnectButton);
}

void CLoginWidget::startConnect()
{
    QHostAddress ipAddress;
    if (!ipAddress.setAddress(m_ipEdit->text()))
    {
        qDebug() << "ip address invalid";
        return;
    }

    int port = 8080; //默认8080端口
    if (!m_portEdit->text().isEmpty())
    {
        port = m_portEdit->text().toInt();
    }

    qDebug() << "startConnect ip : " << m_ipEdit->text() << ", port : " << port;
    m_tcpClientPtr->connectToHost(ipAddress, port);
}

void CLoginWidget::disConnect()
{
    m_tcpClientPtr->disconnectFromHost();
    m_connectButton->setEnabled(true);
    m_disConnectButton->setEnabled(false);
}

void CLoginWidget::connected()
{
    m_connectButton->setEnabled(false);
    m_disConnectButton->setEnabled(true);
}

void CLoginWidget::serverDataParse(int protocol, const QJsonObject &data)
{
    qDebug() << protocol;
    qDebug() << data;
}
