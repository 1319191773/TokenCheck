#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include "platformconfig.h"
#include "usagequery.h"

class DataManager;
class QTabWidget;
class QLineEdit;
class HotkeyButton;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void syncWithDataManager();
    void showTab(int index);

signals:
    void refreshRequested();
    void settingsApplied();

public slots:
    void onAllDataUpdated();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAccountChanged(const QString &name);
    void onComboBoxChanged(int index);
    void onTabChanged(int index);
    void markDirty();
    void onQuickAdd();
    void onEditAccount();
    void onRemoveAccount();

private:
    void setupUI();
    QWidget *createDetailTab();
    QWidget *createAccountsTab();
    QWidget *createAppearanceTab();
    QWidget *createGeneralTab();
    void displayData(const UsageData &data);
    void displayEmpty();
    QWidget *createQuotaSection();
    QWidget *createSummarySection();
    QWidget *createModelSection();
    QWidget *createToolSection();
    QString formatTokens(qint64 n) const;
    void populateAccounts();
    void refreshAccountList();
    void applySettings();
    void reloadSettings();
    bool checkSaveOnLeave();

    DataManager *m_dm;
    QTabWidget *m_tabs;
    bool m_settingsDirty = false;
    int m_previousTab = -1;

    QComboBox *m_accountCombo;
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

    QListWidget *m_accountList;
    QComboBox *m_themeCombo;
    QComboBox *m_langCombo;
    QSpinBox *m_intervalSpin;
    QSpinBox *m_ballSizeSpin;
    QSpinBox *m_timeFontSpin;
    QSpinBox *m_pctFontSpin;
    QPushButton *m_timeColorBtn;
    QPushButton *m_pctColorBtn;
    QCheckBox *m_autoStartCheck;
    QSpinBox *m_notifySpin;
    HotkeyButton *m_toggleHotkeyBtn;
    HotkeyButton *m_refreshHotkeyBtn;
    QComboBox *m_proxyTypeCombo;
    QLineEdit *m_proxyHostEdit;
    QSpinBox *m_proxyPortSpin;
    QColor m_chosenTimeColor;
    QColor m_chosenPctColor;
};

#endif
