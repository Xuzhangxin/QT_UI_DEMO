
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

#include "dpmanager.h"

CDpManager::CDpManager(QSharedPointer<CRequestSender> &requestSender, QWidget *parent)
    : QWidget(parent)
    , m_lastOpenedDevDes(-1)
{
    m_requestSender = requestSender;
    QVBoxLayout *dpManagerLayout = new QVBoxLayout(this);
    dpManagerLayout->setObjectName(QString::fromUtf8("dpManagerLayout"));
    dpManagerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *readAndWriteDpLabel = new QLabel(tr("指令dp"), this);
    QLabel *onlyReadDpLabel = new QLabel(tr("状态dp"), this);

    m_onlyReadDpTableWidget = new QTableWidget(this);
    QStringList onlyReadDpTableHeaderList;
    onlyReadDpTableHeaderList << tr("名称") << tr("数据类型") << tr("数值范围") << tr("实际数值") << tr("同步数据");
    initDpTableWidget(onlyReadDpTableHeaderList, m_onlyReadDpTableWidget);

    m_readAndWriteDpTableWidget = new QTableWidget(this);
    QStringList readAndWriteDpTableHeaderList;
    readAndWriteDpTableHeaderList << tr("名称") << tr("数据类型") << tr("范围及设置值") << tr("当前数值") << tr("同步数据");
    initDpTableWidget(readAndWriteDpTableHeaderList, m_readAndWriteDpTableWidget);

    dpManagerLayout->addWidget(readAndWriteDpLabel, 0, Qt::AlignHCenter);
    dpManagerLayout->addWidget(m_readAndWriteDpTableWidget);
    dpManagerLayout->addWidget(onlyReadDpLabel, 0, Qt::AlignHCenter);
    dpManagerLayout->addWidget(m_onlyReadDpTableWidget);

    setObjectName(QString::fromUtf8("CDpManager"));
    setLayout(dpManagerLayout);
}


void CDpManager::initDpTableWidget(const QStringList &list, QTableWidget *table)
{
    table->setColumnCount(list.size());
    table->setHorizontalHeaderLabels(list);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->setColumnWidth(1, 80);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->setColumnWidth(4, 80);
}

