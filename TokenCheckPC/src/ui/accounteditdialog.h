#ifndef ACCOUNTEDITDIALOG_H
#define ACCOUNTEDITDIALOG_H

#include <QDialog>
#include "platformconfig.h"

class QLineEdit;
class QCheckBox;
class QPushButton;
class NoScrollComboBox;
class ToggleSwitch;

class AccountEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AccountEditDialog(const PlatformConfig &config, QWidget *parent = nullptr);
    PlatformConfig getConfig() const;

private:
    void onTypeChanged(int index);
    void toggleTokenVisibility();

    QLineEdit *m_nameEdit;
    NoScrollComboBox *m_typeCombo;
    QLineEdit *m_urlEdit;
    QLineEdit *m_tokenEdit;
    QLineEdit *m_prefixEdit;
    ToggleSwitch *m_enabledCheck;
    QPushButton *m_tokenToggle;
};

#endif
