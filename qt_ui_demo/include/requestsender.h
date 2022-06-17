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

#ifndef CREQUESTSENDER_H
#define CREQUESTSENDER_H

#include "comminclude.h"

#define GET_ALL_DEV_INFO_REQUEST 1
#define OPEN_DEV_REQUEST         2
#define CLOSE_DEV_REQUEST        3
#define GET_ALL_DPS_INFO_REQUEST 4
#define GET_SINGLE_DP_INFO_REQUEST 5
#define SET_SINGLE_DP_INFO_REQUEST 6
#define UPLOAD_VOICE_REQUEST    7
#define GET_SCENE_LIST_REQUEST 8
#define TRIGGER_SCENE_REQUEST 9
#define GET_ROOM_LIST_REQUEST 10

#define GET_ALL_DEV_INFO_RESPONSE 1
#define OPEN_DEV_RESPONSE        2
#define CLOSE_DEV_RESPONSE        3
#define GET_ALL_DPS_INFO_RESPONSE 4
#define GET_SINGLE_DP_INFO_RESPONSE 5
#define SET_SINGLE_DP_INFO_RESPONSE 6
#define UPLOAD_VOICE_RESPONSE    7
#define GET_SCENE_LIST_RESPONSE 8
#define TRIGGER_SCENE_RESPONSE  9
#define GET_ROOM_LIST_RESPONSE 10

#define SINGLE_DP_INFO_CHANGED_RESPONSE 101
#define REV_CLOUD_EXT_CUSTOM_RESPONSE   102

class CRequestSender
{
public:
    CRequestSender(QSharedPointer<QTcpSocket> &tcpClientPtr);

    int getDevListRequest();
    int openDevRequest(const QString &devID);
    int closeDevRequest(qint64 devDes);
    int getAllDpsRequest(qint64 devDes);
    int getDpRequest(qint64 devDes, const QString &dpCode);
    int setDpRequest(qint64 devDes, const QString &dpCode, const QString &type, const QString &value);
    int uploadVoiceDataRequest(const QString &fileName, QByteArray &data, int dataLen,bool isFinished);
    int getSceneListRequest();
    int triggerSingalSceneRequest(const QString &id);
    int getRoomListRequest();
private:
    void sendToTcp(QJsonObject &data);

private:
    QSharedPointer<QTcpSocket> m_tcpClientPtr;
};

#endif // CREQUESTSENDER_H
