#include "scenelist.h"

CSceneList::CSceneList(QSharedPointer<CRequestSender> &requestSender, QWidget *parent) : QWidget(parent)
{
    m_requestSender = requestSender;

    m_sceneListTable = new QTableWidget(this);

    m_sceneListFreshButton = new QPushButton(tr("获取/刷新列表"));
    connect(m_sceneListFreshButton, &QAbstractButton::clicked, this, &CSceneList::sendGetSceneListRequest);

    QStringList headers;
    headers << tr("id") << tr("名称") << tr("图片路径") << tr("一键执行");

    m_sceneListTable->setColumnCount(headers.size());
    m_sceneListTable->setHorizontalHeaderLabels(headers);
    m_sceneListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_sceneListTable->setColumnWidth(1, 80);
    m_sceneListTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_sceneListTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    QVBoxLayout *sceneListLayout = new QVBoxLayout(this);

    sceneListLayout->addWidget(m_sceneListFreshButton);
    sceneListLayout->addWidget(m_sceneListTable);

    setLayout(sceneListLayout);
}

void CSceneList::serverDataParse(int protocol, const QJsonObject &obj)
{
    if (protocol == GET_SCENE_LIST_RESPONSE
        && obj.contains("data") && obj["data"].isArray())
    {
        updateSceneList(obj["data"].toArray());
    }
}


void CSceneList::sendGetSceneListRequest()
{
    m_requestSender->getSceneListRequest();
}

void CSceneList::updateSceneList(const QJsonArray &data)
{
    m_sceneListTable->clearContents();
    m_sceneListTable->setRowCount(data.size());

    //更新ListWidget
    for (int i = 0; i < data.size(); ++i)
    {
        QJsonObject obj = data.at(i).toObject();

        QTableWidgetItem *item0 = new QTableWidgetItem(obj["id"].toString());
        item0->setBackground(QBrush(QColor(Qt::lightGray)));
        item0->setFlags(item0->flags() & (~Qt::ItemIsEditable));
        m_sceneListTable->setItem(i, 0, item0);

        QByteArray name;
        name = obj["name"].toString().toStdString().c_str();
        name = QByteArray::fromBase64(name);
        QTableWidgetItem *item1 = new QTableWidgetItem(QString(name.toStdString().c_str()));
        item1->setBackground(QBrush(QColor(Qt::lightGray)));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        m_sceneListTable->setItem(i, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(obj["icon_path"].toString());
        item2->setBackground(QBrush(QColor(Qt::lightGray)));
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        m_sceneListTable->setItem(i, 2, item2);

        QPushButton *button = new QPushButton(tr("一键执行"));
        button->setObjectName(obj["id"].toString());
        m_sceneListTable->setCellWidget(i, 3, button);

        connect(button, &QPushButton::clicked, this, &CSceneList::triggerSingalScene);
    }
}

void CSceneList::triggerSingalScene()
{
    QPushButton* pSenderObj = qobject_cast<QPushButton*>(sender());
    if (pSenderObj != nullptr)
    {
        m_requestSender->triggerSingalSceneRequest(pSenderObj->objectName());
    }
}
