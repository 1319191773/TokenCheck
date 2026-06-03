#include "mainwindow.h"
#include "settingsdialog.h"
#include "appsettings.h"
#include "datamanager.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QHeaderView>
#include <QDateTime>
#include <QFrame>
#include <QTabWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QColorDialog>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_dm(&DataManager::instance())
{
    setObjectName("MainWindowRoot");
    setWindowTitle(tr("GLM Usage"));
    setMinimumSize(680, 720);
    setupUI();

    connect(m_dm, &DataManager::accountChanged, this, &MainWindow::onAccountChanged);
    connect(m_dm, &DataManager::allDataUpdated, this, &MainWindow::onAllDataUpdated);
}

void MainWindow::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 12, 8);
    mainLayout->setSpacing(0);

    m_tabs = new QTabWidget();
    m_tabs->addTab(createDetailTab(), tr("Detail"));
    m_tabs->addTab(createAccountsTab(), tr("Accounts"));
    m_tabs->addTab(createAppearanceTab(), tr("Appearance"));
    m_tabs->addTab(createGeneralTab(), tr("General"));
    mainLayout->addWidget(m_tabs);

    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

QWidget *MainWindow::createDetailTab()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(10);

    auto *topBar = new QHBoxLayout();
    topBar->setSpacing(10);

    auto *accountLabel = new QLabel(tr("Account:"));
    accountLabel->setStyleSheet("font-weight: bold;");
    topBar->addWidget(accountLabel);

    m_accountCombo = new QComboBox();
    m_accountCombo->setMinimumWidth(220);
    topBar->addWidget(m_accountCombo);
    topBar->addStretch();

    auto *refreshBtn = new QPushButton(tr("Refresh"));
    refreshBtn->setProperty("class", "primary");
    topBar->addWidget(refreshBtn);
    layout->addLayout(topBar);

    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshRequested);
    connect(m_accountCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::onComboBoxChanged);

    m_statusLabel = new QLabel();
    m_timestampLabel = new QLabel();
    m_timestampLabel->setProperty("class", "dim");
    m_timestampLabel->setStyleSheet("font-size: 11px;");

    auto *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(m_statusLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_timestampLabel);
    layout->addLayout(headerLayout);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *container = new QWidget();
    container->setStyleSheet("background-color: transparent;");
    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(14);
    containerLayout->setContentsMargins(0, 0, 0, 0);

    containerLayout->addWidget(createSummarySection());
    containerLayout->addWidget(createQuotaSection());
    containerLayout->addWidget(createModelSection());
    containerLayout->addWidget(createToolSection());

    containerLayout->addStretch();
    scroll->setWidget(container);
    layout->addWidget(scroll);

    return page;
}

QWidget *MainWindow::createAccountsTab()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(8);

    m_accountList = new QListWidget();
    m_accountList->setMinimumHeight(160);
    layout->addWidget(m_accountList, 1);

    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);
    auto *addBtn = new QPushButton(tr("Quick Add"));
    addBtn->setProperty("class", "primary");
    auto *editBtn = new QPushButton(tr("Edit"));
    auto *removeBtn = new QPushButton(tr("Remove"));
    removeBtn->setProperty("class", "danger");
    btnRow->addWidget(addBtn);
    btnRow->addWidget(editBtn);
    btnRow->addWidget(removeBtn);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onQuickAdd);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditAccount);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveAccount);
    connect(m_accountList, &QListWidget::itemDoubleClicked, this, &MainWindow::onEditAccount);

    refreshAccountList();
    return page;
}

