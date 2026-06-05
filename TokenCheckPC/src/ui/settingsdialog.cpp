#include "settingsdialog.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "theme.h"
#include "component/colorbutton.h"
#include "component/hotkeybutton.h"
#include "component/noscrollcombobox.h"
#include "component/noscrollspinbox.h"
#include "component/mydoubleslider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QFrame>
#include <QShowEvent>

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
    m_tokenToggle = new QPushButton(QString::fromUtf8("\xF0\x9F\x91\x81"));
    m_tokenToggle->setFixedSize(36, 36);
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

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(640, 640);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_tabs = new QTabWidget();
    m_tabs->addTab(createAccountsPage(), tr("Accounts"));
    m_tabs->addTab(createBallPage(), tr("Floating Ball"));
    m_tabs->addTab(createGeneralPage(), tr("General"));
    mainLayout->addWidget(m_tabs);

    auto *bottomBar = new QHBoxLayout();
    bottomBar->setContentsMargins(16, 8, 16, 12);
    bottomBar->addStretch();
    auto *applyBtn = new QPushButton(tr("Apply"));
    applyBtn->setProperty("class", "primary");
    applyBtn->setFixedWidth(120);
    connect(applyBtn, &QPushButton::clicked, this, [this]() { applySettings(); accept(); });
    bottomBar->addWidget(applyBtn);
    auto *closeBtn = new QPushButton(tr("Close"));
    closeBtn->setFixedWidth(100);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    bottomBar->addWidget(closeBtn);
    mainLayout->addLayout(bottomBar);
}

void SettingsDialog::reloadAll() { reloadSettings(); refreshAccountList(); }

void SettingsDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    const AppSettings &s = AppSettings::instance();
    m_ballSnapshot = {
        s.ballSize(), s.ballBgOpacity(), s.ringWidth(),
        s.pctFontSize(), s.timeFontSize(),
        s.pctColor(), s.timeColor(), s.ballBgColor()
    };
}

void SettingsDialog::reject()
{
    AppSettings &s = AppSettings::instance();
    s.setBallSize(m_ballSnapshot.ballSize);
    s.setBallBgOpacity(m_ballSnapshot.ballBgOpacity);
    s.setRingWidth(m_ballSnapshot.ringWidth);
    s.setPctFontSize(m_ballSnapshot.pctFontSize);
    s.setTimeFontSize(m_ballSnapshot.timeFontSize);
    s.setPctColor(m_ballSnapshot.pctColor);
    s.setTimeColor(m_ballSnapshot.timeColor);
    s.setBallBgColor(m_ballSnapshot.ballBgColor);
    emit previewBallChanged();
    QDialog::reject();
}

QWidget *SettingsDialog::createAccountsPage()
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
    connect(addBtn, &QPushButton::clicked, this, &SettingsDialog::onQuickAdd);
    connect(editBtn, &QPushButton::clicked, this, &SettingsDialog::onEditAccount);
    connect(removeBtn, &QPushButton::clicked, this, &SettingsDialog::onRemoveAccount);
    connect(m_accountList, &QListWidget::itemDoubleClicked, this, &SettingsDialog::onEditAccount);
    refreshAccountList();
    return page;
}

