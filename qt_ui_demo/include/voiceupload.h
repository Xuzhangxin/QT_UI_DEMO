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

#ifndef CVOICEUPLOAD_H
#define CVOICEUPLOAD_H

#include <QWidget>
#include "comminclude.h"

class CVoiceUpload : public QWidget
{
    Q_OBJECT
public:
    explicit CVoiceUpload(QSharedPointer<CRequestSender> &requestSender, QWidget *parent = nullptr);

signals:

public slots:
    void serverDataParse(int protocol, const QJsonObject &obj);

    void sendVoiceFile(QListWidgetItem *item);
    void addUploadVoiceFile();
    void delUploadVoiceFile();
    void clearUploadVoiceFile();
    void updateVoiceResponseText(const QJsonObject &data);

private:
    QSharedPointer<CRequestSender> m_requestSender;
    QListWidget     *m_voiceFileListWidget;
    QMenu           *m_voiceControlMenu;
    QTextEdit       *m_voiceToTextResult;
    QTextEdit       *m_voiceParseResult;
};

#endif // CVOICEUPLOAD_H
