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
