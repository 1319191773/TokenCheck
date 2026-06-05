#include "mainwindow.h"
#include "settingsdialog.h"
#include "appsettings.h"
#include "datamanager.h"
#include "theme.h"
#include "component/clickablecard.h"
#include "component/progressbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QFrame>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_dm(&DataManager::instance())
{
    setObjectName("MainWindowRoot");
    setWindowTitle("TokenCheck");
    setMinimumSize(520, 640);
    resize(560, 720);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_stack = new QStackedWidget();
    m_stack->addWidget(createCardListPage());
    m_stack->addWidget(createDetailPage());
    mainLayout->addWidget(m_stack);
}

static QString bgName() { return Theme::bg.name(); }
static QString sfName() { return Theme::surface.name(); }
static QString saName() { return Theme::surfaceAlt.name(); }
static QString txName() { return Theme::text.name(); }
static QString tdName() { return Theme::textDim.name(); }
static QString bdName() { return Theme::border.name(); }
static QString acName() { return Theme::accent.name(); }

static QColor pctColorValue(double pct)
{
    if (pct < 0) return Theme::textDim;
    if (pct < 50) return Theme::accent;
    if (pct < 80) return Theme::warning;
    return Theme::danger;
}

static QString pctColor(double pct) { return pctColorValue(pct).name(); }
static QString barChunkColor(int pct) { return pctColorValue(pct).name(); }

QWidget *MainWindow::createCardListPage()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *header = new QFrame();
    header->setFixedHeight(64);
    header->setObjectName("listHeader");
    header->setStyleSheet(
        QString("QFrame#listHeader { background-color: %1; border-bottom: 1px solid %2; }").arg(bgName(), bdName()));
    m_listHeader = header;
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 0, 16, 10);

    m_headerTitle = new QLabel("TokenCheck");
    m_headerTitle->setStyleSheet(
        QString("font-size: 20px; font-weight: bold; color: %1;").arg(txName()));
    headerLayout->addWidget(m_headerTitle, 0, Qt::AlignBottom);

    m_headerSub = new QLabel();
    m_headerSub->setStyleSheet(
        QString("font-size: 12px; color: %1;").arg(tdName()));
    headerLayout->addWidget(m_headerSub, 0, Qt::AlignBottom);
    headerLayout->addStretch();

    auto *settingsBtn = new QPushButton(QString::fromUtf8("\xe2\x9a\x99"));
    settingsBtn->setFixedSize(36, 36);
    settingsBtn->setStyleSheet(
        QString("QPushButton { font-size: 18px; color: %1; border: none; border-radius: 8px; padding: 2px; }"
                "QPushButton:hover { background-color: %2; color: %3; }")
            .arg(tdName(), saName(), txName()));
    m_settingsBtn = settingsBtn;
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::settingsRequested);
    headerLayout->addWidget(settingsBtn, 0, Qt::AlignBottom);

    layout->addWidget(header);

    m_cardScroll = new QScrollArea();
    m_cardScroll->setWidgetResizable(true);
    m_cardScroll->setFrameShape(QFrame::NoFrame);
    m_cardScroll->setStyleSheet(
        QString("QScrollArea { background-color: %1; border: none; }").arg(sfName()));

    auto *container = new QWidget();
    container->setStyleSheet(QString("background-color: %1;").arg(sfName()));
    m_cardContainer = container;
    m_cardLayout = new QVBoxLayout(container);
    m_cardLayout->setContentsMargins(20, 16, 20, 16);
    m_cardLayout->setSpacing(12);
    m_cardLayout->addStretch();

    m_cardScroll->setWidget(container);
    layout->addWidget(m_cardScroll, 1);

    auto *bottomBar = new QFrame();
    bottomBar->setFixedHeight(72);
    bottomBar->setObjectName("listBottomBar");
    bottomBar->setStyleSheet(
        QString("QFrame#listBottomBar { background-color: %1; border-top: 1px solid %2; }").arg(bgName(), bdName()));
    m_listBottomBar = bottomBar;
    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(20, 12, 20, 12);

    auto *refreshBtn = new QPushButton(tr("Refresh"));
    refreshBtn->setProperty("class", "primary");
    refreshBtn->setStyleSheet(
        QString("QPushButton { background-color: %1; color: %2; font-size: 15px; "
                "font-weight: bold; border: none; border-radius: 10px; padding: 10px; }"
                "QPushButton:hover { background-color: %3; }"
                "QPushButton:pressed { background-color: %4; }")
            .arg(acName(), bgName(), Theme::accentHover.name(), Theme::accentHover.name()));
    m_refreshBtn = refreshBtn;
    connect(refreshBtn, &QPushButton::clicked, this, [this, refreshBtn]() {
        refreshBtn->setEnabled(false);
        refreshBtn->setText(tr("Refreshing..."));
        refreshBtn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: %2; font-size: 15px; "
                    "font-weight: bold; border: none; border-radius: 10px; padding: 10px; }")
                .arg(bdName(), tdName()));
        emit refreshRequested();
    });
    bottomLayout->addWidget(refreshBtn);

    layout->addWidget(bottomBar);

    return page;
}