void CDpManager::serverDataParse(int protocol, const QJsonObject &obj)
{
    if (protocol == OPEN_DEV_RESPONSE
             && obj.contains("data")
             && obj["data"].toObject().contains("devDes"))
    {
        QJsonObject data = obj["data"].toObject();
        qDebug() << "open dev des = " << (qint64)(data["devDes"].toDouble());
        m_lastOpenedDevDes = (qint64)(data["devDes"].toDouble());
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
    else if (protocol == GET_ALL_DPS_INFO_RESPONSE && obj.contains("data") && obj["data"].isArray())
    {
        updateDpTables(obj["data"].toArray());
    }
    else if ((protocol == GET_SINGLE_DP_INFO_RESPONSE || protocol == SINGLE_DP_INFO_CHANGED_RESPONSE)
             && obj.contains("data")
             && obj["data"].toObject().contains("devDes")
             && obj["data"].toObject().contains("code")
             && obj["data"].toObject().contains("value")
             && m_lastOpenedDevDes == (qint64)(obj["data"].toObject()["devDes"].toDouble()))
    {
        updateSingleDp(obj["data"].toObject());
    }
}

void CDpManager::updateDpTables(const QJsonArray &data)
{
    std::vector<int> onlyReadDpIdxs;
    std::vector<int> readAndWriteDpIdxs;

    for (int i = 0; i < data.size(); ++i)
    {
        QJsonValue val = data.at(i);
        if (val.isObject() && val.toObject().contains("mode"))
        {
            if (val.toObject()["mode"].toInt() == 0) //读写模式
            {
                readAndWriteDpIdxs.push_back(i);
            }
            else if(val.toObject()["mode"].toInt() == 2) // 只读
            {
                onlyReadDpIdxs.push_back(i);
            }
        }
    }

    updateDpTable(readAndWriteDpIdxs, data, m_readAndWriteDpTableWidget, M_RW);
    updateDpTable(onlyReadDpIdxs, data, m_onlyReadDpTableWidget, M_RO);

    return;
}

void CDpManager::updateDpTable(const std::vector<int> &idxs, const QJsonArray &data, QTableWidget *table, RW_MODE mode)
{
    table->clearContents();
    table->setRowCount(idxs.size());

    for (size_t i = 0; i < idxs.size(); ++i)
    {
        QJsonObject obj = data.at(idxs[i]).toObject();
        QString valueType;
        if (! obj.contains("code") || !obj.contains("type") || !obj.contains("value"))
        {
            qDebug() << "idxs : " << i << " not valid";
            continue;
        }

        QTableWidgetItem *item0 = new QTableWidgetItem(obj["code"].toString());
        item0->setBackground(QBrush(QColor(Qt::lightGray)));
        item0->setFlags(item0->flags() & (~Qt::ItemIsEditable));
        table->setItem(i, 0, item0);

        valueType = obj["type"].toString();
        QTableWidgetItem *item1 = new QTableWidgetItem(obj["type"].toString());
        item1->setBackground(QBrush(QColor(Qt::lightGray)));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        table->setItem(i, 1, item1);

        QString valueStr;
        if (valueType == "bool")
        {
            valueStr = obj["value"].toInt() ? "true" : "false";
        }
        else if (valueType == "value")
        {
            valueStr = QString::number(obj["value"].toInt(), 10);
        }
        else if (valueType == "enum" || valueType == "str" || valueType == "light_color")
        {
            valueStr = obj["value"].toString();
        }

        if (obj.contains("range") && obj["range"].isArray() && obj["range"].toArray().size() > 0) //bool enum
        {
            QWidget *item2 = getEnumValueSetWidget(obj["range"].toArray(), valueStr, table, mode);
            table->setCellWidget(i, 2, item2);
        }
        else if (obj.contains("max") && obj.contains("min") && obj.contains("step") && obj.contains("scale"))
        {
            QWidget *item2 = getIntValueSetWidget(i
                                                , obj["min"].toInt()
                                                , obj["max"].toInt()
                                                , obj["step"].toInt()
                                                , obj["value"].toInt()
                                                , table
                                                , mode);
            table->setCellWidget(i, 2, item2);
        }
        else if (valueType == "light_color")
        {
             QWidget *item2 = getLightColorValueSetWidget(i, valueStr, table, mode);
             table->setCellWidget(i, 2, item2);
        }
        else // 其他情况，普通字符串输入
        {
            QWidget *item2 = getStrValueSetWidget(i, valueStr, table, mode);
            table->setCellWidget(i, 2, item2);
        }

        QTableWidgetItem *item3 = new QTableWidgetItem(valueStr);
        item3->setBackground(QBrush(QColor(Qt::lightGray)));
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        table->setItem(i, 3, item3);

        QPushButton *item4 = new QPushButton(tr("手动同步"));
        item4->setObjectName(QString::number(i));
        if (mode == M_RW)
        {
            connect(item4, &QAbstractButton::clicked, this, &CDpManager::sendGetSingleRWDpInfoRequest);
        }
        else if (mode == M_RO)
        {
            connect(item4, &QAbstractButton::clicked, this, &CDpManager::sendGetSingleORDpInfoRequest);
        }

        table->setCellWidget(i, 4, item4);
    }
}


void CDpManager::updateSingleDp(const QJsonObject &data)
{
    std::vector<QTableWidget *> tables;
    tables.push_back(m_readAndWriteDpTableWidget);
    tables.push_back(m_onlyReadDpTableWidget);

    for (size_t tableIdx = 0; tableIdx < tables.size(); ++tableIdx)
    {
        for (int rowIdx = 0; rowIdx < tables[tableIdx]->rowCount(); ++rowIdx)
        {
            if (tables[tableIdx]->item(rowIdx, 0)->text() == data["code"].toString())
            {
                QString valType = tables[tableIdx]->item(rowIdx, 1)->text();
                QString valStr;
                if (valType == "bool")
                {
                    valStr = data["value"].toInt() ? "true" : "false";
                }
                else if (valType == "enum" || valType == "str" || valType == "light_color")
                {
                    valStr = data["value"].toString();
                }
                else if (valType == "value")
                {
                    valStr = QString::number(data["value"].toInt());
                }
                tables[tableIdx]->item(rowIdx, 3)->setText(valStr);
                return;
            }
        }
    }
}

QWidget *CDpManager::getIntValueSetWidget(int rowIdx, int min, int max, int step, int curVal, QWidget *parent, RW_MODE mode)
{
    QWidget *intValueSetWidget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(intValueSetWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSpinBox *pSpinBox = new QSpinBox(intValueSetWidget);
    pSpinBox->setMinimum(min);
    pSpinBox->setMaximum(max);
    pSpinBox->setSingleStep(step);
    pSpinBox->setObjectName(QString::number(rowIdx));

    QSlider *pSlider = new QSlider(intValueSetWidget);
    pSlider->setOrientation(Qt::Horizontal);
    pSlider->setMinimum(min);
    pSlider->setMaximum(max);
    pSlider->setSingleStep(step);
    pSlider->setObjectName(QString::number(rowIdx));

    connect(pSpinBox, SIGNAL(valueChanged(int)), pSlider, SLOT(setValue(int)));
    connect(pSlider, SIGNAL(valueChanged(int)), pSpinBox, SLOT(setValue(int)));

    pSpinBox->setValue(curVal);

    if (mode != M_RO)
    {
        connect(pSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sendIntValueDpSetRequest(int)));
        connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(sendIntValueDpSetRequest(int)));
    }

    layout->addWidget(pSpinBox);
    layout->addWidget(pSlider);

    return intValueSetWidget;
}