QWidget *SettingsDialog::createBallPage()
{
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *page = new QWidget();
    auto *form = new QVBoxLayout(page);
    form->setSpacing(16);
    form->setContentsMargins(16, 16, 16, 16);

    auto *sizeGroup = new QGroupBox(tr("Size && Opacity"));
    auto *sizeForm = new QFormLayout(sizeGroup);
    sizeForm->setSpacing(12);
    sizeForm->setContentsMargins(16, 20, 16, 16);
    sizeForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_ballSizeSpin = new NoScrollSpinBox();
    m_ballSizeSpin->setRange(60, 150);
    m_ballSizeSpin->setValue(AppSettings::instance().ballSize());
    m_ballSizeSpin->setSuffix(" px");
    m_ballSizeSpin->setMinimumWidth(100);
    sizeForm->addRow(tr("Ball Size:"), m_ballSizeSpin);

    m_ballBgOpacitySpin = new NoScrollSpinBox();
    m_ballBgOpacitySpin->setRange(0, 100);
    m_ballBgOpacitySpin->setValue(AppSettings::instance().ballBgOpacity());
    m_ballBgOpacitySpin->setSuffix(" %");
    m_ballBgOpacitySpin->setMinimumWidth(100);
    sizeForm->addRow(tr("Background Opacity:"), m_ballBgOpacitySpin);

    m_ringWidthSpin = new NoScrollSpinBox();
    m_ringWidthSpin->setRange(2, 16);
    m_ringWidthSpin->setValue(AppSettings::instance().ringWidth());
    m_ringWidthSpin->setSuffix(" px");
    m_ringWidthSpin->setMinimumWidth(100);
    sizeForm->addRow(tr("Ring Width:"), m_ringWidthSpin);

    m_pctFontSpin = new NoScrollSpinBox();
    m_pctFontSpin->setRange(0, 30);
    m_pctFontSpin->setValue(AppSettings::instance().pctFontSize());
    m_pctFontSpin->setSpecialValueText("Auto");
    m_pctFontSpin->setSuffix(" px");
    m_pctFontSpin->setMinimumWidth(100);
    sizeForm->addRow(tr("% Font Size:"), m_pctFontSpin);

    m_timeFontSpin = new NoScrollSpinBox();
    m_timeFontSpin->setRange(0, 30);
    m_timeFontSpin->setValue(AppSettings::instance().timeFontSize());
    m_timeFontSpin->setSpecialValueText("Auto");
    m_timeFontSpin->setSuffix(" px");
    m_timeFontSpin->setMinimumWidth(100);
    sizeForm->addRow(tr("Time Font Size:"), m_timeFontSpin);

    auto *pctColorRow = new QHBoxLayout();
    pctColorRow->setSpacing(8);
    m_chosenPctColor = AppSettings::instance().pctColor();
    m_pctColorBtn = new ColorButton(m_chosenPctColor, tr("Default"));
    pctColorRow->addWidget(m_pctColorBtn);
    pctColorRow->addStretch();
    sizeForm->addRow(tr("% Text Color:"), pctColorRow);

    auto *timeColorRow = new QHBoxLayout();
    timeColorRow->setSpacing(8);
    m_chosenTimeColor = AppSettings::instance().timeColor();
    m_timeColorBtn = new ColorButton(m_chosenTimeColor, tr("Default"));
    timeColorRow->addWidget(m_timeColorBtn);
    timeColorRow->addStretch();
    sizeForm->addRow(tr("Time Text Color:"), timeColorRow);

    auto *ballBgColorRow = new QHBoxLayout();
    ballBgColorRow->setSpacing(8);
    m_ballBgColorBtn = new ColorButton(AppSettings::instance().ballBgColor(), tr("Default"));
    ballBgColorRow->addWidget(m_ballBgColorBtn);
    ballBgColorRow->addStretch();
    sizeForm->addRow(tr("Ball Background:"), ballBgColorRow);

    form->addWidget(sizeGroup);

    auto *glmGroup = new QGroupBox(tr("GLM Progress Bar Colors"));
    auto *glmLayout = new QVBoxLayout(glmGroup);
    glmLayout->setSpacing(10);
    glmLayout->setContentsMargins(16, 20, 16, 16);

    m_glmRangeSlider = new myDoubleSlider(Qt::Horizontal, myDoubleSlider::DoubleHandles);
    m_glmRangeSlider->SetRange(0, 100);
    m_glmRangeSlider->SetLowerValue(AppSettings::instance().glmGreenThreshold());
    m_glmRangeSlider->SetUpperValue(AppSettings::instance().glmYellowThreshold());
    m_glmRangeSlider->setMinimumHeight(22);
    glmLayout->addWidget(m_glmRangeSlider);

    auto *glmColorGrid = new QFormLayout();
    glmColorGrid->setSpacing(8);
    glmColorGrid->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_glmGreenColorBtn = new ColorButton(AppSettings::instance().glmGreenColor(), tr("Green"));
    auto *glmGreenRow = new QHBoxLayout();
    glmGreenRow->addWidget(m_glmGreenColorBtn);
    glmGreenRow->addStretch();
    m_glmGreenLabel = new QLabel();
    glmColorGrid->addRow(m_glmGreenLabel, glmGreenRow);

    m_glmYellowColorBtn = new ColorButton(AppSettings::instance().glmYellowColor(), tr("Yellow"));
    auto *glmYellowRow = new QHBoxLayout();
    glmYellowRow->addWidget(m_glmYellowColorBtn);
    glmYellowRow->addStretch();
    m_glmYellowLabel = new QLabel();
    glmColorGrid->addRow(m_glmYellowLabel, glmYellowRow);

    m_glmRedColorBtn = new ColorButton(AppSettings::instance().glmRedColor(), tr("Red"));
    auto *glmRedRow = new QHBoxLayout();
    glmRedRow->addWidget(m_glmRedColorBtn);
    glmRedRow->addStretch();
    m_glmRedLabel = new QLabel();
    glmColorGrid->addRow(m_glmRedLabel, glmRedRow);

    glmLayout->addLayout(glmColorGrid);
    form->addWidget(glmGroup);

    auto *dsGroup = new QGroupBox(tr("DeepSeek CNY Balance Colors"));
    auto *dsLayout = new QVBoxLayout(dsGroup);
    dsLayout->setSpacing(10);
    dsLayout->setContentsMargins(16, 20, 16, 16);

    auto *dsTotalRow = new QHBoxLayout();
    dsTotalRow->setSpacing(8);
    auto *dsTotalLbl = new QLabel(tr("Total Balance:"));
    dsTotalLbl->setFixedWidth(110);
    dsTotalRow->addWidget(dsTotalLbl);
    m_dsTotalBalanceSpin = new NoScrollDoubleSpinBox();
    m_dsTotalBalanceSpin->setRange(0.01, 999999.0);
    m_dsTotalBalanceSpin->setDecimals(2);
    m_dsTotalBalanceSpin->setValue(AppSettings::instance().dsTotalBalance());
    m_dsTotalBalanceSpin->setSuffix(" CNY");
    m_dsTotalBalanceSpin->setMinimumWidth(130);
    dsTotalRow->addWidget(m_dsTotalBalanceSpin);
    dsTotalRow->addStretch();
    dsLayout->addLayout(dsTotalRow);

    m_dsRangeSlider = new myDoubleSlider(Qt::Horizontal, myDoubleSlider::DoubleHandles);
    m_dsRangeSlider->SetRange(0, 100);

    int dsGreenPct = AppSettings::instance().dsGreenThreshold() > 0
                         ? qRound(AppSettings::instance().dsGreenThreshold()
                                  / AppSettings::instance().dsTotalBalance() * 100.0)
                         : 10;
    int dsYellowPct = AppSettings::instance().dsYellowThreshold() > 0
                          ? qRound(AppSettings::instance().dsYellowThreshold()
                                   / AppSettings::instance().dsTotalBalance() * 100.0)
                          : 50;
    if (dsGreenPct > 99) dsGreenPct = 99;
    if (dsYellowPct > 99) dsYellowPct = 99;
    if (dsGreenPct >= dsYellowPct) dsYellowPct = dsGreenPct + 1;
    m_dsRangeSlider->SetLowerValue(dsGreenPct);
    m_dsRangeSlider->SetUpperValue(dsYellowPct);
    m_dsRangeSlider->setMinimumHeight(22);
    dsLayout->addWidget(m_dsRangeSlider);

    auto *dsColorGrid = new QFormLayout();
    dsColorGrid->setSpacing(8);
    dsColorGrid->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_dsGreenColorBtn = new ColorButton(AppSettings::instance().dsGreenColor(), tr("Green"));
    auto *dsGreenRow = new QHBoxLayout();
    dsGreenRow->addWidget(m_dsGreenColorBtn);
    dsGreenRow->addStretch();
    m_dsGreenLabel = new QLabel();
    dsColorGrid->addRow(m_dsGreenLabel, dsGreenRow);

    m_dsYellowColorBtn = new ColorButton(AppSettings::instance().dsYellowColor(), tr("Yellow"));
    auto *dsYellowRow = new QHBoxLayout();
    dsYellowRow->addWidget(m_dsYellowColorBtn);
    dsYellowRow->addStretch();
    m_dsYellowLabel = new QLabel();
    dsColorGrid->addRow(m_dsYellowLabel, dsYellowRow);

    m_dsRedColorBtn = new ColorButton(AppSettings::instance().dsRedColor(), tr("Red"));
    auto *dsRedRow = new QHBoxLayout();
    dsRedRow->addWidget(m_dsRedColorBtn);
    dsRedRow->addStretch();
    m_dsRedLabel = new QLabel();
    dsColorGrid->addRow(m_dsRedLabel, dsRedRow);

    dsLayout->addLayout(dsColorGrid);
    form->addWidget(dsGroup);

    m_dsShowUSDCheck = new QCheckBox(tr("Show USD balance on floating ball"));
    m_dsShowUSDCheck->setChecked(AppSettings::instance().dsShowUSD());
    form->addWidget(m_dsShowUSDCheck);

    m_dsUsdWidget = new QWidget();
    auto *dsUsdLayout = new QVBoxLayout(m_dsUsdWidget);
    dsUsdLayout->setSpacing(10);
    dsUsdLayout->setContentsMargins(0, 0, 0, 0);

    auto *dsUsdGroup = new QGroupBox(tr("DeepSeek USD Balance Colors"));
    auto *dsUsdInner = new QVBoxLayout(dsUsdGroup);
    dsUsdInner->setSpacing(10);
    dsUsdInner->setContentsMargins(16, 20, 16, 16);

    auto *dsUsdTotalRow = new QHBoxLayout();
    dsUsdTotalRow->setSpacing(8);
    auto *dsUsdTotalLbl = new QLabel(tr("Total Balance:"));
    dsUsdTotalLbl->setFixedWidth(110);
    dsUsdTotalRow->addWidget(dsUsdTotalLbl);
    m_dsUsdTotalBalanceSpin = new NoScrollDoubleSpinBox();
    m_dsUsdTotalBalanceSpin->setRange(0.01, 999999.0);
    m_dsUsdTotalBalanceSpin->setDecimals(2);
    m_dsUsdTotalBalanceSpin->setValue(AppSettings::instance().dsUsdTotalBalance());
    m_dsUsdTotalBalanceSpin->setSuffix(" USD");
    m_dsUsdTotalBalanceSpin->setMinimumWidth(130);
    dsUsdTotalRow->addWidget(m_dsUsdTotalBalanceSpin);
    dsUsdTotalRow->addStretch();
    dsUsdInner->addLayout(dsUsdTotalRow);

    m_dsUsdRangeSlider = new myDoubleSlider(Qt::Horizontal, myDoubleSlider::DoubleHandles);
    m_dsUsdRangeSlider->SetRange(0, 100);
    {
        int usdGP = AppSettings::instance().dsUsdGreenThreshold() > 0
                        ? qRound(AppSettings::instance().dsUsdGreenThreshold()
                                 / AppSettings::instance().dsUsdTotalBalance() * 100.0)
                        : 50;
        int usdYP = AppSettings::instance().dsUsdYellowThreshold() > 0
                        ? qRound(AppSettings::instance().dsUsdYellowThreshold()
                                 / AppSettings::instance().dsUsdTotalBalance() * 100.0)
                        : 80;
        if (usdGP > 99) usdGP = 99;
        if (usdYP > 99) usdYP = 99;
        if (usdGP >= usdYP) usdYP = usdGP + 1;
        m_dsUsdRangeSlider->SetLowerValue(usdGP);
        m_dsUsdRangeSlider->SetUpperValue(usdYP);
    }
    m_dsUsdRangeSlider->setMinimumHeight(22);
    dsUsdInner->addWidget(m_dsUsdRangeSlider);

    auto *dsUsdColorGrid = new QFormLayout();
    dsUsdColorGrid->setSpacing(8);
    dsUsdColorGrid->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_dsUsdGreenColorBtn = new ColorButton(AppSettings::instance().dsUsdGreenColor(), tr("Green"));
    auto *dsUsdGreenRow = new QHBoxLayout();
    dsUsdGreenRow->addWidget(m_dsUsdGreenColorBtn);
    dsUsdGreenRow->addStretch();
    m_dsUsdGreenLabel = new QLabel();
    dsUsdColorGrid->addRow(m_dsUsdGreenLabel, dsUsdGreenRow);

    m_dsUsdYellowColorBtn = new ColorButton(AppSettings::instance().dsUsdYellowColor(), tr("Yellow"));
    auto *dsUsdYellowRow = new QHBoxLayout();
    dsUsdYellowRow->addWidget(m_dsUsdYellowColorBtn);
    dsUsdYellowRow->addStretch();
    m_dsUsdYellowLabel = new QLabel();
    dsUsdColorGrid->addRow(m_dsUsdYellowLabel, dsUsdYellowRow);

    m_dsUsdRedColorBtn = new ColorButton(AppSettings::instance().dsUsdRedColor(), tr("Red"));
    auto *dsUsdRedRow = new QHBoxLayout();
    dsUsdRedRow->addWidget(m_dsUsdRedColorBtn);
    dsUsdRedRow->addStretch();
    m_dsUsdRedLabel = new QLabel();
    dsUsdColorGrid->addRow(m_dsUsdRedLabel, dsUsdRedRow);

    dsUsdInner->addLayout(dsUsdColorGrid);
    dsUsdLayout->addWidget(dsUsdGroup);
    form->addWidget(m_dsUsdWidget);

    m_dsUsdWidget->setVisible(m_dsShowUSDCheck->isChecked());

    form->addStretch();

    connect(m_glmRangeSlider, &myDoubleSlider::lowerValueChanged, this, [this](int) { updateGlmLabels(); });
    connect(m_glmRangeSlider, &myDoubleSlider::upperValueChanged, this, [this](int) { updateGlmLabels(); });
    connect(m_dsRangeSlider, &myDoubleSlider::lowerValueChanged, this, [this](int) { updateDsLabels(); });
    connect(m_dsRangeSlider, &myDoubleSlider::upperValueChanged, this, [this](int) { updateDsLabels(); });
    connect(m_dsTotalBalanceSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this](double) { updateDsLabels(); });
    connect(m_dsUsdRangeSlider, &myDoubleSlider::lowerValueChanged, this, [this](int) { updateDsUsdLabels(); });
    connect(m_dsUsdRangeSlider, &myDoubleSlider::upperValueChanged, this, [this](int) { updateDsUsdLabels(); });
    connect(m_dsUsdTotalBalanceSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this](double) { updateDsUsdLabels(); });
    connect(m_dsShowUSDCheck, &QCheckBox::toggled, m_dsUsdWidget, &QWidget::setVisible);

    updateGlmLabels();
    updateDsLabels();
    updateDsUsdLabels();

    auto applyPreview = [this]() {
        AppSettings &s = AppSettings::instance();
        s.setBallSize(m_ballSizeSpin->value());
        s.setBallBgOpacity(m_ballBgOpacitySpin->value());
        s.setRingWidth(m_ringWidthSpin->value());
        s.setPctFontSize(m_pctFontSpin->value());
        s.setTimeFontSize(m_timeFontSpin->value());
        s.setPctColor(m_pctColorBtn->chosenColor());
        s.setTimeColor(m_timeColorBtn->chosenColor());
        s.setBallBgColor(m_ballBgColorBtn->chosenColor());
        emit previewBallChanged();
    };

    connect(m_ballSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, applyPreview);
    connect(m_ballBgOpacitySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, applyPreview);
    connect(m_ringWidthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, applyPreview);
    connect(m_pctFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, applyPreview);
    connect(m_timeFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, applyPreview);
    connect(m_pctColorBtn, &ColorButton::colorChanged, this, applyPreview);
    connect(m_timeColorBtn, &ColorButton::colorChanged, this, applyPreview);
    connect(m_ballBgColorBtn, &ColorButton::colorChanged, this, applyPreview);

    scroll->setWidget(page);
    return scroll;
}