QWidget *MainWindow::createAppearanceTab()
{
    auto *page = new QWidget();
    auto *form = new QFormLayout(page);
    form->setSpacing(12);
    form->setContentsMargins(16, 16, 16, 16);

    auto *themeGroup = new QGroupBox(tr("Theme"));
    auto *themeForm = new QFormLayout(themeGroup);
    themeForm->setSpacing(10);

    m_themeCombo = new QComboBox();
    m_themeCombo->addItem(tr("Dark"), 0);
    m_themeCombo->addItem(tr("Light"), 1);
    int tidx = m_themeCombo->findData(static_cast<int>(AppSettings::instance().themeId()));
    if (tidx >= 0)
        m_themeCombo->setCurrentIndex(tidx);
    themeForm->addRow(tr("Color Theme:"), m_themeCombo);
    connect(m_themeCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::markDirty);

    form->addRow(themeGroup);

    auto *ballGroup = new QGroupBox(tr("Floating Ball"));
    auto *ballForm = new QFormLayout(ballGroup);
    ballForm->setSpacing(10);

    m_ballSizeSpin = new QSpinBox();
    m_ballSizeSpin->setRange(60, 150);
    m_ballSizeSpin->setValue(AppSettings::instance().ballSize());
    m_ballSizeSpin->setSuffix(" px");
    ballForm->addRow(tr("Ball Size:"), m_ballSizeSpin);
    connect(m_ballSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    m_timeFontSpin = new QSpinBox();
    m_timeFontSpin->setRange(0, 30);
    m_timeFontSpin->setValue(AppSettings::instance().timeFontSize());
    m_timeFontSpin->setSpecialValueText("Auto");
    m_timeFontSpin->setSuffix(" px");
    ballForm->addRow(tr("Time Font Size:"), m_timeFontSpin);
    connect(m_timeFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    m_chosenTimeColor = AppSettings::instance().timeColor();
    m_timeColorBtn = new QPushButton(m_chosenTimeColor.isValid() ? m_chosenTimeColor.name() : tr("Default"));
    if (m_chosenTimeColor.isValid())
        m_timeColorBtn->setStyleSheet(
            QString("background-color: %1; color: #000; border-radius: 6px; min-height: 24px;").arg(m_chosenTimeColor.name()));
    connect(m_timeColorBtn, &QPushButton::clicked, this, [this]() {
        QColor initial = m_chosenTimeColor.isValid() ? m_chosenTimeColor : Qt::white;
        QColor c = QColorDialog::getColor(initial, this, tr("Time Text Color"));
        if (c.isValid()) {
            m_chosenTimeColor = c;
            m_timeColorBtn->setText(c.name());
            m_timeColorBtn->setStyleSheet(
                QString("background-color: %1; color: #000; border-radius: 6px; min-height: 24px;").arg(c.name()));
            markDirty();
        }
    });
    ballForm->addRow(tr("Time Color:"), m_timeColorBtn);

    m_pctFontSpin = new QSpinBox();
    m_pctFontSpin->setRange(0, 30);
    m_pctFontSpin->setValue(AppSettings::instance().pctFontSize());
    m_pctFontSpin->setSpecialValueText("Auto");
    m_pctFontSpin->setSuffix(" px");
    ballForm->addRow(tr("% Font Size:"), m_pctFontSpin);
    connect(m_pctFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    m_chosenPctColor = AppSettings::instance().pctColor();
    m_pctColorBtn = new QPushButton(m_chosenPctColor.isValid() ? m_chosenPctColor.name() : tr("Default (White)"));
    if (m_chosenPctColor.isValid())
        m_pctColorBtn->setStyleSheet(
            QString("background-color: %1; color: #000; border-radius: 6px; min-height: 24px;").arg(m_chosenPctColor.name()));
    connect(m_pctColorBtn, &QPushButton::clicked, this, [this]() {
        QColor initial = m_chosenPctColor.isValid() ? m_chosenPctColor : Qt::white;
        QColor c = QColorDialog::getColor(initial, this, tr("Percent Text Color"));
        if (c.isValid()) {
            m_chosenPctColor = c;
            m_pctColorBtn->setText(c.name());
            m_pctColorBtn->setStyleSheet(
                QString("background-color: %1; color: #000; border-radius: 6px; min-height: 24px;").arg(c.name()));
            markDirty();
        }
    });
    ballForm->addRow(tr("% Color:"), m_pctColorBtn);

    form->addRow(ballGroup);

    auto *applyBtn = new QPushButton(tr("Apply"));
    applyBtn->setProperty("class", "primary");
    applyBtn->setFixedWidth(120);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applySettings);
    auto *applyRow = new QHBoxLayout();
    applyRow->addStretch();
    applyRow->addWidget(applyBtn);
    form->addRow(applyRow);

    return page;
}

QWidget *MainWindow::createGeneralTab()
{
    auto *page = new QWidget();
    auto *mainForm = new QVBoxLayout(page);
    mainForm->setSpacing(14);
    mainForm->setContentsMargins(16, 16, 16, 16);

    auto *langGroup = new QGroupBox(tr("Language"));
    auto *langForm = new QFormLayout(langGroup);
    langForm->setSpacing(10);

    m_langCombo = new QComboBox();
    m_langCombo->addItem(tr("System Default"), QString());
    m_langCombo->addItem("English", "en");
    m_langCombo->addItem(QString::fromUtf8("\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87"), "zh_CN");
    QString curLang = AppSettings::instance().language();
    int lidx = m_langCombo->findData(curLang);
    if (lidx >= 0)
        m_langCombo->setCurrentIndex(lidx);
    langForm->addRow(tr("Interface Language:"), m_langCombo);
    connect(m_langCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::markDirty);

    auto *langHint = new QLabel(tr("Language change takes effect after restart."));
    langHint->setProperty("class", "dim");
    langHint->setStyleSheet("font-size: 11px;");
    langForm->addRow(QString(), langHint);

    mainForm->addWidget(langGroup);

    auto *refreshGroup = new QGroupBox(tr("Refresh && Startup"));
    auto *refreshForm = new QFormLayout(refreshGroup);
    refreshForm->setSpacing(10);

    m_intervalSpin = new QSpinBox();
    m_intervalSpin->setRange(1, 120);
    m_intervalSpin->setValue(AppSettings::instance().autoRefreshInterval());
    m_intervalSpin->setSuffix(" min");
    refreshForm->addRow(tr("Refresh Interval:"), m_intervalSpin);
    connect(m_intervalSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    m_autoStartCheck = new QCheckBox(tr("Auto start on login"));
    m_autoStartCheck->setChecked(AppSettings::instance().autoStart());
    refreshForm->addRow(QString(), m_autoStartCheck);
    connect(m_autoStartCheck, &QCheckBox::toggled, this, &MainWindow::markDirty);

    m_notifySpin = new QSpinBox();
    m_notifySpin->setRange(1, 50);
    m_notifySpin->setValue(AppSettings::instance().notifyThreshold());
    m_notifySpin->setSuffix(" %");
    refreshForm->addRow(tr("Notify below:"), m_notifySpin);
    connect(m_notifySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    mainForm->addWidget(refreshGroup);

    auto *hotkeyGroup = new QGroupBox(tr("Hotkeys"));
    auto *hotkeyForm = new QFormLayout(hotkeyGroup);
    hotkeyForm->setSpacing(10);

    auto *toggleRow = new QHBoxLayout();
    toggleRow->setSpacing(8);
    m_toggleHotkeyBtn = new HotkeyButton();
    m_toggleHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyToggle());
    toggleRow->addWidget(m_toggleHotkeyBtn);
    auto *clearToggle = new QPushButton(tr("Clear"));
    clearToggle->setFixedWidth(60);
    toggleRow->addWidget(clearToggle);
    connect(clearToggle, &QPushButton::clicked, this, [this]() {
        m_toggleHotkeyBtn->setKeySequence(QString());
        markDirty();
    });
    hotkeyForm->addRow(tr("Toggle ball:"), toggleRow);

    auto *refreshRow = new QHBoxLayout();
    refreshRow->setSpacing(8);
    m_refreshHotkeyBtn = new HotkeyButton();
    m_refreshHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyRefresh());
    refreshRow->addWidget(m_refreshHotkeyBtn);
    auto *clearRefresh = new QPushButton(tr("Clear"));
    clearRefresh->setFixedWidth(60);
    refreshRow->addWidget(clearRefresh);
    connect(clearRefresh, &QPushButton::clicked, this, [this]() {
        m_refreshHotkeyBtn->setKeySequence(QString());
        markDirty();
    });
    hotkeyForm->addRow(tr("Refresh:"), refreshRow);

    mainForm->addWidget(hotkeyGroup);

    auto *proxyGroup = new QGroupBox(tr("Proxy"));
    auto *proxyForm = new QFormLayout(proxyGroup);
    proxyForm->setSpacing(10);

    m_proxyTypeCombo = new QComboBox();
    m_proxyTypeCombo->addItem(tr("No Proxy"), 0);
    m_proxyTypeCombo->addItem("HTTP", 1);
    m_proxyTypeCombo->addItem("SOCKS5", 2);
    int pidx = m_proxyTypeCombo->findData(AppSettings::instance().proxyType());
    if (pidx >= 0)
        m_proxyTypeCombo->setCurrentIndex(pidx);
    proxyForm->addRow(tr("Type:"), m_proxyTypeCombo);
    connect(m_proxyTypeCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::markDirty);

    m_proxyHostEdit = new QLineEdit(AppSettings::instance().proxyHost());
    proxyForm->addRow(tr("Host:"), m_proxyHostEdit);
    connect(m_proxyHostEdit, &QLineEdit::textChanged, this, &MainWindow::markDirty);

    m_proxyPortSpin = new QSpinBox();
    m_proxyPortSpin->setRange(0, 65535);
    m_proxyPortSpin->setValue(AppSettings::instance().proxyPort());
    proxyForm->addRow(tr("Port:"), m_proxyPortSpin);
    connect(m_proxyPortSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::markDirty);

    mainForm->addWidget(proxyGroup);

    auto *applyBtn = new QPushButton(tr("Apply"));
    applyBtn->setProperty("class", "primary");
    applyBtn->setFixedWidth(120);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applySettings);
    auto *applyRow = new QHBoxLayout();
    applyRow->addStretch();
    applyRow->addWidget(applyBtn);
    mainForm->addLayout(applyRow);

    mainForm->addStretch();

    return page;
}

void MainWindow::showTab(int index)
{
    if (index >= 0 && index < m_tabs->count())
        m_tabs->setCurrentIndex(index);
    show();
    raise();
    activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_settingsDirty) {
        auto reply = QMessageBox::question(this, tr("Unsaved Changes"),
                                           tr("Save settings before closing?"),
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
        if (reply == QMessageBox::Yes)
            applySettings();
        else
            reloadSettings();
    }
    event->accept();
}

void MainWindow::onTabChanged(int index)
{
    if (m_previousTab >= 1 && m_previousTab <= 3 && m_settingsDirty) {
        auto reply = QMessageBox::question(this, tr("Unsaved Changes"),
                                           tr("Save settings changes?"),
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            m_tabs->blockSignals(true);
            m_tabs->setCurrentIndex(m_previousTab);
            m_tabs->blockSignals(false);
            return;
        }
        if (reply == QMessageBox::Yes)
            applySettings();
        else
            reloadSettings();
    }
    m_previousTab = index;
}

bool MainWindow::checkSaveOnLeave()
{
    if (!m_settingsDirty)
        return true;
    auto reply = QMessageBox::question(this, tr("Unsaved Changes"),
                                       tr("Save settings changes?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (reply == QMessageBox::Cancel)
        return false;
    if (reply == QMessageBox::Yes)
        applySettings();
    else
        reloadSettings();
    return true;
}

void MainWindow::markDirty()
{
    m_settingsDirty = true;
}

void MainWindow::applySettings()
{
    AppSettings::instance().setAutoRefreshInterval(m_intervalSpin->value());
    AppSettings::instance().setBallSize(m_ballSizeSpin->value());
    AppSettings::instance().setTimeFontSize(m_timeFontSpin->value());
    AppSettings::instance().setTimeColor(m_chosenTimeColor);
    AppSettings::instance().setPctFontSize(m_pctFontSpin->value());
    AppSettings::instance().setPctColor(m_chosenPctColor);
    AppSettings::instance().setAutoStart(m_autoStartCheck->isChecked());
    AppSettings::instance().setNotifyThreshold(m_notifySpin->value());
    AppSettings::instance().setHotkeyToggle(m_toggleHotkeyBtn->keySequence());
    AppSettings::instance().setHotkeyRefresh(m_refreshHotkeyBtn->keySequence());
    AppSettings::instance().setProxyType(m_proxyTypeCombo->currentData().toInt());
    AppSettings::instance().setProxyHost(m_proxyHostEdit->text().trimmed());
    AppSettings::instance().setProxyPort(m_proxyPortSpin->value());
    AppSettings::instance().setThemeId(
        m_themeCombo->currentData().toInt() == 1 ? ThemeId::Light : ThemeId::Dark);
    AppSettings::instance().setLanguage(m_langCombo->currentData().toString());

    m_settingsDirty = false;
    emit settingsApplied();
}

void MainWindow::reloadSettings()
{
    m_themeCombo->blockSignals(true);
    m_ballSizeSpin->blockSignals(true);
    m_timeFontSpin->blockSignals(true);
    m_pctFontSpin->blockSignals(true);
    m_intervalSpin->blockSignals(true);
    m_autoStartCheck->blockSignals(true);
    m_notifySpin->blockSignals(true);
    m_toggleHotkeyBtn->blockSignals(true);
    m_refreshHotkeyBtn->blockSignals(true);
    m_proxyTypeCombo->blockSignals(true);
    m_proxyHostEdit->blockSignals(true);
    m_proxyPortSpin->blockSignals(true);
    m_langCombo->blockSignals(true);

    int tidx = m_themeCombo->findData(static_cast<int>(AppSettings::instance().themeId()));
    if (tidx >= 0) m_themeCombo->setCurrentIndex(tidx);
    m_ballSizeSpin->setValue(AppSettings::instance().ballSize());
    m_timeFontSpin->setValue(AppSettings::instance().timeFontSize());
    m_pctFontSpin->setValue(AppSettings::instance().pctFontSize());
    m_intervalSpin->setValue(AppSettings::instance().autoRefreshInterval());
    m_autoStartCheck->setChecked(AppSettings::instance().autoStart());
    m_notifySpin->setValue(AppSettings::instance().notifyThreshold());
    m_toggleHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyToggle());
    m_refreshHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyRefresh());
    int pidx = m_proxyTypeCombo->findData(AppSettings::instance().proxyType());
    if (pidx >= 0) m_proxyTypeCombo->setCurrentIndex(pidx);
    m_proxyHostEdit->setText(AppSettings::instance().proxyHost());
    m_proxyPortSpin->setValue(AppSettings::instance().proxyPort());
    int lidx = m_langCombo->findData(AppSettings::instance().language());
    if (lidx >= 0) m_langCombo->setCurrentIndex(lidx);

    m_chosenTimeColor = AppSettings::instance().timeColor();
    m_chosenPctColor = AppSettings::instance().pctColor();

    m_themeCombo->blockSignals(false);
    m_ballSizeSpin->blockSignals(false);
    m_timeFontSpin->blockSignals(false);
    m_pctFontSpin->blockSignals(false);
    m_intervalSpin->blockSignals(false);
    m_autoStartCheck->blockSignals(false);
    m_notifySpin->blockSignals(false);
    m_toggleHotkeyBtn->blockSignals(false);
    m_refreshHotkeyBtn->blockSignals(false);
    m_proxyTypeCombo->blockSignals(false);
    m_proxyHostEdit->blockSignals(false);
    m_proxyPortSpin->blockSignals(false);
    m_langCombo->blockSignals(false);

    m_settingsDirty = false;
}

QWidget *MainWindow::createQuotaSection()
{
    auto *group = new QGroupBox(tr("Quota"));
    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(8);

    m_tokenRow = new QWidget();
    auto *tokenLayout = new QHBoxLayout(m_tokenRow);
    tokenLayout->setContentsMargins(0, 0, 0, 0);
    tokenLayout->setSpacing(8);
    tokenLayout->addWidget(new QLabel("Token:"));
    m_tokenBar = new QProgressBar();
    m_tokenBar->setRange(0, 100);
    m_tokenBar->setFixedHeight(20);
    m_tokenBar->setTextVisible(true);
    tokenLayout->addWidget(m_tokenBar);
    m_tokenLabel = new QLabel("--");
    m_tokenLabel->setMinimumWidth(130);
    m_tokenLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tokenLayout->addWidget(m_tokenLabel);
    layout->addWidget(m_tokenRow);

    m_resetRow = new QWidget();
    auto *resetLayout = new QHBoxLayout(m_resetRow);
    resetLayout->setContentsMargins(0, 0, 0, 0);
    resetLayout->setSpacing(8);
    resetLayout->addWidget(new QLabel(tr("Reset:")));
    m_resetLabel = new QLabel("--");
    resetLayout->addWidget(m_resetLabel);
    resetLayout->addStretch();
    layout->addWidget(m_resetRow);

    m_mcpRow = new QWidget();
    auto *mcpLayout = new QHBoxLayout(m_mcpRow);
    mcpLayout->setContentsMargins(0, 0, 0, 0);
    mcpLayout->setSpacing(8);
    mcpLayout->addWidget(new QLabel("MCP:"));
    m_mcpBar = new QProgressBar();
    m_mcpBar->setRange(0, 100);
    m_mcpBar->setFixedHeight(20);
    m_mcpBar->setTextVisible(true);
    mcpLayout->addWidget(m_mcpBar);
    m_mcpLabel = new QLabel("--");
    m_mcpLabel->setMinimumWidth(130);
    m_mcpLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mcpLayout->addWidget(m_mcpLabel);
    layout->addWidget(m_mcpRow);

    m_mcpDetailRow = new QWidget();
    auto *mcpDetailLayout = new QHBoxLayout(m_mcpDetailRow);
    mcpDetailLayout->setContentsMargins(0, 0, 0, 0);
    mcpDetailLayout->setSpacing(8);
    mcpDetailLayout->addWidget(new QLabel(tr("Details:")));
    m_mcpDetailLabel = new QLabel("--");
    m_mcpDetailLabel->setWordWrap(true);
    mcpDetailLayout->addWidget(m_mcpDetailLabel);
    mcpDetailLayout->addStretch();
    layout->addWidget(m_mcpDetailRow);

    m_balanceRow = new QWidget();
    auto *balLayout = new QHBoxLayout(m_balanceRow);
    balLayout->setContentsMargins(0, 0, 0, 0);
    balLayout->setSpacing(8);
    balLayout->addWidget(new QLabel(tr("Balance:")));
    m_balanceLabel = new QLabel("--");
    m_balanceLabel->setWordWrap(true);
    balLayout->addWidget(m_balanceLabel);
    balLayout->addStretch();
    layout->addWidget(m_balanceRow);

    return group;
}

QWidget *MainWindow::createSummarySection()
{
    auto *group = new QGroupBox(tr("Summary"));
    auto *layout = new QHBoxLayout(group);
    layout->setSpacing(12);

    auto makeCard = [](const QString &label, QLabel *&valueLabel) -> QFrame * {
        auto *card = new QFrame();
        card->setProperty("class", "stat-card");
        card->setStyleSheet(
            "QFrame[class=\"stat-card\"] {"
            "  background-color: #2A2A3C;"
            "  border: 1px solid #45475A;"
            "  border-radius: 8px;"
            "}");
        auto *vlayout = new QVBoxLayout(card);
        vlayout->setContentsMargins(12, 10, 12, 10);
        vlayout->setSpacing(2);
        valueLabel = new QLabel("--");
        valueLabel->setProperty("class", "stat-value");
        valueLabel->setAlignment(Qt::AlignCenter);
        vlayout->addWidget(valueLabel);
        auto *lbl = new QLabel(label);
        lbl->setProperty("class", "stat-label");
        lbl->setAlignment(Qt::AlignCenter);
        vlayout->addWidget(lbl);
        return card;
    };

    layout->addWidget(makeCard(tr("Tokens"), m_totalTokensLabel));
    layout->addWidget(makeCard(tr("Requests"), m_totalRequestsLabel));
    layout->addWidget(makeCard(tr("Models"), m_modelCountLabel));
    layout->addWidget(makeCard(tr("Tools"), m_toolCountLabel));

    return group;
}

QWidget *MainWindow::createModelSection()
{
    auto *group = new QGroupBox(tr("Model Usage"));
    auto *layout = new QVBoxLayout(group);
    m_modelTable = new QTableWidget();
    m_modelTable->setColumnCount(6);
    m_modelTable->setHorizontalHeaderLabels(
        {tr("Model"), tr("Provider"), tr("Input"), tr("Output"), tr("Total"), tr("Requests")});
    m_modelTable->horizontalHeader()->setStretchLastSection(true);
    m_modelTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_modelTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_modelTable->setAlternatingRowColors(true);
    m_modelTable->verticalHeader()->setVisible(false);
    m_modelTable->setShowGrid(false);
    m_modelTable->verticalHeader()->setDefaultSectionSize(32);
    m_modelTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_modelTable);
    return group;
}

