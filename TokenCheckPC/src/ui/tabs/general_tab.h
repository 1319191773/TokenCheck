#ifndef GENERAL_TAB_H
#define GENERAL_TAB_H

#include <QWidget>

class ToggleSwitch;
class QLabel;
class QLineEdit;
class HotkeyButton;
class ModernComboBox;
class ModernSpinBox;

class GeneralTab : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralTab(QWidget *parent = nullptr);
    void reload();
    void applySettings();

private:
    ModernComboBox *m_themeCombo;
    ModernComboBox *m_langCombo;
    ModernSpinBox *m_intervalSpin;
    ToggleSwitch *m_autoStartCheck;
    ModernSpinBox *m_notifySpin;
    HotkeyButton *m_toggleHotkeyBtn;
    HotkeyButton *m_refreshHotkeyBtn;
    ModernComboBox *m_proxyTypeCombo;
    QLineEdit *m_proxyHostEdit;
    ModernSpinBox *m_proxyPortSpin;
};

#endif // GENERAL_TAB_H
