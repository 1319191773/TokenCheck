#include "general_tab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "../component/cardgroup.h"
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QLineEdit>
#include "../../core/appsettings.h"
#include "../component/moderncombobox.h"
#include "../component/modernspinbox.h"
#include "../component/hotkeybutton.h"
#include "../component/toggleswitch.h"
#include "../theme.h"

GeneralTab::GeneralTab(QWidget *parent) : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto mkLbl = [](const QString &text) { auto *l = new QLabel(text); l->setFixedWidth(130); return l; };
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *page = new QWidget();
    auto *mainForm = new QVBoxLayout(page);
    mainForm->setSpacing(16);
    mainForm->setContentsMargins(16, 16, 16, 16);

    auto *themeGroup = new CardGroup(tr("Theme && Language"));
    auto *themeForm = new QFormLayout();
    themeForm->setSpacing(12);
    themeForm->setContentsMargins(0, 4, 0, 0);
    themeForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_themeCombo = new ModernComboBox();
    m_themeCombo->addItem(tr("Dark"), 0);
    m_themeCombo->addItem(tr("Light"), 1);
    int tidx = m_themeCombo->findData(static_cast<int>(AppSettings::instance().themeId()));
    if (tidx >= 0) m_themeCombo->setCurrentIndex(tidx);
    m_themeCombo->setMinimumWidth(160);
    themeForm->addRow(mkLbl(tr("Color Theme:")), m_themeCombo);

    m_langCombo = new ModernComboBox();
    m_langCombo->addItem(tr("System Default"), QString());
    m_langCombo->addItem("English", "en");
    m_langCombo->addItem(QString::fromUtf8("\u7B80\u4F53\u4E2D\u6587"), "zh_CN");
    QString curLang = AppSettings::instance().language();
    int lidx = m_langCombo->findData(curLang);
    if (lidx >= 0) m_langCombo->setCurrentIndex(lidx);
    m_langCombo->setMinimumWidth(160);
    themeForm->addRow(mkLbl(tr("Language:")), m_langCombo);

    auto *langHint = new QLabel(tr("Language change takes effect after restart."));
    langHint->setProperty("class", "dim");
    langHint->setStyleSheet("font-size: 11px;");
    themeForm->addRow(QString(), langHint);

    themeGroup->addLayout(themeForm);
    mainForm->addWidget(themeGroup);

    auto *refreshGroup = new CardGroup(tr("Refresh && Startup"));
    auto *refreshForm = new QFormLayout();
    refreshForm->setSpacing(12);
    refreshForm->setContentsMargins(0, 4, 0, 0);
    refreshForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_intervalSpin = new ModernSpinBox();
    m_intervalSpin->setRange(1, 120);
    m_intervalSpin->setValue(AppSettings::instance().autoRefreshInterval());
    m_intervalSpin->setSuffix(" min");
    m_intervalSpin->setMinimumWidth(100);
    refreshForm->addRow(mkLbl(tr("Interval:")), m_intervalSpin);

    m_notifySpin = new ModernSpinBox();
    m_notifySpin->setRange(1, 50);
    m_notifySpin->setValue(AppSettings::instance().notifyThreshold());
    m_notifySpin->setSuffix(" %");
    m_notifySpin->setMinimumWidth(100);
    refreshForm->addRow(mkLbl(tr("Notify below:")), m_notifySpin);

    m_autoStartCheck = new ToggleSwitch();
    refreshForm->addRow(mkLbl(tr("Auto start on login:")), m_autoStartCheck);

    refreshGroup->addLayout(refreshForm);
    mainForm->addWidget(refreshGroup);

    auto *hotkeyGroup = new CardGroup(tr("Hotkeys"));
    auto *hotkeyForm = new QFormLayout();
    hotkeyForm->setSpacing(12);
    hotkeyForm->setContentsMargins(0, 4, 0, 0);
    hotkeyForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *toggleRow = new QHBoxLayout();
    toggleRow->setSpacing(12);
    m_toggleHotkeyBtn = new HotkeyButton();
    m_toggleHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyToggle());
    toggleRow->addWidget(m_toggleHotkeyBtn);
    auto *clearToggle = new QPushButton(tr("Clear"));
    toggleRow->addWidget(clearToggle);
    toggleRow->addStretch();
    connect(clearToggle, &QPushButton::clicked, this, [this]() { m_toggleHotkeyBtn->setKeySequence(QString()); });
    hotkeyForm->addRow(mkLbl(tr("Toggle ball:")), toggleRow);

    auto *refreshRow = new QHBoxLayout();
    refreshRow->setSpacing(12);
    m_refreshHotkeyBtn = new HotkeyButton();
    m_refreshHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyRefresh());
    refreshRow->addWidget(m_refreshHotkeyBtn);
    auto *clearRefresh = new QPushButton(tr("Clear"));
    refreshRow->addWidget(clearRefresh);
    refreshRow->addStretch();
    connect(clearRefresh, &QPushButton::clicked, this, [this]() { m_refreshHotkeyBtn->setKeySequence(QString()); });
    hotkeyForm->addRow(mkLbl(tr("Refresh:")), refreshRow);

    hotkeyGroup->addLayout(hotkeyForm);
    mainForm->addWidget(hotkeyGroup);

    auto *proxyGroup = new CardGroup(tr("Proxy"));
    auto *proxyForm = new QFormLayout();
    proxyForm->setSpacing(12);
    proxyForm->setContentsMargins(0, 4, 0, 0);
    proxyForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_proxyTypeCombo = new ModernComboBox();
    m_proxyTypeCombo->addItem(tr("No Proxy"), 0);
    m_proxyTypeCombo->addItem("HTTP", 1);
    m_proxyTypeCombo->addItem("SOCKS5", 2);
    int pidx = m_proxyTypeCombo->findData(AppSettings::instance().proxyType());
    if (pidx >= 0) m_proxyTypeCombo->setCurrentIndex(pidx);
    m_proxyTypeCombo->setMinimumWidth(100);
    proxyForm->addRow(mkLbl(tr("Type:")), m_proxyTypeCombo);

    m_proxyHostEdit = new QLineEdit(AppSettings::instance().proxyHost());
    m_proxyHostEdit->setMinimumWidth(160);
    proxyForm->addRow(mkLbl(tr("Host:")), m_proxyHostEdit);

    m_proxyPortSpin = new ModernSpinBox();
    m_proxyPortSpin->setRange(0, 65535);
    m_proxyPortSpin->setValue(AppSettings::instance().proxyPort());
    m_proxyPortSpin->setMinimumWidth(100);
    proxyForm->addRow(mkLbl(tr("Port:")), m_proxyPortSpin);

    proxyGroup->addLayout(proxyForm);
    mainForm->addWidget(proxyGroup);
    mainForm->addStretch();

    scroll->setWidget(page);
    mainLayout->addWidget(scroll);
}