QWidget *MainWindow::createToolSection()
{
    auto *group = new QGroupBox(tr("Tool Usage"));
    auto *layout = new QVBoxLayout(group);
    m_toolTable = new QTableWidget();
    m_toolTable->setColumnCount(2);
    m_toolTable->setHorizontalHeaderLabels({tr("Tool"), tr("Calls")});
    m_toolTable->horizontalHeader()->setStretchLastSection(true);
    m_toolTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_toolTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_toolTable->setAlternatingRowColors(true);
    m_toolTable->verticalHeader()->setVisible(false);
    m_toolTable->setShowGrid(false);
    m_toolTable->verticalHeader()->setDefaultSectionSize(32);
    m_toolTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_toolTable);
    return group;
}

void MainWindow::syncWithDataManager()
{
    populateAccounts();
    QString cur = m_dm->currentAccount();
    if (!cur.isEmpty()) {
        UsageData d = m_dm->data(cur);
        displayData(d);
    } else {
        displayEmpty();
    }
}

void MainWindow::populateAccounts()
{
    m_accountCombo->blockSignals(true);
    QString cur = m_dm->currentAccount();
    m_accountCombo->clear();
    auto names = m_dm->accountNames();
    for (const auto &n : names)
        m_accountCombo->addItem(n);
    int idx = m_accountCombo->findText(cur);
    if (idx >= 0)
        m_accountCombo->setCurrentIndex(idx);
    m_accountCombo->blockSignals(false);
}

