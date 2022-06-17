
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

#include "functionswitch.h"

CFunctionSwitch::CFunctionSwitch(QWidget *parent) : QWidget(parent)
{
    QLabel *functionLabel = new QLabel(tr("功能"));
    m_functionListWidget = new QListWidget();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(functionLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_functionListWidget);

    QStringList functionNames = {"子设备管理", "场景管理", "房间管理", "音频上传"};
    foreach(const QString &name, functionNames)
    {
        QListWidgetItem *item = new QListWidgetItem(name, m_functionListWidget);
        QPushButton *button = new QPushButton(name);
        m_functionListWidget->setItemWidget(item, button);
    }

    connect(m_functionListWidget, &QListWidget::itemSelectionChanged, this, &CFunctionSwitch::itemSelectionChanged);

    setLayout(layout);
}

void CFunctionSwitch::itemSelectionChanged()
{
    emit functionItemChanged(m_functionListWidget->currentItem()->text());
}