void SettingsDialog::updateGlmLabels()
{
    int low = m_glmRangeSlider->GetLowerValue();
    int high = m_glmRangeSlider->GetUpperValue();
    m_glmGreenLabel->setText(tr("0 ~ %1%:").arg(low));
    m_glmYellowLabel->setText(tr("%1% ~ %2%:").arg(low).arg(high));
    m_glmRedLabel->setText(tr("> %1%:").arg(high));
}

void SettingsDialog::updateDsLabels()
{
    int low = m_dsRangeSlider->GetLowerValue();
    int high = m_dsRangeSlider->GetUpperValue();
    double total = m_dsTotalBalanceSpin->value();
    double v1 = total * low / 100.0;
    double v2 = total * high / 100.0;
    m_dsGreenLabel->setText(tr("0 ~ %1 CNY (%2%):").arg(v1, 0, 'f', 2).arg(low));
    m_dsYellowLabel->setText(tr("%1 ~ %2 CNY (%3% ~ %4%):").arg(v1, 0, 'f', 2).arg(v2, 0, 'f', 2).arg(low).arg(high));
    m_dsRedLabel->setText(tr("> %1 CNY (%2%):").arg(v2, 0, 'f', 2).arg(high));
}

void SettingsDialog::updateDsUsdLabels()
{
    int low = m_dsUsdRangeSlider->GetLowerValue();
    int high = m_dsUsdRangeSlider->GetUpperValue();
    double total = m_dsUsdTotalBalanceSpin->value();
    double v1 = total * low / 100.0;
    double v2 = total * high / 100.0;
    m_dsUsdGreenLabel->setText(tr("0 ~ %1 USD (%2%):").arg(v1, 0, 'f', 2).arg(low));
    m_dsUsdYellowLabel->setText(tr("%1 ~ %2 USD (%3% ~ %4%):").arg(v1, 0, 'f', 2).arg(v2, 0, 'f', 2).arg(low).arg(high));
    m_dsUsdRedLabel->setText(tr("> %1 USD (%2%):").arg(v2, 0, 'f', 2).arg(high));
}