void MainWindow::onAccountChanged(const QString &name)
{
    populateAccounts();
    if (!name.isEmpty())
        displayData(m_dm->data(name));
}

void MainWindow::onComboBoxChanged(int index)
{
    if (index < 0 || index >= m_accountCombo->count())
        return;
    QString name = m_accountCombo->itemText(index);
    m_dm->setCurrentAccount(name);
}

void MainWindow::onAllDataUpdated()
{
    populateAccounts();
    QString cur = m_dm->currentAccount();
    if (!cur.isEmpty())
        displayData(m_dm->data(cur));
}

void MainWindow::refreshAccountList()
{
    m_accountList->clear();
    auto platforms = AppSettings::instance().allPlatforms();
    for (int i = 0; i < platforms.size(); i++) {
        const auto &p = platforms[i];

        QString dot = p.enabled
            ? QString::fromUtf8("\xe2\x97\x8f")
            : QString::fromUtf8("\xe2\x97\x8b");

        QString line1 = QString("%1 %2 [%3]").arg(dot, p.name, p.platformType);
        if (!p.enabled)
            line1 += " (" + tr("Disabled") + ")";

        auto *item = new QListWidgetItem();
        item->setData(Qt::UserRole, i);
        item->setText(line1);
        if (!p.enabled)
            item->setForeground(QColor(108, 112, 134));
        m_accountList->addItem(item);
    }
}

