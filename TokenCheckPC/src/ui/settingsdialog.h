#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include "platformconfig.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;

class HotkeyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit HotkeyButton(QWidget *parent = nullptr);
    void setKeySequence(const QString &seq);
    QString keySequence() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_capturing = false;
    QString m_sequence;
    void updateDisplay();
};

class QuickAddDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddDialog(QWidget *parent = nullptr);
    PlatformConfig getConfig() const;

private:
    QLineEdit *m_nameEdit;
    QComboBox *m_typeCombo;
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
    QComboBox *m_typeCombo;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tokenEdit;
    QLineEdit *m_prefixEdit;
    QCheckBox *m_enabledCheck;
    QPushButton *m_tokenToggle;

    void onTypeChanged(int index);
    void toggleTokenVisibility();
};

#endif