QWidget *CDpManager::getEnumValueSetWidget(const QJsonArray &range, const QString &curValue, QWidget *parent, RW_MODE mode)
{
    QComboBox *comBox = new QComboBox(parent);
    int comBoxIdx = 0;
    for (int i = 0; i < range.size(); ++i)
    {
        comBox->addItem(range[i].toString());
        if (comBoxIdx == 0 && curValue == range[i].toString())
        {
            comBoxIdx = i;
        }
    }

    comBox->setCurrentIndex(comBoxIdx);
    if (mode != M_RO)
    {
        connect(comBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(sendEnumDpSetRequest(const QString &)));
    }

    return comBox;
}

QWidget *CDpManager::getStrValueSetWidget(int rowIdx, const QString &curValue, QWidget *parent, RW_MODE mode)
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setObjectName(QString::number(rowIdx));
    lineEdit->setText(curValue);

    if (mode != M_RO)
    {
        connect(lineEdit, &QLineEdit::editingFinished, this, &CDpManager::sendStrDpSetRequest);
    }

    return lineEdit;
}

QWidget *CDpManager::getLightColorValueSetWidget(int rowIdx, const QString &curValue, QWidget *parent, RW_MODE mode)
{
    (void)mode;
    QPushButton *button = new QPushButton(parent);
    button->setObjectName(QString::number(rowIdx));
    button->setText(curValue);

    connect(button, &QPushButton::clicked, this, &CDpManager::selectLightColor);
    return button;
}

void CDpManager::sendGetSingleRWDpInfoRequest()
{
    // 获取当前被点击的按钮
    QPushButton* pSenderObj = qobject_cast<QPushButton*>(sender());
    if (m_lastOpenedDevDes == -1 || pSenderObj == nullptr)
    {
        return;
    }

    // 获取当前按钮所在的行和列
    int row = pSenderObj->objectName().toInt();
    if (row < 0 || row > m_readAndWriteDpTableWidget->rowCount())
    {
        return;
    }

    QString code = m_readAndWriteDpTableWidget->item(row, 0)->text();
    m_requestSender->getDpRequest(m_lastOpenedDevDes, code);
}