void MainWindow::onQuickAdd()
{
    QuickAddDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        PlatformConfig pc = dlg.getConfig();
        if (pc.name.isEmpty() || pc.authToken.isEmpty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Name and Token are required."));
            return;
        }
        AppSettings::instance().addPlatform(pc);
        refreshAccountList();
    }
}

void MainWindow::onEditAccount()
{
    auto *current = m_accountList->currentItem();
    if (!current)
        return;
    int idx = current->data(Qt::UserRole).toInt();
    PlatformConfig config = AppSettings::instance().platformAt(idx);

    AccountEditDialog dlg(config, this);
    if (dlg.exec() == QDialog::Accepted) {
        AppSettings::instance().setPlatform(idx, dlg.getConfig());
        refreshAccountList();
    }
}

void MainWindow::onRemoveAccount()
{
    auto *current = m_accountList->currentItem();
    if (!current)
        return;
    int idx = current->data(Qt::UserRole).toInt();
    PlatformConfig config = AppSettings::instance().platformAt(idx);
    auto reply = QMessageBox::question(this, tr("Remove Account"),
                                       tr("Remove \"%1\"?").arg(config.name),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        AppSettings::instance().removePlatform(idx);
        refreshAccountList();
    }
}

static QString barChunkColor(int usedPct)
{
    if (usedPct < 50)
        return "#2ECC71";
    if (usedPct < 80)
        return "#F1C40F";
    return "#E74C3C";
}

