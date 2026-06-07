#include "accounteditdialog.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "component/noscrollcombobox.h"
#include "component/toggleswitch.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>

AccountEditDialog::AccountEditDialog(const PlatformConfig &config, QWidget *parent) : QDialog(parent)
{
    setWindowTitle(config.name.isEmpty() ? tr("Add Account") : tr("Edit Account"));
    setMinimumWidth(460);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    auto *form = new QFormLayout();
    form->setSpacing(10);
    m_nameEdit = new QLineEdit(config.name);
    form->addRow(tr("Name:"), m_nameEdit);
    PlatformRegistry::init();
    m_typeCombo = new NoScrollComboBox();
    for (const auto &t : PlatformRegistry::instance().allTypes())
        m_typeCombo->addItem(PlatformRegistry::instance().displayName(t), t);
    int typeIdx = m_typeCombo->findData(config.platformType);
    if (typeIdx >= 0) m_typeCombo->setCurrentIndex(typeIdx);
    form->addRow(tr("Platform:"), m_typeCombo);
    m_urlEdit = new QLineEdit(config.baseUrl);
    form->addRow(tr("Base URL:"), m_urlEdit);
    auto *tokenRow = new QHBoxLayout();
    m_tokenEdit = new QLineEdit(config.authToken);
    m_tokenEdit->setEchoMode(QLineEdit::Password);
    tokenRow->addWidget(m_tokenEdit);
    m_tokenToggle = new QPushButton(QString::fromUtf8("\U0001F441"));
    m_tokenToggle->setFixedSize(36, 36);
    m_tokenToggle->setStyleSheet("padding: 0;");
    m_tokenToggle->setFocusPolicy(Qt::NoFocus);
    tokenRow->addWidget(m_tokenToggle);
    form->addRow(tr("Auth Token:"), tokenRow);
    m_prefixEdit = new QLineEdit(config.apiPrefix);
    form->addRow(tr("API Prefix:"), m_prefixEdit);
    m_enabledCheck = new ToggleSwitch(this);
    m_enabledCheck->setChecked(config.enabled);
    form->addRow(tr("Enabled:"), m_enabledCheck);
    mainLayout->addLayout(form);
    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_typeCombo, static_cast<void(NoScrollComboBox::*)(int)>(&NoScrollComboBox::currentIndexChanged),
            this, &AccountEditDialog::onTypeChanged);
    connect(m_tokenToggle, &QPushButton::clicked, this, &AccountEditDialog::toggleTokenVisibility);
}

void AccountEditDialog::onTypeChanged(int index)
{
    QString type = m_typeCombo->itemData(index).toString();
    m_urlEdit->setText(PlatformRegistry::instance().defaultBaseUrl(type));
    m_prefixEdit->setText(PlatformRegistry::instance().defaultApiPrefix(type));
}

void AccountEditDialog::toggleTokenVisibility()
{
    if (m_tokenEdit->echoMode() == QLineEdit::Password) {
        m_tokenEdit->setEchoMode(QLineEdit::Normal);
        m_tokenToggle->setText(QString::fromUtf8("\U0001F512"));
    } else {
        m_tokenEdit->setEchoMode(QLineEdit::Password);
        m_tokenToggle->setText(QString::fromUtf8("\U0001F441"));
    }
}

PlatformConfig AccountEditDialog::getConfig() const
{
    PlatformConfig pc;
    pc.name = m_nameEdit->text().trimmed();
    pc.platformType = m_typeCombo->currentData().toString();
    pc.baseUrl = m_urlEdit->text().trimmed();
    pc.authToken = m_tokenEdit->text().trimmed();
    pc.apiPrefix = m_prefixEdit->text().trimmed();
    pc.enabled = m_enabledCheck->isChecked();
    return pc;
}