QWidget *MainWindow::createDetailPage()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *header = new QFrame();
    header->setFixedHeight(64);
    header->setObjectName("detailHeader");
    header->setStyleSheet(
        QString("QFrame#detailHeader { background-color: %1; border-bottom: 1px solid %2; }").arg(bgName(), bdName()));
    m_detailHeader = header;
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(12, 0, 16, 0);
    headerLayout->setSpacing(8);

    auto *backWidget = new ClickableCard();
    auto *backLayout = new QHBoxLayout(backWidget);
    backLayout->setContentsMargins(8, 6, 12, 6);
    backLayout->setSpacing(6);
    auto *backIcon = new QLabel(QString::fromUtf8("\xe2\x86\x90"));
    backIcon->setStyleSheet(QString("font-size: 18px; color: %1; border: none; background: transparent;").arg(tdName()));
    backLayout->addWidget(backIcon);
    auto *backText = new QLabel(tr("Back"));
    backText->setStyleSheet(QString("font-size: 15px; color: %1; border: none; background: transparent;").arg(tdName()));
    backLayout->addWidget(backText);
    backWidget->setStyleSheet(
        QString("ClickableCard { background: transparent; border: none; border-radius: 8px; }"
                "ClickableCard:hover { background-color: %1; }")
            .arg(saName()));
    m_backWidget = backWidget;
    backWidget->setCursor(Qt::PointingHandCursor);
    backWidget->onClick = [this]() { onBackToList(); };

    headerLayout->addWidget(backWidget);
    headerLayout->addStretch();

    layout->addWidget(header);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(
        QString("QScrollArea { background-color: %1; border: none; }").arg(sfName()));
    m_detailScroll = scroll;
    auto *container = new QWidget();
    container->setStyleSheet(QString("background-color: %1;").arg(sfName()));
    m_detailContainer = container;
    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(14);
    containerLayout->setContentsMargins(20, 16, 20, 16);

    m_statusLabel = new QLabel();
    m_timestampLabel = new QLabel();
    m_timestampLabel->setProperty("class", "dim");
    m_timestampLabel->setStyleSheet("font-size: 11px;");

    auto *headerRow = new QHBoxLayout();
    headerRow->addWidget(m_statusLabel);
    headerRow->addStretch();
    headerRow->addWidget(m_timestampLabel);
    containerLayout->addLayout(headerRow);

    containerLayout->addWidget(createSummarySection());
    containerLayout->addWidget(createQuotaSection());
    containerLayout->addWidget(createModelSection());
    containerLayout->addWidget(createToolSection());
    containerLayout->addStretch();

    scroll->setWidget(container);
    layout->addWidget(scroll, 1);

    return page;
}

