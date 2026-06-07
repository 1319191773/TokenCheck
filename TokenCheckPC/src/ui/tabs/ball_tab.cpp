#include "ball_tab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "../component/cardgroup.h"
#include <QLabel>
#include <QScrollArea>
#include <QPropertyAnimation>
#include "../../core/appsettings.h"
#include "../component/colorbutton.h"
#include "../component/modernspinbox.h"
#include "../component/noscrollspinbox.h"
#include "../component/doublerangeslider.h"
#include "../component/toggleswitch.h"

BallTab::BallTab(QWidget *parent) : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto mkLbl = [](const QString &text) { auto *l = new QLabel(text); l->setFixedWidth(130); return l; };
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *page = new QWidget();
    auto *form = new QVBoxLayout(page);
    form->setSpacing(16);
    form->setContentsMargins(16, 16, 16, 16);

    auto *sizeGroup = new CardGroup(tr("Size && Opacity"));
    auto *sizeForm = new QFormLayout();
    sizeForm->setSpacing(12);
    sizeForm->setContentsMargins(0, 4, 0, 0);
    sizeForm->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_ballSizeSpin = new ModernSpinBox();
    m_ballSizeSpin->setRange(60, 150);
    m_ballSizeSpin->setValue(AppSettings::instance().ballSize());
    m_ballSizeSpin->setSuffix(" px");
    m_ballSizeSpin->setMinimumWidth(100);
    sizeForm->addRow(mkLbl(tr("Ball Size:")), m_ballSizeSpin);

    m_ballBgOpacitySpin = new ModernSpinBox();
    m_ballBgOpacitySpin->setRange(0, 100);
    m_ballBgOpacitySpin->setValue(AppSettings::instance().ballBgOpacity());
    m_ballBgOpacitySpin->setSuffix(" %");
    m_ballBgOpacitySpin->setMinimumWidth(100);
    sizeForm->addRow(mkLbl(tr("Background Opacity:")), m_ballBgOpacitySpin);

    m_ringWidthSpin = new ModernSpinBox();
    m_ringWidthSpin->setRange(2, 16);
    m_ringWidthSpin->setValue(AppSettings::instance().ringWidth());
    m_ringWidthSpin->setSuffix(" px");
    m_ringWidthSpin->setMinimumWidth(100);
    sizeForm->addRow(mkLbl(tr("Ring Width:")), m_ringWidthSpin);

    m_pctFontSpin = new ModernSpinBox();
    m_pctFontSpin->setRange(0, 30);
    m_pctFontSpin->setValue(AppSettings::instance().pctFontSize());
    m_pctFontSpin->setSpecialValueText("Auto");
    m_pctFontSpin->setSuffix(" px");
    m_pctFontSpin->setMinimumWidth(100);
    sizeForm->addRow(mkLbl(tr("% Font Size:")), m_pctFontSpin);

    m_timeFontSpin = new ModernSpinBox();
    m_timeFontSpin->setRange(0, 30);
    m_timeFontSpin->setValue(AppSettings::instance().timeFontSize());
    m_timeFontSpin->setSpecialValueText("Auto");
    m_timeFontSpin->setSuffix(" px");
    m_timeFontSpin->setMinimumWidth(100);
    sizeForm->addRow(mkLbl(tr("Time Font Size:")), m_timeFontSpin);

    auto *pctColorRow = new QHBoxLayout();
    pctColorRow->setSpacing(8);
    m_chosenPctColor = AppSettings::instance().pctColor();
    m_pctColorBtn = new ColorButton(m_chosenPctColor, tr("Default"));
    pctColorRow->addWidget(m_pctColorBtn);
    pctColorRow->addStretch();
    sizeForm->addRow(mkLbl(tr("% Text Color:")), pctColorRow);

    auto *timeColorRow = new QHBoxLayout();
    timeColorRow->setSpacing(8);
    m_chosenTimeColor = AppSettings::instance().timeColor();
    m_timeColorBtn = new ColorButton(m_chosenTimeColor, tr("Default"));
    timeColorRow->addWidget(m_timeColorBtn);
    timeColorRow->addStretch();
    sizeForm->addRow(mkLbl(tr("Time Text Color:")), timeColorRow);

    auto *ballBgColorRow = new QHBoxLayout();
    ballBgColorRow->setSpacing(8);
    m_ballBgColorBtn = new ColorButton(AppSettings::instance().ballBgColor(), tr("Default"));
    ballBgColorRow->addWidget(m_ballBgColorBtn);
    ballBgColorRow->addStretch();
    sizeForm->addRow(mkLbl(tr("Ball Background:")), ballBgColorRow);

    auto *presetRow = new QHBoxLayout();
    presetRow->setSpacing(8);
    auto *presetDefaultBtn = new QPushButton(tr("Standard"));
    presetDefaultBtn->setProperty("class", "ghost");
    auto *presetLargeBtn = new QPushButton(tr("Large Glass"));
    presetLargeBtn->setProperty("class", "ghost");
    auto *presetCompactBtn = new QPushButton(tr("Compact Solid"));
    presetCompactBtn->setProperty("class", "ghost");
    
    presetRow->addWidget(presetDefaultBtn);
    presetRow->addWidget(presetLargeBtn);
    presetRow->addWidget(presetCompactBtn);
    presetRow->addStretch();
    sizeForm->addRow(mkLbl(tr("Style Presets:")), presetRow);

    connect(presetDefaultBtn, &QPushButton::clicked, this, [this]() {
        m_ballSizeSpin->setValue(80);
        m_ballBgOpacitySpin->setValue(50);
        m_ringWidthSpin->setValue(4);
        m_pctFontSpin->setValue(0);
        m_timeFontSpin->setValue(0);
        m_pctColorBtn->setChosenColor(QColor());
        m_timeColorBtn->setChosenColor(QColor());
        m_ballBgColorBtn->setChosenColor(QColor());
    });

    connect(presetLargeBtn, &QPushButton::clicked, this, [this]() {
        m_ballSizeSpin->setValue(110);
        m_ballBgOpacitySpin->setValue(20);
        m_ringWidthSpin->setValue(6);
        m_pctFontSpin->setValue(18);
        m_timeFontSpin->setValue(14);
        m_pctColorBtn->setChosenColor(QColor(255, 255, 255));
        m_timeColorBtn->setChosenColor(QColor(220, 220, 220));
        m_ballBgColorBtn->setChosenColor(QColor(0, 0, 0));
    });

    connect(presetCompactBtn, &QPushButton::clicked, this, [this]() {
        m_ballSizeSpin->setValue(65);
        m_ballBgOpacitySpin->setValue(100);
        m_ringWidthSpin->setValue(3);
        m_pctFontSpin->setValue(11);
        m_timeFontSpin->setValue(10);
        m_pctColorBtn->setChosenColor(QColor(255, 255, 255));
        m_timeColorBtn->setChosenColor(QColor(255, 255, 255));
        m_ballBgColorBtn->setChosenColor(QColor(30, 30, 30));
    });

    sizeGroup->addLayout(sizeForm);
    form->addWidget(sizeGroup);

    auto *glmGroup = new CardGroup(tr("GLM Progress Bar Colors"));
    auto *glmLayout = new QVBoxLayout();
    glmLayout->setSpacing(10);
    glmLayout->setContentsMargins(0, 4, 0, 0);

    m_glmRangeSlider = new DoubleRangeSlider(Qt::Horizontal, DoubleRangeSlider::DoubleHandles);
    m_glmRangeSlider->SetRange(0, 100);
    m_glmRangeSlider->SetLowerValue(AppSettings::instance().glmGreenThreshold());
    m_glmRangeSlider->SetUpperValue(AppSettings::instance().glmYellowThreshold());
    m_glmRangeSlider->setMinimumHeight(22);
    glmLayout->addWidget(m_glmRangeSlider);

    auto *glmColorGrid = new QFormLayout();
    glmColorGrid->setSpacing(8);
    glmColorGrid->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_glmGreenColorBtn = new ColorButton(AppSettings::instance().glmGreenColor(), tr("Green"));
    auto *glmGreenRow = new QHBoxLayout();
    glmGreenRow->addWidget(m_glmGreenColorBtn);
    glmGreenRow->addStretch();
    m_glmGreenLabel = new QLabel();
    m_glmGreenLabel->setFixedWidth(130);
    glmColorGrid->addRow(m_glmGreenLabel, glmGreenRow);

    m_glmYellowColorBtn = new ColorButton(AppSettings::instance().glmYellowColor(), tr("Yellow"));
    auto *glmYellowRow = new QHBoxLayout();
    glmYellowRow->addWidget(m_glmYellowColorBtn);
    glmYellowRow->addStretch();
    m_glmYellowLabel = new QLabel();
    m_glmYellowLabel->setFixedWidth(130);
    glmColorGrid->addRow(m_glmYellowLabel, glmYellowRow);

    m_glmRedColorBtn = new ColorButton(AppSettings::instance().glmRedColor(), tr("Red"));
    auto *glmRedRow = new QHBoxLayout();
    glmRedRow->addWidget(m_glmRedColorBtn);
    glmRedRow->addStretch();
    m_glmRedLabel = new QLabel();
    m_glmRedLabel->setFixedWidth(130);
    glmColorGrid->addRow(m_glmRedLabel, glmRedRow);

    glmLayout->addLayout(glmColorGrid);
    glmGroup->addLayout(glmLayout);
    form->addWidget(glmGroup);

    auto *dsGroup = new CardGroup(tr("DeepSeek CNY Balance Colors"));
    auto *dsLayout = new QVBoxLayout();
    dsLayout->setSpacing(10);
    dsLayout->setContentsMargins(0, 4, 0, 0);

    auto *dsTotalRow = new QHBoxLayout();
    dsTotalRow->setSpacing(8);
    auto *dsTotalLbl = new QLabel(tr("Total Balance:"));
    dsTotalLbl->setFixedWidth(110);
    dsTotalRow->addWidget(dsTotalLbl);
    m_dsTotalBalanceSpin = new ModernDoubleSpinBox();
    m_dsTotalBalanceSpin->setRange(0.01, 999999.0);
    m_dsTotalBalanceSpin->setDecimals(2);
    m_dsTotalBalanceSpin->setValue(AppSettings::instance().dsTotalBalance());
    m_dsTotalBalanceSpin->setSuffix(" CNY");
    m_dsTotalBalanceSpin->setMinimumWidth(130);
    dsTotalRow->addWidget(m_dsTotalBalanceSpin);
    dsTotalRow->addStretch();
    dsLayout->addLayout(dsTotalRow);

    m_dsRangeSlider = new DoubleRangeSlider(Qt::Horizontal, DoubleRangeSlider::DoubleHandles);
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
    dsColorGrid->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_dsGreenColorBtn = new ColorButton(AppSettings::instance().dsGreenColor(), tr("Green"));
    auto *dsGreenRow = new QHBoxLayout();
    dsGreenRow->addWidget(m_dsGreenColorBtn);
    dsGreenRow->addStretch();
    m_dsGreenLabel = new QLabel();
    m_dsGreenLabel->setFixedWidth(130);
    dsColorGrid->addRow(m_dsGreenLabel, dsGreenRow);

    m_dsYellowColorBtn = new ColorButton(AppSettings::instance().dsYellowColor(), tr("Yellow"));
    auto *dsYellowRow = new QHBoxLayout();
    dsYellowRow->addWidget(m_dsYellowColorBtn);
    dsYellowRow->addStretch();
    m_dsYellowLabel = new QLabel();
    m_dsYellowLabel->setFixedWidth(130);
    dsColorGrid->addRow(m_dsYellowLabel, dsYellowRow);

    m_dsRedColorBtn = new ColorButton(AppSettings::instance().dsRedColor(), tr("Red"));
    auto *dsRedRow = new QHBoxLayout();
    dsRedRow->addWidget(m_dsRedColorBtn);
    dsRedRow->addStretch();
    m_dsRedLabel = new QLabel();
    m_dsRedLabel->setFixedWidth(130);
    dsColorGrid->addRow(m_dsRedLabel, dsRedRow);

    dsLayout->addLayout(dsColorGrid);
    dsGroup->addLayout(dsLayout);
    form->addWidget(dsGroup);

    auto *usdToggleRow = new QHBoxLayout();
    m_dsShowUSDCheck = new ToggleSwitch();
    m_dsShowUSDCheck->setChecked(AppSettings::instance().dsShowUSD());
    usdToggleRow->addWidget(m_dsShowUSDCheck);
    auto *usdToggleLbl = new QLabel(tr("Show USD balance on floating ball"));
    usdToggleRow->addWidget(usdToggleLbl);
    usdToggleRow->addStretch();
    form->addLayout(usdToggleRow);

    m_dsUsdWidget = new QWidget();
    auto *dsUsdLayout = new QVBoxLayout(m_dsUsdWidget);
    dsUsdLayout->setSpacing(10);
    dsUsdLayout->setContentsMargins(0, 0, 0, 0);

    auto *dsUsdGroup = new CardGroup(tr("DeepSeek USD Balance Colors"));
    auto *dsUsdInner = new QVBoxLayout();
    dsUsdInner->setSpacing(10);
    dsUsdInner->setContentsMargins(0, 4, 0, 0);

    auto *dsUsdTotalRow = new QHBoxLayout();
    dsUsdTotalRow->setSpacing(8);
    auto *dsUsdTotalLbl = new QLabel(tr("Total Balance:"));
    dsUsdTotalLbl->setFixedWidth(110);
    dsUsdTotalRow->addWidget(dsUsdTotalLbl);
    m_dsUsdTotalBalanceSpin = new ModernDoubleSpinBox();
    m_dsUsdTotalBalanceSpin->setRange(0.01, 999999.0);
    m_dsUsdTotalBalanceSpin->setDecimals(2);
    m_dsUsdTotalBalanceSpin->setValue(AppSettings::instance().dsUsdTotalBalance());
    m_dsUsdTotalBalanceSpin->setSuffix(" USD");
    m_dsUsdTotalBalanceSpin->setMinimumWidth(130);
    dsUsdTotalRow->addWidget(m_dsUsdTotalBalanceSpin);
    dsUsdTotalRow->addStretch();
    dsUsdInner->addLayout(dsUsdTotalRow);

    m_dsUsdRangeSlider = new DoubleRangeSlider(Qt::Horizontal, DoubleRangeSlider::DoubleHandles);
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
    dsUsdColorGrid->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_dsUsdGreenColorBtn = new ColorButton(AppSettings::instance().dsUsdGreenColor(), tr("Green"));
    auto *dsUsdGreenRow = new QHBoxLayout();
    dsUsdGreenRow->addWidget(m_dsUsdGreenColorBtn);
    dsUsdGreenRow->addStretch();
    m_dsUsdGreenLabel = new QLabel();
    m_dsUsdGreenLabel->setFixedWidth(130);
    dsUsdColorGrid->addRow(m_dsUsdGreenLabel, dsUsdGreenRow);

    m_dsUsdYellowColorBtn = new ColorButton(AppSettings::instance().dsUsdYellowColor(), tr("Yellow"));
    auto *dsUsdYellowRow = new QHBoxLayout();
    dsUsdYellowRow->addWidget(m_dsUsdYellowColorBtn);
    dsUsdYellowRow->addStretch();
    m_dsUsdYellowLabel = new QLabel();
    m_dsUsdYellowLabel->setFixedWidth(130);
    dsUsdColorGrid->addRow(m_dsUsdYellowLabel, dsUsdYellowRow);

    m_dsUsdRedColorBtn = new ColorButton(AppSettings::instance().dsUsdRedColor(), tr("Red"));
    auto *dsUsdRedRow = new QHBoxLayout();
    dsUsdRedRow->addWidget(m_dsUsdRedColorBtn);
    dsUsdRedRow->addStretch();
    m_dsUsdRedLabel = new QLabel();
    m_dsUsdRedLabel->setFixedWidth(130);
    dsUsdColorGrid->addRow(m_dsUsdRedLabel, dsUsdRedRow);

    dsUsdInner->addLayout(dsUsdColorGrid);
    dsUsdGroup->addLayout(dsUsdInner);
    dsUsdLayout->addWidget(dsUsdGroup);
    form->addWidget(m_dsUsdWidget);

    m_dsUsdWidget->setVisible(m_dsShowUSDCheck->isChecked());

    form->addStretch();

    connect(m_glmRangeSlider, &DoubleRangeSlider::lowerValueChanged, this, [this](int) { updateGlmLabels(); });
    connect(m_glmRangeSlider, &DoubleRangeSlider::upperValueChanged, this, [this](int) { updateGlmLabels(); });
    connect(m_dsRangeSlider, &DoubleRangeSlider::lowerValueChanged, this, [this](int) { updateDsLabels(); });
    connect(m_dsRangeSlider, &DoubleRangeSlider::upperValueChanged, this, [this](int) { updateDsLabels(); });
    connect(m_dsTotalBalanceSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this](double) { updateDsLabels(); });
    connect(m_dsUsdRangeSlider, &DoubleRangeSlider::lowerValueChanged, this, [this](int) { updateDsUsdLabels(); });
    connect(m_dsUsdRangeSlider, &DoubleRangeSlider::upperValueChanged, this, [this](int) { updateDsUsdLabels(); });
    connect(m_dsUsdTotalBalanceSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [this](double) { updateDsUsdLabels(); });
    connect(m_dsShowUSDCheck, &ToggleSwitch::toggled, m_dsUsdWidget, &QWidget::setVisible);

    updateGlmLabels();
    updateDsLabels();
    updateDsUsdLabels();

    connect(m_ballSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BallTab::applyPreview);
    connect(m_ballBgOpacitySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BallTab::applyPreview);
    connect(m_ringWidthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BallTab::applyPreview);
    connect(m_pctFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BallTab::applyPreview);
    connect(m_timeFontSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BallTab::applyPreview);
    connect(m_pctColorBtn, &ColorButton::colorChanged, this, &BallTab::applyPreview);
    connect(m_timeColorBtn, &ColorButton::colorChanged, this, &BallTab::applyPreview);
    connect(m_ballBgColorBtn, &ColorButton::colorChanged, this, &BallTab::applyPreview);

    scroll->setWidget(page);
    mainLayout->addWidget(scroll);
}

