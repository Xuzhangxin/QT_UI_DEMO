#include "mainwindow.h"


static QTextEdit *s_logShower = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1200, 900);
    setMinimumSize(1200, 900);
    setMaximumSize(1200, 900);
    QWidget *centralWidget = new QWidget(this);

    m_tcpClientPtr = QSharedPointer<QTcpSocket>(new QTcpSocket());
    m_requestSender = QSharedPointer<CRequestSender>(new CRequestSender(m_tcpClientPtr));

    connect(m_tcpClientPtr.get(), &QIODevice::readyRead, this, &MainWindow::dataParse);

    m_logShower = new QTextEdit(centralWidget);
    m_loginWidget = new CLoginWidget(m_tcpClientPtr, centralWidget);
    m_functionSwitch = new CFunctionSwitch(centralWidget);
    m_singleFunctionPage = new QStackedWidget(centralWidget);

    m_subDevManagerWidget = getSubDevManagerWidget();
    m_voiceUpLoad = new CVoiceUpload(m_requestSender);
    connect(this, &MainWindow::recvServerData, m_voiceUpLoad, &CVoiceUpload::serverDataParse);
    m_sceneList = new CSceneList(m_requestSender);
    connect(this, &MainWindow::recvServerData, m_sceneList, &CSceneList::serverDataParse);
    m_roomList = new CRoomList(m_requestSender);
    connect(this, &MainWindow::recvServerData, m_roomList, &CRoomList::serverDataParse);

    m_singleFunctionPage->addWidget(m_subDevManagerWidget);
    m_singleFunctionPage->addWidget(m_voiceUpLoad);
    m_singleFunctionPage->addWidget(m_sceneList);
    m_singleFunctionPage->addWidget(m_roomList);

    m_singleFunctionPage->setCurrentIndex(0);

    connect(m_functionSwitch, &CFunctionSwitch::functionItemChanged, this, &MainWindow::functionItemChanged);

    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 5);

    mainLayout->addWidget(m_loginWidget, 0, 0, 1, 6);
    mainLayout->addWidget(m_functionSwitch, 1, 0, 5, 1);
    mainLayout->addWidget(m_singleFunctionPage, 1, 1, 5, 5);
    mainLayout->addWidget(m_logShower, 6, 0, 4, 6);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    qInstallMessageHandler(logOutput);
    s_logShower = m_logShower;
}

MainWindow::~MainWindow()
{
}

void MainWindow::dataParse()
{
    while (m_tcpClientPtr->canReadLine())
    {
        QByteArray response = m_tcpClientPtr->readLine();
        qDebug() << "response data : " << response;
        QJsonParseError jsonParseError;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(QString(response).toUtf8(), &jsonParseError);
        //检查json是否有错误
        if (jsonParseError.error != QJsonParseError::NoError)
        {
            qDebug() << "response data parse failed";
            return;
        }

        if (jsonDocument.isObject() && jsonDocument.object().contains("protocol"))
        {
            int protocol = jsonDocument.object().take("protocol").toVariant().toInt();
            emit recvServerData(protocol, jsonDocument.object());
        }
    }

}

void MainWindow::functionItemChanged(const QString &functionItemName)
{
    qDebug() << functionItemName;
    if (functionItemName == tr("子设备管理"))
    {
        m_singleFunctionPage->setCurrentWidget(m_subDevManagerWidget);
    }
    else if (functionItemName == tr("场景管理"))
    {
        m_singleFunctionPage->setCurrentWidget(m_sceneList);
    }
    else if (functionItemName == tr("房间管理"))
    {
        m_singleFunctionPage->setCurrentWidget(m_roomList);
    }
    else if (functionItemName == tr("音频上传"))
    {
        m_singleFunctionPage->setCurrentWidget(m_voiceUpLoad);
    }
}

void MainWindow::logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    (void)context;
    QString text;
    text.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " ");
    switch((int)type)
    {
        case QtDebugMsg:
            text.append("Debug:");
            break;

        case QtWarningMsg:
            text.append("Warning:");
            break;

        case QtCriticalMsg:
            text.append("Critical:");
            break;

        case QtFatalMsg:
            text.append("Fatal:");
    }

    text.append(msg);
    s_logShower->append(text);
    s_logShower->moveCursor(QTextCursor::End);
}

QWidget *MainWindow::getSubDevManagerWidget()
{
    QWidget *subDevManagerWidget = new QWidget();
    m_subDevList = new CDeviceList(m_requestSender, subDevManagerWidget);
    m_dpManager = new CDpManager(m_requestSender, subDevManagerWidget);

    QHBoxLayout *subDevManagerLayout = new QHBoxLayout();
    subDevManagerLayout->addWidget(m_subDevList);
    subDevManagerLayout->addWidget(m_dpManager);
    subDevManagerLayout->setStretchFactor(m_subDevList, 2);
    subDevManagerLayout->setStretchFactor(m_dpManager, 5);
    subDevManagerWidget->setLayout(subDevManagerLayout);

    connect(this, &MainWindow::recvServerData, m_subDevList, &CDeviceList::serverDataParse);
    connect(this, &MainWindow::recvServerData, m_dpManager, &CDpManager::serverDataParse);

    setObjectName(QString::fromUtf8("subDevManagerWidget"));
    return subDevManagerWidget;
}