void MainWindow::rebuildCards()
{
    while (m_cardLayout->count() > 1) {
        auto *item = m_cardLayout->takeAt(0);
        if (item->widget())
            delete item->widget();
        delete item;
    }

    auto platforms = AppSettings::instance().allPlatforms();
    QStringList names;
    for (const auto &p : platforms)
        names.append(p.name);

    int connected = 0;
    for (const auto &n : names) {
        UsageData d = m_dm->data(n);
        if (d.isValid) connected++;
    }
    m_headerSub->setText(tr("Connected %1/%2").arg(connected).arg(names.size()));

    if (names.isEmpty()) {
        auto *emptyCard = new QFrame();
        emptyCard->setStyleSheet(
            QString("QFrame { background-color: %1; border: 1px solid %2; border-radius: 14px; }")
                .arg(bgName(), bdName()));
        auto *emptyLayout = new QVBoxLayout(emptyCard);
        emptyLayout->setContentsMargins(20, 24, 20, 24);
        emptyLayout->setAlignment(Qt::AlignCenter);

        auto *emptyText = new QLabel(tr("No platform data"));
        emptyText->setStyleSheet(QString("color: %1; font-size: 15px; background: transparent;").arg(tdName()));
        emptyText->setAlignment(Qt::AlignCenter);
        emptyLayout->addWidget(emptyText);

        auto *hintText = new QLabel(tr("Click the gear icon to add a platform"));
        hintText->setStyleSheet(QString("color: %1; font-size: 12px; background: transparent;").arg(tdName()));
        hintText->setAlignment(Qt::AlignCenter);
        emptyLayout->addWidget(hintText);

        m_cardLayout->insertWidget(m_cardLayout->count() - 1, emptyCard);
        return;
    }

    for (const auto &name : names) {
        UsageData d = m_dm->data(name);
        auto *card = new ClickableCard();
        QString bgColor = d.isValid ? bgName() : sfName();
        QString borderColor = d.isValid ? bdName() : Theme::danger.name();
        card->setObjectName("accountCard");
        card->setStyleSheet(
            QString("QFrame#accountCard { background-color: %1; border: 1px solid %2; border-radius: 14px; }"
                    "QFrame#accountCard:hover { border-color: %3; }")
                .arg(bgColor, borderColor, acName()));
        card->setCursor(Qt::PointingHandCursor);

        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(20, 16, 20, 16);
        cardLayout->setSpacing(10);

        auto *topRow = new QHBoxLayout();
        topRow->setSpacing(8);

        auto *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; background: transparent;").arg(txName()));
        topRow->addWidget(nameLabel);
        topRow->addStretch();

        auto *statusBadge = new QLabel(d.isValid ? tr("Connected") : tr("Error"));
        statusBadge->setAlignment(Qt::AlignCenter);
        statusBadge->setStyleSheet(
            d.isValid
                ? QString("background-color: #1A3A2A; color: %1; font-size: 11px; font-weight: bold; "
                          "border-radius: 6px; padding: 2px 10px;").arg(Theme::accent.name())
                : QString("background-color: #3A1A1A; color: %1; font-size: 11px; font-weight: bold; "
                          "border-radius: 6px; padding: 2px 10px;").arg(Theme::danger.name()));
        statusBadge->setFixedHeight(22);
        topRow->addWidget(statusBadge);
        cardLayout->addLayout(topRow);

        if (d.isValid) {
            bool isDeepSeek = (d.platformType == "deepseek");

            if (isDeepSeek) {
                auto *balRow = new QHBoxLayout();
                auto *balLbl = new QLabel(tr("Balance"));
                balLbl->setStyleSheet(QString("font-size: 12px; color: %1; background: transparent;").arg(tdName()));
                balRow->addWidget(balLbl);

                auto *balVal = new QLabel();
                double bal = d.balanceTotal();
                balVal->setStyleSheet(QString("font-size: 15px; font-weight: bold; color: %1; background: transparent;")
                                          .arg(bal > 10 ? Theme::accent.name() : (bal > 1 ? Theme::warning.name() : Theme::danger.name())));
                balVal->setText(bal >= 0 ? QString("%1 %2").arg(bal, 0, 'f', 2).arg(d.balanceCurrency()) : "--");
                balRow->addWidget(balVal);
                balRow->addStretch();
                cardLayout->addLayout(balRow);
            } else {
                auto makeProgressBar = [this](QVBoxLayout *cardLayout, const QString &label, double pct) {
                    auto *row = new QHBoxLayout();
                    row->setSpacing(8);
                    auto *lbl = new QLabel(label);
                    lbl->setStyleSheet(QString("font-size: 12px; color: %1; background: transparent;").arg(tdName()));
                    lbl->setFixedWidth(50);
                    row->addWidget(lbl);

                    auto *bar = new Progressbar();
                    bar->setPercentage(pct);
                    row->addWidget(bar, 1);

                    auto *val = new QLabel(pct >= 0 ? QString("%1%").arg(qRound(pct)) : "--");
                    val->setStyleSheet(QString("font-size: 12px; font-weight: bold; color: %1; background: transparent;").arg(pctColor(pct)));
                    val->setFixedWidth(42);
                    val->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                    row->addWidget(val);
                    cardLayout->addLayout(row);
                };

                makeProgressBar(cardLayout, "Token", d.tokenPercentage());
                makeProgressBar(cardLayout, "MCP", d.mcpPercentage());
            }

            auto *sep = new QFrame();
            sep->setFrameShape(QFrame::HLine);
            sep->setFixedHeight(1);
            sep->setStyleSheet(QString("background-color: %1; border: none;").arg(bdName()));
            cardLayout->addWidget(sep);

            auto *bottomRow = new QHBoxLayout();
            QString extra;
            if (isDeepSeek) {
                extra = tr("View Details");
            } else {
                extra = tr("Reset") + " " + (d.tokenResetTime().isEmpty() ? "--" : d.tokenResetTime());
            }
            auto *extraLbl = new QLabel(extra);
            extraLbl->setStyleSheet(QString("font-size: 12px; color: %1; background: transparent;").arg(tdName()));
            bottomRow->addWidget(extraLbl);
            bottomRow->addStretch();
            cardLayout->addLayout(bottomRow);
        } else {
            auto *errLabel = new QLabel(d.errorMsg);
            errLabel->setStyleSheet(QString("font-size: 12px; color: %1; background: transparent;").arg(Theme::danger.name()));
            errLabel->setWordWrap(true);
            cardLayout->addWidget(errLabel);
        }

        QString capturedName = name;
        card->onClick = [this, capturedName]() {
            showDetail(capturedName);
        };

        m_cardLayout->insertWidget(m_cardLayout->count() - 1, card);
    }
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
            QString("QFrame#statCard {"
                    "  background-color: %1;"
                    "  border: 1px solid %2;"
                    "  border-radius: 8px;"
                    "}").arg(sfName(), bdName()));
        card->setObjectName("statCard");
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

