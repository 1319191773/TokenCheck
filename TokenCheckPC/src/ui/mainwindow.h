#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QStackedWidget>
#include "usagequery.h"

class DataManager;
class QVBoxLayout;
class QScrollArea;
class QPushButton;
class ClickableCard;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void showCardList();
    void showDetail(const QString &accountName);

signals:
    void refreshRequested();
    void settingsRequested();

public slots:
    void onAllDataUpdated();
    void refreshTheme();

private slots:
    void onBackToList();

private:
    QWidget *createCardListPage();
    QWidget *createDetailPage();
    void rebuildCards();
    void displayData(const UsageData &data);
    void displayEmpty();

    QWidget *createQuotaSection();
    QWidget *createSummarySection();
    QWidget *createModelSection();
    QWidget *createToolSection();
    QString formatTokens(qint64 n) const;

    DataManager *m_dm;
    QStackedWidget *m_stack;

    static const int PAGE_LIST = 0;
    static const int PAGE_DETAIL = 1;

    QFrame *m_listHeader;
    QFrame *m_listBottomBar;
    QPushButton *m_refreshBtn;
    QPushButton *m_settingsBtn;
    QScrollArea *m_cardScroll;
    QWidget *m_cardContainer;

    QFrame *m_detailHeader;
    ClickableCard *m_backWidget;
    QScrollArea *m_detailScroll;
    QWidget *m_detailContainer;

    QLabel *m_headerTitle;
    QLabel *m_headerSub;
    QVBoxLayout *m_cardLayout;

    QLabel *m_statusLabel;
    QLabel *m_timestampLabel;

    QWidget *m_tokenRow;
    QLabel *m_tokenLabel;
    QProgressBar *m_tokenBar;

    QWidget *m_resetRow;
    QLabel *m_resetLabel;

    QWidget *m_mcpRow;
    QLabel *m_mcpLabel;
    QProgressBar *m_mcpBar;

    QWidget *m_mcpDetailRow;
    QLabel *m_mcpDetailLabel;

    QWidget *m_balanceRow;
    QLabel *m_balanceLabel;

    QLabel *m_totalTokensLabel;
    QLabel *m_totalRequestsLabel;
    QLabel *m_modelCountLabel;
    QLabel *m_toolCountLabel;

    QTableWidget *m_modelTable;
    QTableWidget *m_toolTable;

    QString m_detailAccount;
};

#endif
