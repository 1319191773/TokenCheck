#include "settingsdialog.h"
#include "appsettings.h"
#include "platform_registry.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QKeyEvent>

HotkeyButton::HotkeyButton(QWidget *parent) : QPushButton(parent)
{
    setMinimumWidth(140);
    setFocusPolicy(Qt::StrongFocus);
}

void HotkeyButton::setKeySequence(const QString &seq)
{
    m_sequence = seq;
    m_capturing = false;
    updateDisplay();
}

QString HotkeyButton::keySequence() const
{
    return m_sequence;
}

void HotkeyButton::updateDisplay()
{
    if (m_capturing) {
        setText(tr("Press keys..."));
        setStyleSheet("background-color: #5C4B00; border: 2px solid #F1C40F; border-radius: 6px; color: #F1C40F; font-weight: bold;");
    } else if (m_sequence.isEmpty()) {
        setText(tr("(None)"));
        setStyleSheet(QString());
    } else {
        setText(m_sequence);
        setStyleSheet("color: #2ECC71; font-weight: bold;");
    }
}

void HotkeyButton::keyPressEvent(QKeyEvent *event)
{
    if (!m_capturing) {
        QPushButton::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        m_capturing = false;
        updateDisplay();
        return;
    }
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        m_sequence.clear();
        m_capturing = false;
        updateDisplay();
        return;
    }

    Qt::KeyboardModifiers mods = event->modifiers();
    int key = event->key();
    if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt
        || key == Qt::Key_Meta)
        return;

    QStringList parts;
    if (mods & Qt::ControlModifier)
        parts << "Ctrl";
    if (mods & Qt::ShiftModifier)
        parts << "Shift";
    if (mods & Qt::AltModifier)
        parts << "Alt";
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        parts << QChar(key).toUpper();
    else
        parts << QKeySequence(key).toString();

    if (parts.size() >= 2) {
        m_sequence = parts.join("+");
        m_capturing = false;
        updateDisplay();
    }
}

void HotkeyButton::focusOutEvent(QFocusEvent *event)
{
    m_capturing = false;
    updateDisplay();
    QPushButton::focusOutEvent(event);
}

void HotkeyButton::mousePressEvent(QMouseEvent *event)
{
    if (!m_capturing) {
        m_capturing = true;
        updateDisplay();
        setFocus();
    }
    QPushButton::mousePressEvent(event);
}

QuickAddDialog::QuickAddDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Quick Add Account"));
    setMinimumWidth(400);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    auto *form = new QFormLayout();
    form->setSpacing(10);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(tr("Account name"));
    form->addRow(tr("Name:"), m_nameEdit);

    PlatformRegistry::init();
    m_typeCombo = new QComboBox();
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
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

PlatformConfig QuickAddDialog::getConfig() const
{
    PlatformConfig pc;
    pc.name = m_nameEdit->text().trimmed();
    pc.platformType = m_typeCombo->currentData().toString();
    pc.authToken = m_tokenEdit->text().trimmed();
    pc.baseUrl = PlatformRegistry::instance().defaultBaseUrl(pc.platformType);
    pc.apiPrefix = PlatformRegistry::instance().defaultApiPrefix(pc.platformType);
    pc.enabled = true;
    return pc;
}

AccountEditDialog::AccountEditDialog(const PlatformConfig &config, QWidget *parent)
    : QDialog(parent)
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
    m_typeCombo = new QComboBox();
    for (const auto &t : PlatformRegistry::instance().allTypes())
        m_typeCombo->addItem(PlatformRegistry::instance().displayName(t), t);
    int typeIdx = m_typeCombo->findData(config.platformType);
    if (typeIdx >= 0)
        m_typeCombo->setCurrentIndex(typeIdx);
    form->addRow(tr("Platform:"), m_typeCombo);

    m_urlEdit = new QLineEdit(config.baseUrl);
    form->addRow(tr("Base URL:"), m_urlEdit);

    auto *tokenRow = new QHBoxLayout();
    m_tokenEdit = new QLineEdit(config.authToken);
    m_tokenEdit->setEchoMode(QLineEdit::Password);
    tokenRow->addWidget(m_tokenEdit);
    m_tokenToggle = new QPushButton(QString::fromUtf8("\xF0\x9F\x91\x81"));
    m_tokenToggle->setFixedSize(32, 32);
    m_tokenToggle->setFocusPolicy(Qt::NoFocus);
    tokenRow->addWidget(m_tokenToggle);
    form->addRow(tr("Auth Token:"), tokenRow);

    m_prefixEdit = new QLineEdit(config.apiPrefix);
    form->addRow(tr("API Prefix:"), m_prefixEdit);

    m_enabledCheck = new QCheckBox(tr("Enabled"));
    m_enabledCheck->setChecked(config.enabled);
    form->addRow(QString(), m_enabledCheck);

    mainLayout->addLayout(form);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_typeCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
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
        m_tokenToggle->setText(QString::fromUtf8("\xF0\x9F\x94\x92"));
    } else {
        m_tokenEdit->setEchoMode(QLineEdit::Password);
        m_tokenToggle->setText(QString::fromUtf8("\xF0\x9F\x91\x81"));
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
