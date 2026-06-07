#include "hotkeybutton.h"
#include <QKeyEvent>
#include "../theme.h"

HotkeyButton::HotkeyButton(QWidget *parent) : QPushButton(parent)
{
    setMinimumWidth(140);
    setFixedHeight(32);
    setFocusPolicy(Qt::StrongFocus);
}

void HotkeyButton::setKeySequence(const QString &seq)
{
    m_sequence = seq;
    m_capturing = false;
    updateDisplay();
}

QString HotkeyButton::keySequence() const { return m_sequence; }

void HotkeyButton::updateDisplay()
{
    if (m_capturing) {
        setText(tr("Press keys..."));
        setStyleSheet(QString("border: 1px solid %1; color: %1; font-weight: bold; background-color: transparent;").arg(Theme::warning.name()));
    } else if (m_sequence.isEmpty()) {
        setText(tr("(None)"));
        setStyleSheet(QString());
    } else {
        setText(m_sequence);
        setStyleSheet(QString("color: %1; font-weight: bold; background-color: transparent; border: 1px solid %2;").arg(Theme::accent.name(), Theme::border.name()));
    }
}

void HotkeyButton::keyPressEvent(QKeyEvent *event)
{
    if (!m_capturing) {
        QPushButton::keyPressEvent(event);
        return;
    }
    if (event->key() == Qt::Key_Escape) { m_capturing = false; updateDisplay(); return; }
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        m_sequence.clear(); m_capturing = false; updateDisplay(); return;
    }
    Qt::KeyboardModifiers mods = event->modifiers();
    int key = event->key();
    if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta)
        return;
    QStringList parts;
    if (mods & Qt::ControlModifier) parts << "Ctrl";
    if (mods & Qt::ShiftModifier) parts << "Shift";
    if (mods & Qt::AltModifier) parts << "Alt";
    if (key >= Qt::Key_A && key <= Qt::Key_Z) parts << QChar(key).toUpper();
    else parts << QKeySequence(key).toString();
    if (parts.size() >= 2) { m_sequence = parts.join("+"); m_capturing = false; updateDisplay(); }
}

void HotkeyButton::focusOutEvent(QFocusEvent *event) { m_capturing = false; updateDisplay(); QPushButton::focusOutEvent(event); }
void HotkeyButton::mousePressEvent(QMouseEvent *event) { if (!m_capturing) { m_capturing = true; updateDisplay(); setFocus(); } QPushButton::mousePressEvent(event); }
