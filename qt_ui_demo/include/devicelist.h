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

#ifndef CDEVICELIST_H
#define CDEVICELIST_H

#include "comminclude.h"

class CDeviceList : public QWidget
{
    Q_OBJECT
public:
    explicit CDeviceList(QSharedPointer<CRequestSender> &requestSender, QWidget *parent = nullptr);

    void initDevBriefWidget(const QStringList &list, QTableWidget *table);

    void updateDevList(const QJsonArray &data);
    void devListItemChanged();
signals:

public slots:
    void sendGetDeviceListRequest();
    void serverDataParse(int protocol, const QJsonObject &obj);

private:
    QPushButton *m_devInfoFreshButton;
    QListWidget *m_devListWidget;
    QTableWidget *m_devBriefTableWidget;
    QSharedPointer<CRequestSender> m_requestSender;

    QJsonArray m_devListInfo;
    int        m_lastSelectDevIdx;
    qint64     m_lastOpenedDevDes;
};

#endif // CDEVICELIST_H
