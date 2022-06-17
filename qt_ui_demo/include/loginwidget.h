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
#ifndef CLOGINWIDGET_H
#define CLOGINWIDGET_H
#include "comminclude.h"

class CLoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CLoginWidget(QSharedPointer<QTcpSocket> &tcpClientPtr, QWidget *parent = nullptr);

public slots:
    void startConnect();
    void disConnect();
    void connected();
    void serverDataParse(int protocol, const QJsonObject &data);

signals:

private:
    QSharedPointer<QTcpSocket> m_tcpClientPtr;
    QLineEdit *m_ipEdit;
    QLineEdit *m_portEdit;
    QPushButton *m_connectButton;
    QPushButton *m_disConnectButton;
};

#endif // CLOGINWIDGET_H