QWidget *SettingsDialog::createGeneralPage()
{
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *page = new QWidget();
    auto *mainForm = new QVBoxLayout(page);
    mainForm->setSpacing(16);
    mainForm->setContentsMargins(16, 16, 16, 16);

    auto *themeGroup = new QGroupBox(tr("Theme && Language"));
    auto *themeForm = new QFormLayout(themeGroup);
    themeForm->setSpacing(12);
    themeForm->setContentsMargins(16, 20, 16, 16);
    themeForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_themeCombo = new NoScrollComboBox();
    m_themeCombo->addItem(tr("Dark"), 0);
    m_themeCombo->addItem(tr("Light"), 1);
    int tidx = m_themeCombo->findData(static_cast<int>(AppSettings::instance().themeId()));
    if (tidx >= 0) m_themeCombo->setCurrentIndex(tidx);
    m_themeCombo->setMinimumWidth(160);
    themeForm->addRow(tr("Color Theme:"), m_themeCombo);

    m_langCombo = new NoScrollComboBox();
    m_langCombo->addItem(tr("System Default"), QString());
    m_langCombo->addItem("English", "en");
    m_langCombo->addItem(QString::fromUtf8("\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87"), "zh_CN");
    QString curLang = AppSettings::instance().language();
    int lidx = m_langCombo->findData(curLang);
    if (lidx >= 0) m_langCombo->setCurrentIndex(lidx);
    m_langCombo->setMinimumWidth(160);
    themeForm->addRow(tr("Language:"), m_langCombo);

    auto *langHint = new QLabel(tr("Language change takes effect after restart."));
    langHint->setProperty("class", "dim");
    langHint->setStyleSheet("font-size: 11px;");
    themeForm->addRow(QString(), langHint);

    mainForm->addWidget(themeGroup);

    auto *refreshGroup = new QGroupBox(tr("Refresh && Startup"));
    auto *refreshForm = new QFormLayout(refreshGroup);
    refreshForm->setSpacing(12);
    refreshForm->setContentsMargins(16, 20, 16, 16);
    refreshForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_intervalSpin = new NoScrollSpinBox();
    m_intervalSpin->setRange(1, 120);
    m_intervalSpin->setValue(AppSettings::instance().autoRefreshInterval());
    m_intervalSpin->setSuffix(" min");
    m_intervalSpin->setMinimumWidth(100);
    refreshForm->addRow(tr("Interval:"), m_intervalSpin);

    m_notifySpin = new NoScrollSpinBox();
    m_notifySpin->setRange(1, 50);
    m_notifySpin->setValue(AppSettings::instance().notifyThreshold());
    m_notifySpin->setSuffix(" %");
    m_notifySpin->setMinimumWidth(100);
    refreshForm->addRow(tr("Notify below:"), m_notifySpin);

    m_autoStartCheck = new QCheckBox();
    refreshForm->addRow(tr("Auto start on login:"), m_autoStartCheck);

    mainForm->addWidget(refreshGroup);

    auto *hotkeyGroup = new QGroupBox(tr("Hotkeys"));
    auto *hotkeyForm = new QFormLayout(hotkeyGroup);
    hotkeyForm->setSpacing(12);
    hotkeyForm->setContentsMargins(16, 20, 16, 16);
    hotkeyForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *toggleRow = new QHBoxLayout();
    toggleRow->setSpacing(12);
    m_toggleHotkeyBtn = new HotkeyButton();
    m_toggleHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyToggle());
    toggleRow->addWidget(m_toggleHotkeyBtn);
    auto *clearToggle = new QPushButton(tr("Clear"));
    clearToggle->setFixedWidth(60);
    clearToggle->setFixedHeight(32);
    toggleRow->addWidget(clearToggle);
    toggleRow->addStretch();
    connect(clearToggle, &QPushButton::clicked, this, [this]() { m_toggleHotkeyBtn->setKeySequence(QString()); });
    hotkeyForm->addRow(tr("Toggle ball:"), toggleRow);

    auto *refreshRow = new QHBoxLayout();
    refreshRow->setSpacing(12);
    m_refreshHotkeyBtn = new HotkeyButton();
    m_refreshHotkeyBtn->setKeySequence(AppSettings::instance().hotkeyRefresh());
    refreshRow->addWidget(m_refreshHotkeyBtn);
    auto *clearRefresh = new QPushButton(tr("Clear"));
    clearRefresh->setFixedWidth(60);
    clearRefresh->setFixedHeight(32);
    refreshRow->addWidget(clearRefresh);
    refreshRow->addStretch();
    connect(clearRefresh, &QPushButton::clicked, this, [this]() { m_refreshHotkeyBtn->setKeySequence(QString()); });
    hotkeyForm->addRow(tr("Refresh:"), refreshRow);

    mainForm->addWidget(hotkeyGroup);

    auto *proxyGroup = new QGroupBox(tr("Proxy"));
    auto *proxyForm = new QFormLayout(proxyGroup);
    proxyForm->setSpacing(12);
    proxyForm->setContentsMargins(16, 20, 16, 16);
    proxyForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_proxyTypeCombo = new NoScrollComboBox();
    m_proxyTypeCombo->addItem(tr("No Proxy"), 0);
    m_proxyTypeCombo->addItem("HTTP", 1);
    m_proxyTypeCombo->addItem("SOCKS5", 2);
    int pidx = m_proxyTypeCombo->findData(AppSettings::instance().proxyType());
    if (pidx >= 0) m_proxyTypeCombo->setCurrentIndex(pidx);
    m_proxyTypeCombo->setMinimumWidth(100);
    proxyForm->addRow(tr("Type:"), m_proxyTypeCombo);

    m_proxyHostEdit = new QLineEdit(AppSettings::instance().proxyHost());
    m_proxyHostEdit->setMinimumWidth(160);
    proxyForm->addRow(tr("Host:"), m_proxyHostEdit);

    m_proxyPortSpin = new NoScrollSpinBox();
    m_proxyPortSpin->setRange(0, 65535);
    m_proxyPortSpin->setValue(AppSettings::instance().proxyPort());
    m_proxyPortSpin->setMinimumWidth(100);
    proxyForm->addRow(tr("Port:"), m_proxyPortSpin);

    mainForm->addWidget(proxyGroup);
    mainForm->addStretch();

    scroll->setWidget(page);
    return scroll;
}

