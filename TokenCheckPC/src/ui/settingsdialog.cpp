#include "settingsdialog.h"
#include "tabs/account_tab.h"
#include "tabs/ball_tab.h"
#include "tabs/general_tab.h"
#include "quickadddialog.h"
#include "accounteditdialog.h"
#include "appsettings.h"
#include "platform_registry.h"
#include "theme.h"
#include "component/colorbutton.h"
#include "component/hotkeybutton.h"
#include "component/colorbutton.h"
#include "component/hotkeybutton.h"
#include "component/doublerangeslider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QFrame>
#include <QShowEvent>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(680, 680);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    m_navList = new QListWidget();
    m_navList->setFixedWidth(160);
    m_navList->setObjectName("settingsNavList");
    
    m_stackedWidget = new QStackedWidget();
    
    m_accountTab = new AccountTab(this);
    m_ballTab = new BallTab(this);
    connect(m_ballTab, &BallTab::previewBallChanged, this, &SettingsDialog::previewBallChanged);
    m_generalTab = new GeneralTab(this);
    
    m_stackedWidget->addWidget(m_accountTab);
    m_stackedWidget->addWidget(m_ballTab);
    m_stackedWidget->addWidget(m_generalTab);
    
    m_navList->addItem(tr("Accounts"));
    m_navList->addItem(tr("Floating Ball"));
    m_navList->addItem(tr("General"));
    
    connect(m_navList, &QListWidget::currentRowChanged, m_stackedWidget, &QStackedWidget::setCurrentIndex);
    m_navList->setCurrentRow(0);
    
    auto *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setProperty("class", "v-separator");
    
    contentLayout->addWidget(m_navList);
    contentLayout->addWidget(separator);
    contentLayout->addWidget(m_stackedWidget, 1);
    
    mainLayout->addLayout(contentLayout, 1);

    auto *bottomBar = new QHBoxLayout();
    bottomBar->setContentsMargins(16, 16, 16, 16);
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

void SettingsDialog::reloadAll() { m_generalTab->reload(); m_accountTab->reload(); m_ballTab->reload(); }

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









void SettingsDialog::applySettings()
{
    m_ballTab->applySettings();
    m_generalTab->applySettings();

    emit settingsApplied();
}