void GeneralTab::reload()
{
    const AppSettings &s = AppSettings::instance();

    m_themeCombo->blockSignals(true);
    m_langCombo->blockSignals(true);
    m_intervalSpin->blockSignals(true);
    m_autoStartCheck->blockSignals(true);
    m_notifySpin->blockSignals(true);
    m_toggleHotkeyBtn->blockSignals(true);
    m_refreshHotkeyBtn->blockSignals(true);
    m_proxyTypeCombo->blockSignals(true);
    m_proxyHostEdit->blockSignals(true);
    m_proxyPortSpin->blockSignals(true);

    int tidx = m_themeCombo->findData(static_cast<int>(s.themeId()));
    if (tidx >= 0) m_themeCombo->setCurrentIndex(tidx);
    int lidx = m_langCombo->findData(s.language());
    if (lidx >= 0) m_langCombo->setCurrentIndex(lidx);
    m_intervalSpin->setValue(s.autoRefreshInterval());
    m_autoStartCheck->setChecked(s.autoStart());
    m_notifySpin->setValue(s.notifyThreshold());
    m_toggleHotkeyBtn->setKeySequence(s.hotkeyToggle());
    m_refreshHotkeyBtn->setKeySequence(s.hotkeyRefresh());
    int pidx = m_proxyTypeCombo->findData(s.proxyType());
    if (pidx >= 0) m_proxyTypeCombo->setCurrentIndex(pidx);
    m_proxyHostEdit->setText(s.proxyHost());
    m_proxyPortSpin->setValue(s.proxyPort());

    m_themeCombo->blockSignals(false);
    m_langCombo->blockSignals(false);
    m_intervalSpin->blockSignals(false);
    m_autoStartCheck->blockSignals(false);
    m_notifySpin->blockSignals(false);
    m_toggleHotkeyBtn->blockSignals(false);
    m_refreshHotkeyBtn->blockSignals(false);
    m_proxyTypeCombo->blockSignals(false);
    m_proxyHostEdit->blockSignals(false);
    m_proxyPortSpin->blockSignals(false);
}

void GeneralTab::applySettings()
{
    AppSettings &s = AppSettings::instance();

    s.setAutoRefreshInterval(m_intervalSpin->value());
    s.setAutoStart(m_autoStartCheck->isChecked());
    s.setNotifyThreshold(m_notifySpin->value());
    s.setHotkeyToggle(m_toggleHotkeyBtn->keySequence());
    s.setHotkeyRefresh(m_refreshHotkeyBtn->keySequence());
    s.setProxyType(m_proxyTypeCombo->currentData().toInt());
    s.setProxyHost(m_proxyHostEdit->text().trimmed());
    s.setProxyPort(m_proxyPortSpin->value());
    s.setThemeId(m_themeCombo->currentData().toInt() == 1 ? ThemeId::Light : ThemeId::Dark);
    s.setLanguage(m_langCombo->currentData().toString());
}