void SettingsDialog::refreshAccountList()
{
    m_accountList->clear();
    auto platforms = AppSettings::instance().allPlatforms();
    for (int i = 0; i < platforms.size(); i++) {
        const auto &p = platforms[i];
        QString dot = p.enabled ? QString::fromUtf8("\xe2\x97\x8f") : QString::fromUtf8("\xe2\x97\x8b");
        QString line1 = QString("%1 %2 [%3]").arg(dot, p.name, p.platformType);
        if (!p.enabled) line1 += " (" + tr("Disabled") + ")";
        auto *item = new QListWidgetItem();
        item->setData(Qt::UserRole, i);
        item->setText(line1);
        if (!p.enabled) item->setForeground(QColor(108, 112, 134));
        m_accountList->addItem(item);
    }
}

void SettingsDialog::onQuickAdd()
{
    QuickAddDialog dlg(this);
    while (dlg.exec() == QDialog::Accepted) {
        PlatformConfig pc = dlg.getConfig();
        auto platforms = AppSettings::instance().allPlatforms();
        bool dup = false;
        for (const auto &p : platforms) {
            if (p.name == pc.name) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("An account named \"%1\" already exists.").arg(pc.name));
                dup = true;
                break;
            }
        }
        if (dup)
            continue;
        AppSettings::instance().addPlatform(pc);
        refreshAccountList();
        break;
    }
}

