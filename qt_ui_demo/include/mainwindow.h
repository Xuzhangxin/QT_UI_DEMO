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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "comminclude.h"

#include "loginwidget.h"
#include "functionswitch.h"
#include "dpmanager.h"
#include "devicelist.h"
#include "voiceupload.h"
#include "scenelist.h"
#include "roomlist.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    QWidget *getSubDevManagerWidget();

signals:
    void recvServerData(int protocol, const QJsonObject &data);

public slots:
    void dataParse();
    void functionItemChanged(const QString &functionItemName);

private:
    QSharedPointer<QTcpSocket> m_tcpClientPtr;
    QSharedPointer<CRequestSender> m_requestSender;
    CLoginWidget *m_loginWidget;
    CFunctionSwitch *m_functionSwitch;
    QWidget *m_subDevManagerWidget;
    CVoiceUpload *m_voiceUpLoad;
    CSceneList *m_sceneList;
    CRoomList *m_roomList;
    QStackedWidget *m_singleFunctionPage;

    CDpManager *m_dpManager;
    CDeviceList *m_subDevList;
    QTextEdit *m_logShower;
};
#endif // MAINWINDOW_H
