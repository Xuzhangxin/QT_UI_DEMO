
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

#include "devicelist.h"

CDeviceList::CDeviceList(QSharedPointer<CRequestSender> &requestSender, QWidget *parent)
    : QWidget(parent)
    , m_lastOpenedDevDes(-1)
{
    m_requestSender = requestSender;
    QVBoxLayout *devInfoLayout = new QVBoxLayout(this);
    QLabel *devListLabel = new QLabel(tr("设备列表"), this);
    QLabel *devBriefLabel = new QLabel(tr("设备简要信息"), this);

    m_devInfoFreshButton = new QPushButton(tr("获取/刷新列表"));
    connect(m_devInfoFreshButton, &QAbstractButton::clicked, this, &CDeviceList::sendGetDeviceListRequest);

    m_devListWidget = new QListWidget();
    connect(m_devListWidget, &QListWidget::itemSelectionChanged, this, &CDeviceList::devListItemChanged);

    m_devBriefTableWidget = new QTableWidget();
    QStringList devBriefTableHeaderList;
    devBriefTableHeaderList << tr("名称") << tr("品类") << tr("设备ID") << tr("在线状态") << tr("图标路径");
    initDevBriefWidget(devBriefTableHeaderList, m_devBriefTableWidget);

    devInfoLayout->addWidget(m_devInfoFreshButton);
    devInfoLayout->addWidget(devListLabel, 0, Qt::AlignHCenter);
    devInfoLayout->addWidget(m_devListWidget);
    devInfoLayout->addWidget(devBriefLabel, 0, Qt::AlignHCenter);
    devInfoLayout->addWidget(m_devBriefTableWidget);

    setObjectName(QString::fromUtf8("CDeviceList"));
    setLayout(devInfoLayout);
}


void CDeviceList::initDevBriefWidget(const QStringList &list, QTableWidget *table)
{
    table->setColumnCount(1);
    table->setRowCount(list.size());

    QTableWidgetItem *hItem1 = new QTableWidgetItem();
    hItem1->setTextAlignment(Qt::AlignCenter);
    table->setHorizontalHeaderItem(0, hItem1);

    table->setVerticalHeaderLabels(list);
    table->setObjectName(QString::fromUtf8("tableWidget"));

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(table->sizePolicy().hasHeightForWidth());
    table->setSizePolicy(sizePolicy);
    table->horizontalHeader()->setVisible(false);
    table->horizontalHeader()->setCascadingSectionResizes(false);
    table->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(true);
    table->verticalHeader()->setMinimumSectionSize(19);
    table->verticalHeader()->setProperty("showSortIndicator", QVariant(false));
    table->verticalHeader()->setStretchLastSection(false);

}

void CDeviceList::updateDevList(const QJsonArray &data)
{
    m_devListInfo = data;
    m_devListWidget->clear();
    //更新ListWidget
    for (quint16 i = 0; i < m_devListInfo.size(); ++i)
    {
        QJsonValue value = m_devListInfo.at(i);
        if (value.isObject() && value.toObject().contains("name"))
        {
            m_devListWidget->insertItem(i, value.toObject().take("name").toString());
        }
    }

    // 暂时从0开始，后续从当前索引开始
    if (m_devListWidget->count() > 0)
    {
        m_devListWidget->setCurrentRow(0);
    }
}

void CDeviceList::devListItemChanged()
{
    if (m_lastSelectDevIdx != -1
        && m_lastSelectDevIdx != m_devListWidget->currentRow()
        && m_lastOpenedDevDes != -1)
    {
        m_requestSender->closeDevRequest(m_lastOpenedDevDes);
        m_lastOpenedDevDes = -1;
    }

    QJsonValue value = m_devListInfo.at(m_devListWidget->currentRow());
    if (value.isObject())
    {
        QJsonObject obj = value.toObject();
        m_devBriefTableWidget->setItem(0, 0, new QTableWidgetItem(obj.contains("name") ? obj["name"].toString() : ""));
        m_devBriefTableWidget->setItem(1, 0, new QTableWidgetItem(obj.contains("category") ? obj["category"].toString() : ""));
        m_devBriefTableWidget->setItem(2, 0, new QTableWidgetItem(obj.contains("id") ? obj["id"].toString() : ""));
        m_devBriefTableWidget->setItem(3, 0, new QTableWidgetItem(obj.contains("online_stat") ? (obj["online_stat"].toInt() ? tr("在线") : tr("离线")) : ("")));
        m_devBriefTableWidget->setItem(4, 0, new QTableWidgetItem(obj.contains("icon_path") ? obj["icon_path"].toString() : ""));
        if (obj.contains("id"))
        {
            m_requestSender->openDevRequest(obj["id"].toString());
        }
    }

    m_lastSelectDevIdx = m_devListWidget->currentRow();
}

void CDeviceList::sendGetDeviceListRequest()
{
    m_requestSender->getDevListRequest();
}

void CDeviceList::serverDataParse(int protocol, const QJsonObject &obj)
{
    if (protocol == GET_ALL_DEV_INFO_RESPONSE
        && obj.contains("data") && obj["data"].isArray())
    {
        updateDevList(obj["data"].toArray());
        devListItemChanged(); //手动更新下列表
    }
    else if (protocol == OPEN_DEV_RESPONSE
             && obj.contains("data")
             && obj["data"].toObject().contains("devDes"))
    {
        QJsonObject data = obj["data"].toObject();
        qDebug() << "open dev des = " << (qint64)(data["devDes"].toDouble());
        m_lastOpenedDevDes = (qint64)(data["devDes"].toDouble());
        // 打开后首次获取设备dp点信息
        m_requestSender->getAllDpsRequest(m_lastOpenedDevDes);
    }
    else if (protocol == CLOSE_DEV_RESPONSE
             && obj.contains("data")
             && obj["data"].toObject().contains("devDes"))
    {
        QJsonObject data = obj["data"].toObject();
        qDebug() << "close dev des = " << (qint64)(data["devDes"].toDouble());
        if (m_lastOpenedDevDes == (qint64)(data["devDes"].toDouble()))
        {
            m_lastOpenedDevDes = -1;
        }
    }
}