void MainWindow::displayData(const UsageData &data)
{
    if (!data.isValid) {
        m_statusLabel->setText(QString::fromUtf8("\xe2\x97\x8f ") + data.platformName + ": " + data.errorMsg);
        m_statusLabel->setStyleSheet("color: #E74C3C; font-weight: bold;");
        return;
    }
    m_statusLabel->setText(QString::fromUtf8("\xe2\x97\x8f ") + data.platformName + " [" + data.platformType + "] OK");
    m_statusLabel->setStyleSheet("color: #2ECC71; font-weight: bold;");

    QDateTime lastUpdate = m_dm->lastUpdateTime();
    if (lastUpdate.isValid())
        m_timestampLabel->setText(tr("Updated: %1").arg(lastUpdate.toString("yyyy-MM-dd HH:mm:ss")));
    else
        m_timestampLabel->clear();

    bool isDeepSeek = (data.platformType == "deepseek");

    m_tokenRow->setVisible(!isDeepSeek);
    m_resetRow->setVisible(!isDeepSeek);
    m_mcpRow->setVisible(!isDeepSeek);
    m_mcpDetailRow->setVisible(!isDeepSeek);
    m_balanceRow->setVisible(isDeepSeek);

    if (isDeepSeek) {
        for (const auto &q : data.quotaLimits) {
            if (q.type.startsWith("BALANCE_")) {
                QString cur = q.type.mid(8);
                double total = q.total / 100.0;
                double granted = q.remaining / 100.0;
                double topped = q.currentUsage / 100.0;
                m_balanceLabel->setText(
                    QString("%1 %2 (Granted: %3, Topped: %4)")
                        .arg(total, 0, 'f', 2)
                        .arg(cur)
                        .arg(granted, 0, 'f', 2)
                        .arg(topped, 0, 'f', 2));
            }
        }
    } else {
        for (const auto &q : data.quotaLimits) {
            QString t = q.type.toLower();
            if (t.contains("token") || t == "tokens_limit") {
                int usedPct = static_cast<int>(q.percentage);
                m_tokenBar->setValue(usedPct);
                m_tokenLabel->setText(tr("Used %1%").arg(usedPct));
                m_tokenBar->setStyleSheet(
                    QString("QProgressBar::chunk { background: %1; border-radius: 8px; }")
                        .arg(barChunkColor(usedPct)));
                if (!q.resetTime.isEmpty())
                    m_resetLabel->setText(q.resetTime);
                else
                    m_resetLabel->setText("--");
            }
            if (t.contains("mcp") || t.contains("time_limit") || t.contains("time")) {
                int usedPct = static_cast<int>(q.percentage);
                m_mcpBar->setValue(usedPct);
                QString text = tr("Used %1%").arg(usedPct);
                if (!q.resetTime.isEmpty())
                    text += " (" + tr("Reset: %1").arg(q.resetTime) + ")";
                m_mcpLabel->setText(text);
                m_mcpBar->setStyleSheet(
                    QString("QProgressBar::chunk { background: %1; border-radius: 8px; }")
                        .arg(barChunkColor(usedPct)));
                if (!q.usageDetails.isEmpty())
                    m_mcpDetailLabel->setText(q.usageDetails);
                else
                    m_mcpDetailLabel->setText("--");
            }
        }
    }

    m_totalTokensLabel->setText(formatTokens(data.totalTokens()));
    m_totalRequestsLabel->setText(QString::number(data.totalRequests()));
    m_modelCountLabel->setText(QString::number(data.modelCount()));
    m_toolCountLabel->setText(QString::number(data.toolCount()));

    m_modelTable->setRowCount(data.modelUsage.size());
    for (int i = 0; i < data.modelUsage.size(); i++) {
        const auto &m = data.modelUsage[i];
        m_modelTable->setItem(i, 0, new QTableWidgetItem(m.model));
        m_modelTable->setItem(i, 1, new QTableWidgetItem(m.provider));
        auto *inp = new QTableWidgetItem(formatTokens(m.inputTokens));
        inp->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_modelTable->setItem(i, 2, inp);
        auto *out = new QTableWidgetItem(formatTokens(m.outputTokens));
        out->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_modelTable->setItem(i, 3, out);
        auto *tot = new QTableWidgetItem(formatTokens(m.totalTokens));
        tot->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_modelTable->setItem(i, 4, tot);
        auto *req = new QTableWidgetItem(QString::number(m.requestCount));
        req->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_modelTable->setItem(i, 5, req);
    }
    m_modelTable->resizeColumnsToContents();

    m_toolTable->setRowCount(data.toolUsage.size());
    for (int i = 0; i < data.toolUsage.size(); i++) {
        const auto &t = data.toolUsage[i];
        m_toolTable->setItem(i, 0, new QTableWidgetItem(t.toolName));
        auto *calls = new QTableWidgetItem(QString::number(t.callCount));
        calls->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_toolTable->setItem(i, 1, calls);
    }
    m_toolTable->resizeColumnsToContents();
}

