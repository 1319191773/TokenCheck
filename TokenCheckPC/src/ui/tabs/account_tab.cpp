#include "account_tab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QFrame>
#include <QLabel>
#include "../component/toast.h"
#include "../quickadddialog.h"
#include "../accounteditdialog.h"
#include "../../core/appsettings.h"

AccountTab::AccountTab(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(8);

    m_accountList = new QListWidget();
    m_accountList->setObjectName("accountList");
    m_accountList->setMinimumHeight(240);
    layout->addWidget(m_accountList, 1);

    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);
    auto *addBtn = new QPushButton(tr("Quick Add"));
    addBtn->setProperty("class", "primary");
    auto *editBtn = new QPushButton(tr("Edit"));
    editBtn->setProperty("class", "ghost");
    auto *removeBtn = new QPushButton(tr("Remove"));
    removeBtn->setProperty("class", "danger");

    btnRow->addWidget(addBtn);
    btnRow->addWidget(editBtn);
    btnRow->addWidget(removeBtn);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    connect(addBtn, &QPushButton::clicked, this, &AccountTab::onQuickAdd);
    connect(editBtn, &QPushButton::clicked, this, &AccountTab::onEditAccount);
    connect(removeBtn, &QPushButton::clicked, this, &AccountTab::onRemoveAccount);
    connect(m_accountList, &QListWidget::itemDoubleClicked, this, &AccountTab::onEditAccount);

    refreshAccountList();
}

void AccountTab::reload()
{
    refreshAccountList();
}

void AccountTab::refreshAccountList()
{
    m_accountList->clear();
    auto platforms = AppSettings::instance().allPlatforms();
    for (int i = 0; i < platforms.size(); i++) {
        const auto &p = platforms[i];
        
        auto *item = new QListWidgetItem();
        item->setData(Qt::UserRole, i);
        item->setSizeHint(QSize(0, 84));
        m_accountList->addItem(item);

        auto *card = new QFrame();
        card->setProperty("class", "account-card");
        
        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);
        cardLayout->setSpacing(12);

        auto *statusDot = new QLabel(p.enabled ? QString::fromUtf8("\u25CF") : QString::fromUtf8("\u25CB"));
        statusDot->setStyleSheet(QString("font-size: 20px; color: %1;").arg(p.enabled ? "#10B981" : "#6B7280"));
        cardLayout->addWidget(statusDot);

        auto *textLayout = new QVBoxLayout();
        textLayout->setSpacing(4);
        
        auto *nameLabel = new QLabel(p.name);
        nameLabel->setStyleSheet("font-size: 15px; font-weight: bold;");
        if (!p.enabled) nameLabel->setProperty("class", "dim");
        
        auto *typeLabel = new QLabel(QString("%1 %2").arg(p.platformType).arg(p.enabled ? "" : tr("(Disabled)")));
        typeLabel->setProperty("class", "dim");
        
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(typeLabel);
        textLayout->setAlignment(Qt::AlignVCenter);
        
        cardLayout->addLayout(textLayout, 1);

        m_accountList->setItemWidget(item, card);
    }
}

void AccountTab::onQuickAdd()
{
    QuickAddDialog dlg(this);
    while (dlg.exec() == QDialog::Accepted) {
        PlatformConfig pc = dlg.getConfig();
        auto platforms = AppSettings::instance().allPlatforms();
        bool dup = false;
        for (const auto &p : platforms) {
            if (p.name == pc.name) {
                Toast::show(this, tr("Account \"%1\" already exists.").arg(pc.name), true);
                dup = true;
                break;
            }
        }
        if (dup)
            continue;
        AppSettings::instance().addPlatform(pc);
        refreshAccountList();
        Toast::show(this, tr("Account added!"), false);
        break;
    }
}

void AccountTab::onEditAccount()
{
    auto *current = m_accountList->currentItem();
    if (!current) return;
    int idx = current->data(Qt::UserRole).toInt();
    PlatformConfig config = AppSettings::instance().platformAt(idx);
    AccountEditDialog dlg(config, this);
    if (dlg.exec() == QDialog::Accepted) {
        AppSettings::instance().setPlatform(idx, dlg.getConfig());
        refreshAccountList();
        Toast::show(this, tr("Account saved!"), false);
    }
}

void AccountTab::onRemoveAccount()
{
    auto *current = m_accountList->currentItem();
    if (!current) return;
    int idx = current->data(Qt::UserRole).toInt();
    PlatformConfig config = AppSettings::instance().platformAt(idx);
    auto reply = QMessageBox::question(this, tr("Remove Account"),
                                       tr("Remove \"%1\"?").arg(config.name),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        AppSettings::instance().removePlatform(idx);
        refreshAccountList();
        Toast::show(this, tr("Account removed!"), false);
    }
}
