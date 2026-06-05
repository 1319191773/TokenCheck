#include "colorbutton.h"
#include <QColorDialog>

ColorButton::ColorButton(const QColor &initial, const QString &defaultLabel, QWidget *parent)
    : QPushButton(parent), m_color(initial), m_defaultLabel(defaultLabel)
{
    setFixedSize(80, 28);
    updateAppearance();
    connect(this, &QPushButton::clicked, this, &ColorButton::pickColor);
}

QColor ColorButton::chosenColor() const { return m_color; }

void ColorButton::setChosenColor(const QColor &c) { m_color = c; updateAppearance(); }
void ColorButton::setDefaultLabel(const QString &label) { m_defaultLabel = label; updateAppearance(); }

void ColorButton::pickColor()
{
    QColor initial = m_color.isValid() ? m_color : Qt::white;
    QColor c = QColorDialog::getColor(initial, this, tr("Choose Color"));
    if (c.isValid()) {
        m_color = c;
        updateAppearance();
        emit colorChanged(c);
    }
}

void ColorButton::updateAppearance()
{
    if (m_color.isValid()) {
        setText(m_color.name());
        setStyleSheet(QString("background-color: %1; color: #000; border-radius: 6px; font-size: 11px;").arg(m_color.name()));
    } else {
        setText(m_defaultLabel);
        setStyleSheet(QString());
    }
}
