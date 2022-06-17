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

#include "roomlist.h"

CRoomList::CRoomList(QSharedPointer<CRequestSender> &requestSender, QWidget *parent) : QWidget(parent)
{
    m_requestSender = requestSender;

    m_roomListTable = new QTableWidget(this);

    m_roomListFreshButton = new QPushButton(tr("获取/刷新列表"));
    connect(m_roomListFreshButton, &QAbstractButton::clicked, this, &CRoomList::sendGetRoomListRequest);

    QStringList headers;
    headers << tr("房间id") << tr("房间名称") << tr("设备id");

    m_roomListTable->setColumnCount(headers.size());
    m_roomListTable->setHorizontalHeaderLabels(headers);
    m_roomListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_roomListTable->setColumnWidth(1, 80);
    m_roomListTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    QVBoxLayout *sceneListLayout = new QVBoxLayout(this);

    sceneListLayout->addWidget(m_roomListFreshButton);
    sceneListLayout->addWidget(m_roomListTable);

    setLayout(sceneListLayout);
}

void CRoomList::serverDataParse(int protocol, const QJsonObject &obj)
{
    if (protocol == GET_ROOM_LIST_RESPONSE
        && obj.contains("data") && obj["data"].isArray())
    {
        updateRoomList(obj["data"].toArray());
    }
}

void CRoomList::sendGetRoomListRequest()
{
    m_requestSender->getRoomListRequest();
}

void CRoomList::updateRoomList(const QJsonArray &data)
{
    int rowCount = 0;
    for (int i = 0; i < data.size(); ++i)
    {
        QJsonObject obj = data.at(i).toObject();
        rowCount += obj["dev_list"].toArray().size() > 0 ? obj["dev_list"].toArray().size() : 1;
    }

    m_roomListTable->clearContents();
    m_roomListTable->setRowCount(rowCount);

    int rowIdx = 0;
    for (int i = 0; i < data.size(); ++i)
    {
        QJsonObject obj = data.at(i).toObject();
        QJsonArray dev_list = obj["dev_list"].toArray();

        if (dev_list.size() == 0)
        {
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(obj["id"].toInt()));
            item0->setBackground(QBrush(QColor(Qt::lightGray)));
            item0->setFlags(item0->flags() & (~Qt::ItemIsEditable));
            m_roomListTable->setItem(rowIdx, 0, item0);

            QTableWidgetItem *item1 = new QTableWidgetItem(obj["name"].toString());
            item1->setBackground(QBrush(QColor(Qt::lightGray)));
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            m_roomListTable->setItem(rowIdx, 1, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem(tr("房间内无设备"));
            item2->setBackground(QBrush(QColor(Qt::lightGray)));
            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            m_roomListTable->setItem(rowIdx, 2, item2);
            rowIdx++;
        }
        else
        {
            for (int j = 0; j < dev_list.size(); ++j)
            {
                QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(obj["id"].toInt()));
                item0->setBackground(QBrush(QColor(Qt::lightGray)));
                item0->setFlags(item0->flags() & (~Qt::ItemIsEditable));
                m_roomListTable->setItem(rowIdx, 0, item0);

                QTableWidgetItem *item1 = new QTableWidgetItem(obj["name"].toString());
                item1->setBackground(QBrush(QColor(Qt::lightGray)));
                item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
                m_roomListTable->setItem(rowIdx, 1, item1);

                QTableWidgetItem *item2 = new QTableWidgetItem(dev_list.at(j).toString());
                item2->setBackground(QBrush(QColor(Qt::lightGray)));
                item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
                m_roomListTable->setItem(rowIdx, 2, item2);
                rowIdx++;
            }
        }
    }
}
