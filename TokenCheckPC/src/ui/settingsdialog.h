#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include "platformconfig.h"

class QLineEdit;
class QCheckBox;
class QTabWidget;
class QLabel;
class ColorButton;
class HotkeyButton;
class NoScrollComboBox;
class NoScrollSpinBox;
class NoScrollDoubleSpinBox;
class myDoubleSlider;

class QuickAddDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddDialog(QWidget *parent = nullptr);
    PlatformConfig getConfig() const;

private:
    QLineEdit *m_nameEdit;
    NoScrollComboBox *m_typeCombo;
    QLineEdit *m_tokenEdit;
};

class AccountEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AccountEditDialog(const PlatformConfig &config, QWidget *parent = nullptr);
    PlatformConfig getConfig() const;

private:
    QLineEdit *m_nameEdit;
    NoScrollComboBox *m_typeCombo;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tokenEdit;
    QLineEdit *m_prefixEdit;
    QCheckBox *m_enabledCheck;
    QPushButton *m_tokenToggle;

    void onTypeChanged(int index);
    void toggleTokenVisibility();
};

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    void reloadAll();

signals:
    void settingsApplied();
    void previewBallChanged();

private slots:
    void onQuickAdd();
    void onEditAccount();
    void onRemoveAccount();

protected:
    void showEvent(QShowEvent *event) override;
    void reject() override;

private:
    QWidget *createAccountsPage();
    QWidget *createBallPage();
    QWidget *createGeneralPage();
    void refreshAccountList();
    void updateGlmLabels();
    void updateDsLabels();
    void updateDsUsdLabels();
    void applySettings();
    void reloadSettings();

    QTabWidget *m_tabs;

    QListWidget *m_accountList;

    NoScrollSpinBox *m_ballSizeSpin;
    NoScrollSpinBox *m_ballBgOpacitySpin;
    NoScrollSpinBox *m_ringWidthSpin;
    NoScrollSpinBox *m_pctFontSpin;
    NoScrollSpinBox *m_timeFontSpin;
    ColorButton *m_pctColorBtn;
    ColorButton *m_timeColorBtn;
    ColorButton *m_ballBgColorBtn;
    QColor m_chosenPctColor;
    QColor m_chosenTimeColor;

    myDoubleSlider *m_glmRangeSlider;
    QLabel *m_glmGreenLabel;
    QLabel *m_glmYellowLabel;
    QLabel *m_glmRedLabel;
    ColorButton *m_glmGreenColorBtn;
    ColorButton *m_glmYellowColorBtn;
    ColorButton *m_glmRedColorBtn;

    myDoubleSlider *m_dsRangeSlider;
    QLabel *m_dsGreenLabel;
    QLabel *m_dsYellowLabel;
    QLabel *m_dsRedLabel;
    NoScrollDoubleSpinBox *m_dsTotalBalanceSpin;
    ColorButton *m_dsGreenColorBtn;
    ColorButton *m_dsYellowColorBtn;
    ColorButton *m_dsRedColorBtn;

    QCheckBox *m_dsShowUSDCheck;
    QWidget *m_dsUsdWidget;
    NoScrollDoubleSpinBox *m_dsUsdTotalBalanceSpin;
    myDoubleSlider *m_dsUsdRangeSlider;
    QLabel *m_dsUsdGreenLabel;
    QLabel *m_dsUsdYellowLabel;
    QLabel *m_dsUsdRedLabel;
    ColorButton *m_dsUsdGreenColorBtn;
    ColorButton *m_dsUsdYellowColorBtn;
    ColorButton *m_dsUsdRedColorBtn;

    NoScrollComboBox *m_themeCombo;
    NoScrollComboBox *m_langCombo;
    NoScrollSpinBox *m_intervalSpin;
    QCheckBox *m_autoStartCheck;
    NoScrollSpinBox *m_notifySpin;
    HotkeyButton *m_toggleHotkeyBtn;
    HotkeyButton *m_refreshHotkeyBtn;
    NoScrollComboBox *m_proxyTypeCombo;
    QLineEdit *m_proxyHostEdit;
    NoScrollSpinBox *m_proxyPortSpin;

    struct BallSnapshot {
        int ballSize;
        int ballBgOpacity;
        int ringWidth;
        int pctFontSize;
        int timeFontSize;
        QColor pctColor;
        QColor timeColor;
        QColor ballBgColor;
    } m_ballSnapshot;
};

#endif
