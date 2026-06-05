#ifndef HOTKEYBUTTON_H
#define HOTKEYBUTTON_H

#include <QPushButton>

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

#endif