void SettingsDialog::onEditAccount()
{
    auto *current = m_accountList->currentItem();
    if (!current) return;
    int idx = current->data(Qt::UserRole).toInt();
    PlatformConfig config = AppSettings::instance().platformAt(idx);
    AccountEditDialog dlg(config, this);
    if (dlg.exec() == QDialog::Accepted) {
        AppSettings::instance().setPlatform(idx, dlg.getConfig());
        refreshAccountList();
    }
}

void SettingsDialog::onRemoveAccount()
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
    }
}

void SettingsDialog::applySettings()
{
    AppSettings &s = AppSettings::instance();
    s.setBallSize(m_ballSizeSpin->value());
    s.setBallBgOpacity(m_ballBgOpacitySpin->value());
    s.setRingWidth(m_ringWidthSpin->value());
    s.setPctFontSize(m_pctFontSpin->value());
    s.setTimeFontSize(m_timeFontSpin->value());
    s.setPctColor(m_pctColorBtn->chosenColor());
    s.setTimeColor(m_timeColorBtn->chosenColor());
    s.setBallBgColor(m_ballBgColorBtn->chosenColor());

    s.setGlmGreenThreshold(m_glmRangeSlider->GetLowerValue());
    s.setGlmYellowThreshold(m_glmRangeSlider->GetUpperValue());
    s.setGlmGreenColor(m_glmGreenColorBtn->chosenColor());
    s.setGlmYellowColor(m_glmYellowColorBtn->chosenColor());
    s.setGlmRedColor(m_glmRedColorBtn->chosenColor());

    double total = m_dsTotalBalanceSpin->value();
    s.setDsTotalBalance(total);
    s.setDsGreenThreshold(total * m_dsRangeSlider->GetLowerValue() / 100.0);
    s.setDsYellowThreshold(total * m_dsRangeSlider->GetUpperValue() / 100.0);
    s.setDsGreenColor(m_dsGreenColorBtn->chosenColor());
    s.setDsYellowColor(m_dsYellowColorBtn->chosenColor());
    s.setDsRedColor(m_dsRedColorBtn->chosenColor());

    s.setDsShowUSD(m_dsShowUSDCheck->isChecked());
    double usdTotal = m_dsUsdTotalBalanceSpin->value();
    s.setDsUsdTotalBalance(usdTotal);
    s.setDsUsdGreenThreshold(usdTotal * m_dsUsdRangeSlider->GetLowerValue() / 100.0);
    s.setDsUsdYellowThreshold(usdTotal * m_dsUsdRangeSlider->GetUpperValue() / 100.0);
    s.setDsUsdGreenColor(m_dsUsdGreenColorBtn->chosenColor());
    s.setDsUsdYellowColor(m_dsUsdYellowColorBtn->chosenColor());
    s.setDsUsdRedColor(m_dsUsdRedColorBtn->chosenColor());

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

    emit settingsApplied();
}

