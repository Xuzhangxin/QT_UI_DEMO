
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
#ifndef CDPMANAGER_H
#define CDPMANAGER_H

#include "comminclude.h"

enum RW_MODE
{
    M_RW = 0,
    M_WR = 1,
    M_RO = 2,
};

class CDpManager : public QWidget
{
    Q_OBJECT
public:
    explicit CDpManager(QSharedPointer<CRequestSender> &requestSender, QWidget *parent = nullptr);
    void initDpTableWidget(const QStringList &list, QTableWidget *table);

    void updateDpTables(const QJsonArray &data);
    void updateDpTable(const std::vector<int> &idxs, const QJsonArray &data, QTableWidget *table, RW_MODE mode);
    void updateSingleDp(const QJsonObject &data);

    QWidget *getIntValueSetWidget(int rowIdx, int min, int max, int step, int curVal, QWidget *parent, RW_MODE mode = M_RW);
    QWidget *getEnumValueSetWidget(const QJsonArray &range, const QString &curValue, QWidget *parent, RW_MODE mode = M_RW);
    QWidget *getStrValueSetWidget(int rowIdx, const QString &curValue, QWidget *parent, RW_MODE mode = M_RW);
    QWidget *getLightColorValueSetWidget(int rowIdx, const QString &curValue, QWidget *parent, RW_MODE mode = M_RW);
signals:

public slots:
    void serverDataParse(int protocol, const QJsonObject &obj);

    void sendGetSingleRWDpInfoRequest();
    void sendGetSingleORDpInfoRequest();

    void sendEnumDpSetRequest(const QString &str);
    void sendStrDpSetRequest();
    void sendIntValueDpSetRequest(int value);
    void selectLightColor();

private:
    QTableWidget *m_readAndWriteDpTableWidget;
    QTableWidget *m_onlyReadDpTableWidget;
    QSharedPointer<CRequestSender> m_requestSender;
    qint64 m_lastOpenedDevDes;
};

#endif // CDPMANAGER_H
