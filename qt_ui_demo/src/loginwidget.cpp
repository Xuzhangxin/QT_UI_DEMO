/*
 * Copyright (C) 2021-2022 Symeon XU <xzx_work_2020@163.com>
 *
 * QT_UI_DEMO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * QT_UI_DEMO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

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