void SettingsDialog::reloadSettings()
{
    const AppSettings &s = AppSettings::instance();

    m_themeCombo->blockSignals(true);
    m_langCombo->blockSignals(true);
    m_ballSizeSpin->blockSignals(true);
    m_ballBgOpacitySpin->blockSignals(true);
    m_ringWidthSpin->blockSignals(true);
    m_pctFontSpin->blockSignals(true);
    m_timeFontSpin->blockSignals(true);
    m_intervalSpin->blockSignals(true);
    m_autoStartCheck->blockSignals(true);
    m_notifySpin->blockSignals(true);
    m_toggleHotkeyBtn->blockSignals(true);
    m_refreshHotkeyBtn->blockSignals(true);
    m_proxyTypeCombo->blockSignals(true);
    m_proxyHostEdit->blockSignals(true);
    m_proxyPortSpin->blockSignals(true);
    m_dsTotalBalanceSpin->blockSignals(true);
    m_dsUsdTotalBalanceSpin->blockSignals(true);
    m_dsShowUSDCheck->blockSignals(true);

    int tidx = m_themeCombo->findData(static_cast<int>(s.themeId()));
    if (tidx >= 0) m_themeCombo->setCurrentIndex(tidx);
    int lidx = m_langCombo->findData(s.language());
    if (lidx >= 0) m_langCombo->setCurrentIndex(lidx);
    m_ballSizeSpin->setValue(s.ballSize());
    m_ballBgOpacitySpin->setValue(s.ballBgOpacity());
    m_ringWidthSpin->setValue(s.ringWidth());
    m_pctFontSpin->setValue(s.pctFontSize());
    m_timeFontSpin->setValue(s.timeFontSize());
    m_pctColorBtn->setChosenColor(s.pctColor());
    m_timeColorBtn->setChosenColor(s.timeColor());
    m_ballBgColorBtn->setChosenColor(s.ballBgColor());
    m_intervalSpin->setValue(s.autoRefreshInterval());
    m_autoStartCheck->setChecked(s.autoStart());
    m_notifySpin->setValue(s.notifyThreshold());
    m_toggleHotkeyBtn->setKeySequence(s.hotkeyToggle());
    m_refreshHotkeyBtn->setKeySequence(s.hotkeyRefresh());
    int pidx = m_proxyTypeCombo->findData(s.proxyType());
    if (pidx >= 0) m_proxyTypeCombo->setCurrentIndex(pidx);
    m_proxyHostEdit->setText(s.proxyHost());
    m_proxyPortSpin->setValue(s.proxyPort());

    m_glmRangeSlider->blockSignals(true);
    m_glmRangeSlider->SetLowerValue(s.glmGreenThreshold());
    m_glmRangeSlider->SetUpperValue(s.glmYellowThreshold());
    m_glmRangeSlider->blockSignals(false);
    m_glmGreenColorBtn->setChosenColor(s.glmGreenColor());
    m_glmYellowColorBtn->setChosenColor(s.glmYellowColor());
    m_glmRedColorBtn->setChosenColor(s.glmRedColor());

    m_dsRangeSlider->blockSignals(true);
    m_dsTotalBalanceSpin->setValue(s.dsTotalBalance());
    int dsGP = s.dsGreenThreshold() > 0
                   ? qRound(s.dsGreenThreshold() / s.dsTotalBalance() * 100.0)
                   : 10;
    int dsYP = s.dsYellowThreshold() > 0
                   ? qRound(s.dsYellowThreshold() / s.dsTotalBalance() * 100.0)
                   : 50;
    if (dsGP > 99) dsGP = 99;
    if (dsYP > 99) dsYP = 99;
    if (dsGP >= dsYP) dsYP = dsGP + 1;
    m_dsRangeSlider->SetLowerValue(dsGP);
    m_dsRangeSlider->SetUpperValue(dsYP);
    m_dsRangeSlider->blockSignals(false);
    m_dsGreenColorBtn->setChosenColor(s.dsGreenColor());
    m_dsYellowColorBtn->setChosenColor(s.dsYellowColor());
    m_dsRedColorBtn->setChosenColor(s.dsRedColor());

    m_dsShowUSDCheck->setChecked(s.dsShowUSD());
    m_dsUsdWidget->setVisible(s.dsShowUSD());
    m_dsUsdRangeSlider->blockSignals(true);
    m_dsUsdTotalBalanceSpin->setValue(s.dsUsdTotalBalance());
    {
        int usdGP = s.dsUsdGreenThreshold() > 0
                        ? qRound(s.dsUsdGreenThreshold() / s.dsUsdTotalBalance() * 100.0)
                        : 50;
        int usdYP = s.dsUsdYellowThreshold() > 0
                        ? qRound(s.dsUsdYellowThreshold() / s.dsUsdTotalBalance() * 100.0)
                        : 80;
        if (usdGP > 99) usdGP = 99;
        if (usdYP > 99) usdYP = 99;
        if (usdGP >= usdYP) usdYP = usdGP + 1;
        m_dsUsdRangeSlider->SetLowerValue(usdGP);
        m_dsUsdRangeSlider->SetUpperValue(usdYP);
    }
    m_dsUsdRangeSlider->blockSignals(false);
    m_dsUsdGreenColorBtn->setChosenColor(s.dsUsdGreenColor());
    m_dsUsdYellowColorBtn->setChosenColor(s.dsUsdYellowColor());
    m_dsUsdRedColorBtn->setChosenColor(s.dsUsdRedColor());

    updateGlmLabels();
    updateDsLabels();
    updateDsUsdLabels();

    m_themeCombo->blockSignals(false);
    m_langCombo->blockSignals(false);
    m_ballSizeSpin->blockSignals(false);
    m_ballBgOpacitySpin->blockSignals(false);
    m_ringWidthSpin->blockSignals(false);
    m_pctFontSpin->blockSignals(false);
    m_timeFontSpin->blockSignals(false);
    m_intervalSpin->blockSignals(false);
    m_autoStartCheck->blockSignals(false);
    m_notifySpin->blockSignals(false);
    m_toggleHotkeyBtn->blockSignals(false);
    m_refreshHotkeyBtn->blockSignals(false);
    m_proxyTypeCombo->blockSignals(false);
    m_proxyHostEdit->blockSignals(false);
    m_proxyPortSpin->blockSignals(false);
    m_dsTotalBalanceSpin->blockSignals(false);
    m_dsUsdTotalBalanceSpin->blockSignals(false);
    m_dsShowUSDCheck->blockSignals(false);
}