void BallTab::applyPreview()
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
    emit previewBallChanged();
}

void BallTab::updateGlmLabels()
{
    int low = m_glmRangeSlider->GetLowerValue();
    int high = m_glmRangeSlider->GetUpperValue();
    m_glmGreenLabel->setText(tr("0 ~ %1%:").arg(low));
    m_glmYellowLabel->setText(tr("%1% ~ %2%:").arg(low).arg(high));
    m_glmRedLabel->setText(tr("> %1%:").arg(high));
}

void BallTab::updateDsLabels()
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

void BallTab::updateDsUsdLabels()
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

void BallTab::reload()
{
    const AppSettings &s = AppSettings::instance();

    m_ballSizeSpin->blockSignals(true);
    m_ballBgOpacitySpin->blockSignals(true);
    m_ringWidthSpin->blockSignals(true);
    m_pctFontSpin->blockSignals(true);
    m_timeFontSpin->blockSignals(true);
    m_dsTotalBalanceSpin->blockSignals(true);
    m_dsUsdTotalBalanceSpin->blockSignals(true);
    m_dsShowUSDCheck->blockSignals(true);

    m_ballSizeSpin->setValue(s.ballSize());
    m_ballBgOpacitySpin->setValue(s.ballBgOpacity());
    m_ringWidthSpin->setValue(s.ringWidth());
    m_pctFontSpin->setValue(s.pctFontSize());
    m_timeFontSpin->setValue(s.timeFontSize());
    m_pctColorBtn->setChosenColor(s.pctColor());
    m_timeColorBtn->setChosenColor(s.timeColor());
    m_ballBgColorBtn->setChosenColor(s.ballBgColor());

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

    m_ballSizeSpin->blockSignals(false);
    m_ballBgOpacitySpin->blockSignals(false);
    m_ringWidthSpin->blockSignals(false);
    m_pctFontSpin->blockSignals(false);
    m_timeFontSpin->blockSignals(false);
    m_dsTotalBalanceSpin->blockSignals(false);
    m_dsUsdTotalBalanceSpin->blockSignals(false);
    m_dsShowUSDCheck->blockSignals(false);
}

void BallTab::applySettings()
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
}
