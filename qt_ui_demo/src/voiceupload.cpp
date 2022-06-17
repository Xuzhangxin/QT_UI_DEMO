#include "voiceupload.h"

CVoiceUpload::CVoiceUpload(QSharedPointer<CRequestSender> &requestSender, QWidget *parent) : QWidget(parent)
{
    m_requestSender = requestSender;
    QGridLayout *voiceUploadLayout = new QGridLayout(this);
    voiceUploadLayout->setObjectName(QString::fromUtf8("voiceUploadLayout"));
    voiceUploadLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *voiceFileListLabel = new QLabel(tr("可选音频文件列表(双击发送)"), this);
    voiceFileListLabel->setAlignment(Qt::AlignHCenter);
    m_voiceFileListWidget = new QListWidget(this);
    m_voiceFileListWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
    connect(m_voiceFileListWidget, &QListWidget::itemDoubleClicked, this, &CVoiceUpload::sendVoiceFile);

    QPushButton *voiceControlAdd = new QPushButton(this);
    voiceControlAdd->setText(tr("新增"));
    connect(voiceControlAdd, &QPushButton::clicked, this, &CVoiceUpload::addUploadVoiceFile);
    QPushButton *voiceControlDel = new QPushButton(this);
    voiceControlDel->setText(tr("删除"));
    connect(voiceControlDel, &QPushButton::clicked, this, &CVoiceUpload::delUploadVoiceFile);
    QPushButton *voiceControlClear = new QPushButton(this);
    voiceControlClear->setText(tr("清空"));
    connect(voiceControlClear, &QPushButton::clicked, this, &CVoiceUpload::clearUploadVoiceFile);

    QLabel *voiceToTextResultLabel = new QLabel(tr("音频转文本结果"), this);
    voiceToTextResultLabel->setAlignment(Qt::AlignHCenter);
    m_voiceToTextResult = new QTextEdit(this); // 音频原文转文本后返回
    QLabel *voiceParseResultLabel = new QLabel(tr("音频解析结果"), this);
    voiceParseResultLabel->setAlignment(Qt::AlignHCenter);
    m_voiceParseResult = new QTextEdit(this); // 音频原文处理

    voiceUploadLayout->addWidget(voiceFileListLabel, 0, 0, 1, 3);
    voiceUploadLayout->addWidget(m_voiceFileListWidget, 1, 0, 1, 3);
    voiceUploadLayout->addWidget(voiceControlAdd, 2, 0, 1, 1);
    voiceUploadLayout->addWidget(voiceControlDel, 2, 1, 1, 1);
    voiceUploadLayout->addWidget(voiceControlClear, 2, 2, 1, 1);
    voiceUploadLayout->addWidget(voiceToTextResultLabel, 3, 0, 1, 3);
    voiceUploadLayout->addWidget(m_voiceToTextResult, 4, 0, 1, 3);
    voiceUploadLayout->addWidget(voiceParseResultLabel, 5, 0, 1, 3);
    voiceUploadLayout->addWidget(m_voiceParseResult, 6, 0, 1, 3);
}

void CVoiceUpload::serverDataParse(int protocol, const QJsonObject &obj)
{
    if (protocol == REV_CLOUD_EXT_CUSTOM_RESPONSE
             && obj.contains("data")
             && obj["data"].toObject().contains("speaker")
             && obj["data"].toObject().contains("text"))
    {
        updateVoiceResponseText(obj["data"].toObject());
    }
}

void CVoiceUpload::sendVoiceFile(QListWidgetItem *item)
{
    QFile *voiceFile = new QFile(item->text());
    if (!voiceFile->open(QFile::ReadOnly))
    {
        qDebug() << QString("open %1 file error!").arg(item->text());
        return;
    }

    int totalSize = voiceFile->size();
    QByteArray data;

    data = voiceFile->read(640);
    totalSize -= data.size();

    while(totalSize > 0)
    {
        m_requestSender->uploadVoiceDataRequest(QFileInfo(item->text()).fileName(), data, data.size(), false);
        data = voiceFile->read(640);
        totalSize -= data.size();
    }

    m_requestSender->uploadVoiceDataRequest(QFileInfo(item->text()).fileName(), data, data.size(), true);
}

void CVoiceUpload::addUploadVoiceFile()
{
    QStringList fileNameList = QFileDialog::getOpenFileNames(
            this,
            tr("选择要上传的文件."),
            "Z:/",
            tr("voice files(*.wav *.mp3)"));

    if (!fileNameList.empty())
    {
        m_voiceFileListWidget->addItems(fileNameList);
    }
}

void CVoiceUpload::delUploadVoiceFile()
{
    QList<QListWidgetItem*> selectedItems = m_voiceFileListWidget->selectedItems();
    foreach (QListWidgetItem *item, selectedItems)
    {
        m_voiceFileListWidget->takeItem(m_voiceFileListWidget->row(item));
    }
}

void CVoiceUpload::clearUploadVoiceFile()
{
    m_voiceFileListWidget->clear();
}

void CVoiceUpload::updateVoiceResponseText(const QJsonObject &data)
{
    if (data["speaker"].toString() == "human")
    {
        m_voiceToTextResult->clear();
        m_voiceToTextResult->insertPlainText(data["text"].toString());
    }
    else if(data["speaker"].toString() == "robot")
    {
        m_voiceParseResult->clear();
        m_voiceParseResult->insertPlainText(data["text"].toString());
    }
}