void MainWindow::showCardList()
{
    m_stack->setCurrentIndex(PAGE_LIST);
    show();
    raise();
    activateWindow();
}

void MainWindow::showDetail(const QString &accountName)
{
    m_detailAccount = accountName;
    UsageData d = m_dm->data(accountName);
    displayData(d);
    m_stack->setCurrentIndex(PAGE_DETAIL);
    show();
    raise();
    activateWindow();
}

void MainWindow::onBackToList()
{
    showCardList();
}

void MainWindow::onAllDataUpdated()
{
    m_refreshBtn->setEnabled(true);
    m_refreshBtn->setText(tr("Refresh"));
    m_refreshBtn->setStyleSheet(
        QString("QPushButton { background-color: %1; color: %2; font-size: 15px; "
                "font-weight: bold; border: none; border-radius: 10px; padding: 10px; }"
                "QPushButton:hover { background-color: %3; }"
                "QPushButton:pressed { background-color: %3; }")
            .arg(acName(), bgName(), Theme::accentHover.name()));

    rebuildCards();
    if (m_stack->currentIndex() == PAGE_DETAIL && !m_detailAccount.isEmpty()) {
        UsageData d = m_dm->data(m_detailAccount);
        displayData(d);
    }
}

void MainWindow::displayData(const UsageData &data)
{
    if (!data.isValid) {
        m_statusLabel->setText(QString::fromUtf8("\xe2\x97\x8f ") + data.platformName + ": " + data.errorMsg);
        m_statusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(Theme::danger.name()));
        return;
    }
    m_statusLabel->setText(QString::fromUtf8("\xe2\x97\x8f ") + data.platformName + " [" + data.platformType + "] OK");
    m_statusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(Theme::accent.name()));

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

void MainWindow::refreshTheme()
{
    m_listHeader->setStyleSheet(
        QString("QFrame#listHeader { background-color: %1; border-bottom: 1px solid %2; }").arg(bgName(), bdName()));
    m_listBottomBar->setStyleSheet(
        QString("QFrame#listBottomBar { background-color: %1; border-top: 1px solid %2; }").arg(bgName(), bdName()));
    m_headerTitle->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(txName()));
    m_headerSub->setStyleSheet(QString("font-size: 12px; color: %1;").arg(tdName()));
    m_settingsBtn->setStyleSheet(
        QString("QPushButton { font-size: 18px; color: %1; border: none; border-radius: 8px; padding: 2px; }"
                "QPushButton:hover { background-color: %2; color: %3; }")
            .arg(tdName(), saName(), txName()));
    m_refreshBtn->setStyleSheet(
        QString("QPushButton { background-color: %1; color: %2; font-size: 15px; "
                "font-weight: bold; border: none; border-radius: 10px; padding: 10px; }"
                "QPushButton:hover { background-color: %3; }"
                "QPushButton:pressed { background-color: %3; }")
            .arg(acName(), bgName(), Theme::accentHover.name()));
    m_cardScroll->setStyleSheet(
        QString("QScrollArea { background-color: %1; border: none; }").arg(sfName()));
    m_cardContainer->setStyleSheet(QString("background-color: %1;").arg(sfName()));

    m_detailHeader->setStyleSheet(
        QString("QFrame#detailHeader { background-color: %1; border-bottom: 1px solid %2; }").arg(bgName(), bdName()));
    m_backWidget->setStyleSheet(
        QString("ClickableCard { background: transparent; border: none; border-radius: 8px; }"
                "ClickableCard:hover { background-color: %1; }")
            .arg(saName()));
    m_detailScroll->setStyleSheet(
        QString("QScrollArea { background-color: %1; border: none; }").arg(sfName()));
    m_detailContainer->setStyleSheet(QString("background-color: %1;").arg(sfName()));
    m_timestampLabel->setStyleSheet("font-size: 11px;");

    rebuildCards();
    if (m_stack->currentIndex() == PAGE_DETAIL && !m_detailAccount.isEmpty())
        displayData(m_dm->data(m_detailAccount));
}
