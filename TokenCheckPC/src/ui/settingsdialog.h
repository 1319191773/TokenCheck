#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QColor>
#include "platformconfig.h"

class QCheckBox;
class QListWidget;
class QStackedWidget;
class QLabel;
class ColorButton;
class HotkeyButton;

class DoubleRangeSlider;
class AccountTab;
class BallTab;
class GeneralTab;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    void reloadAll();

signals:
    void settingsApplied();
    void previewBallChanged();

protected:
    void showEvent(QShowEvent *event) override;
    void reject() override;

private:
    void applySettings();

    QListWidget *m_navList;
    QStackedWidget *m_stackedWidget;
    AccountTab *m_accountTab;
    BallTab *m_ballTab;
    GeneralTab *m_generalTab;

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