void CDpManager::sendGetSingleORDpInfoRequest()
{
    // 获取当前被点击的按钮
    QPushButton* pSenderObj = qobject_cast<QPushButton*>(sender());
    if (m_lastOpenedDevDes == -1 || pSenderObj == nullptr)
    {
        return;
    }

    // 获取当前按钮所在的行和列
    int row = pSenderObj->objectName().toInt();
    if (row < 0 || row > m_onlyReadDpTableWidget->rowCount())
    {
        return;
    }

    QString code = m_onlyReadDpTableWidget->item(row, 0)->text();
    m_requestSender->getDpRequest(m_lastOpenedDevDes, code);
}

void CDpManager::sendEnumDpSetRequest(const QString &str)
{
    m_requestSender->setDpRequest(m_lastOpenedDevDes
                                    , m_readAndWriteDpTableWidget->item(m_readAndWriteDpTableWidget->currentRow(), 0)->text()
                                    , m_readAndWriteDpTableWidget->item(m_readAndWriteDpTableWidget->currentRow(), 1)->text()
                                    , str);
}

void CDpManager::sendStrDpSetRequest()
{
    QLineEdit* pSenderObj = qobject_cast<QLineEdit *>(sender());
    if (pSenderObj == nullptr)
    {
        return;
    }

    int rowIdx = pSenderObj->objectName().toInt();
    if (rowIdx < 0 || rowIdx > m_readAndWriteDpTableWidget->rowCount())
    {
        return;
    }

    m_requestSender->setDpRequest(m_lastOpenedDevDes
                                    , m_readAndWriteDpTableWidget->item(rowIdx, 0)->text()
                                    , m_readAndWriteDpTableWidget->item(rowIdx, 1)->text()
                                    , pSenderObj->text());

}

void CDpManager::sendIntValueDpSetRequest(int value)
{
    // 获取当前被点击的按钮
    QWidget* pSenderObj = qobject_cast<QWidget *>(sender());
    if (pSenderObj == nullptr)
    {
        return;
    }

    int rowIdx = pSenderObj->objectName().toInt();
    if (rowIdx < 0 || rowIdx > m_readAndWriteDpTableWidget->rowCount())
    {
        return;
    }

    m_requestSender->setDpRequest(m_lastOpenedDevDes
                                    , m_readAndWriteDpTableWidget->item(rowIdx, 0)->text()
                                    , m_readAndWriteDpTableWidget->item(rowIdx, 1)->text()
                                    , QString::number(value));

}

void CDpManager::selectLightColor()
{
    QPushButton* pSenderObj = qobject_cast<QPushButton *>(sender());

    if (pSenderObj == nullptr)
    {
        return;
    }

    int rowIdx = pSenderObj->objectName().toInt();
    if (rowIdx < 0 || rowIdx > m_readAndWriteDpTableWidget->rowCount())
    {
        return;
    }

    QColor curColor;
    int h = 0;
    int s = 0;
    int v = 0;
    sscanf(pSenderObj->text().toStdString().c_str(), "h:%d-s:%d-v:%d", &h, &s, &v);
    curColor.setHsv(h, s * 255 / 1000, v * 255 / 100);

    QColor newColor = QColorDialog::getColor(curColor, this, "选择颜色");
    if (newColor.isValid())
    {
        newColor.getHsv(&h, &s, &v);
        QString value = QString("h:%1-s:%2-v:%3") \
                        .arg(QString::number(h)) \
                        .arg(QString::number(s * 1000 / 255)) \
                        .arg(QString::number(v * 100 / 255));
        pSenderObj->setText(value);

        m_requestSender->setDpRequest(m_lastOpenedDevDes
                                        , m_readAndWriteDpTableWidget->item(rowIdx, 0)->text()
                                        , m_readAndWriteDpTableWidget->item(rowIdx, 1)->text()
                                        , value);
    }
}
