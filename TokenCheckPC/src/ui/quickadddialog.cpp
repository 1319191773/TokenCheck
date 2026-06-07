#include "quickadddialog.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "component/noscrollcombobox.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

QuickAddDialog::QuickAddDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Quick Add Account"));
    setMinimumWidth(400);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    auto *form = new QFormLayout();
    form->setSpacing(10);
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Default to platform name"));
    form->addRow(tr("Name:"), m_nameEdit);
    PlatformRegistry::init();
    m_typeCombo = new NoScrollComboBox();
    for (const auto &t : PlatformRegistry::instance().allTypes())
        m_typeCombo->addItem(PlatformRegistry::instance().displayName(t), t);
    form->addRow(tr("Platform:"), m_typeCombo);
    m_tokenEdit = new QLineEdit();
    m_tokenEdit->setEchoMode(QLineEdit::Password);
    m_tokenEdit->setPlaceholderText(tr("API Token"));
    form->addRow(tr("Token:"), m_tokenEdit);
    mainLayout->addLayout(form);
    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, this, [this]() {
        if (m_tokenEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Token cannot be empty."));
            return;
        }
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

PlatformConfig QuickAddDialog::getConfig() const
{
    PlatformConfig pc;
    pc.name = m_nameEdit->text().trimmed();
    pc.platformType = m_typeCombo->currentData().toString();
    if (pc.name.isEmpty())
        pc.name = PlatformRegistry::instance().displayName(pc.platformType);
    pc.authToken = m_tokenEdit->text().trimmed();
    pc.baseUrl = PlatformRegistry::instance().defaultBaseUrl(pc.platformType);
    pc.apiPrefix = PlatformRegistry::instance().defaultApiPrefix(pc.platformType);
    pc.enabled = true;
    return pc;
}
