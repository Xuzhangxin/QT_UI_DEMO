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
#include "requestsender.h"

CRequestSender::CRequestSender(QSharedPointer<QTcpSocket> &tcpClientPtr)
{
    m_tcpClientPtr = tcpClientPtr;
}

int CRequestSender::getDevListRequest()
{
    QJsonObject request;
    request.insert("protocol", GET_ALL_DEV_INFO_REQUEST);

    sendToTcp(request);
    return 0;
}

int CRequestSender::openDevRequest(const QString &devID)
{
    QJsonObject request;
    request.insert("protocol", OPEN_DEV_REQUEST);
    request.insert("devID", devID);

    sendToTcp(request);
    return 0;
}

int CRequestSender::closeDevRequest(qint64 devDes)
{
    QJsonObject request;
    request.insert("protocol", CLOSE_DEV_REQUEST);
    request.insert("devDes", devDes);

    sendToTcp(request);
    return 0;
}

int CRequestSender::getAllDpsRequest(qint64 devDes)
{
    QJsonObject request;
    request.insert("protocol", GET_ALL_DPS_INFO_REQUEST);
    request.insert("devDes", devDes);

    sendToTcp(request);
    return 0;
}

int CRequestSender::getDpRequest(qint64 devDes, const QString &dpCode)
{
    QJsonObject request;
    request.insert("protocol", GET_SINGLE_DP_INFO_REQUEST);
    request.insert("devDes", devDes);
    request.insert("code", dpCode);

    sendToTcp(request);
    return 0;
}

int CRequestSender::setDpRequest(qint64 devDes, const QString &dpCode, const QString &type, const QString &value)
{
    QJsonObject request;
    request.insert("protocol", SET_SINGLE_DP_INFO_REQUEST);
    request.insert("devDes", devDes);
    request.insert("code", dpCode);
    request.insert("type", type);
    if (type == "value")
    {
        request.insert("value", value.toInt());
    }
    else
    {
        request.insert("value", value);
    }

    sendToTcp(request);
    return 0;
}

int CRequestSender::uploadVoiceDataRequest(const QString &fileName, QByteArray &data, int dataLen,bool isFinished)
{
    QJsonObject request;
    request.insert("protocol", UPLOAD_VOICE_REQUEST);
    request.insert("fileName", fileName);
    request.insert("isFinished", isFinished);
    request.insert("data", QString(data.toBase64()));
    request.insert("dataLen", dataLen);

    sendToTcp(request);
    return 0;
}

int CRequestSender::getSceneListRequest()
{
    QJsonObject request;
    request.insert("protocol", GET_SCENE_LIST_REQUEST);
    sendToTcp(request);
    return 0;
}

int CRequestSender::triggerSingalSceneRequest(const QString &id)
{
    QJsonObject request;
    request.insert("protocol", TRIGGER_SCENE_REQUEST);
    request.insert("id", id);

    sendToTcp(request);
    return 0;
}

int CRequestSender::getRoomListRequest()
{
    QJsonObject request;
    request.insert("protocol", GET_ROOM_LIST_REQUEST);
    sendToTcp(request);
    return 0;
}



void CRequestSender::sendToTcp(QJsonObject &request)
{
    QJsonDocument document;
    document.setObject(request);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    data[data.size()] = '\n';
    qDebug() << "request : " << data;
    m_tcpClientPtr->write(data);
    m_tcpClientPtr->flush();
}