void MainWindow::displayEmpty()
{
    m_statusLabel->setText("--");
    m_statusLabel->setStyleSheet("");
    m_timestampLabel->clear();
    m_tokenRow->setVisible(true);
    m_resetRow->setVisible(true);
    m_mcpRow->setVisible(true);
    m_mcpDetailRow->setVisible(true);
    m_balanceRow->setVisible(true);
    m_tokenBar->setValue(0);
    m_tokenLabel->setText("--");
    m_resetLabel->setText("--");
    m_mcpBar->setValue(0);
    m_mcpLabel->setText("--");
    m_mcpDetailLabel->setText("--");
    m_balanceLabel->setText("--");
    m_totalTokensLabel->setText("--");
    m_totalRequestsLabel->setText("--");
    m_modelCountLabel->setText("--");
    m_toolCountLabel->setText("--");
    m_modelTable->setRowCount(0);
    m_toolTable->setRowCount(0);
}

QString MainWindow::formatTokens(qint64 n) const
{
    if (n >= 1000000000)
        return QString::number(n / 1000000000.0, 'f', 2) + "B";
    if (n >= 1000000)
        return QString::number(n / 1000000.0, 'f', 2) + "M";
    if (n >= 1000)
        return QString::number(n / 1000.0, 'f', 1) + "K";
    return QString::number(n);
}
