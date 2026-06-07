#include "colorpickerdialog.h"
#include "studiocolorpalette.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

ColorPickerDialog::ColorPickerDialog(const QColor &initial, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Choose Color"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    m_palette = new StudioColorPalette(this);
    m_palette->setColor(initial);
    mainLayout->addWidget(m_palette);

    QColor bg = Theme::bg;
    QColor bd = Theme::border;
    setStyleSheet(
        QString("QDialog { background-color: %1; }")
            .arg(bg.name()));

    connect(m_palette, &StudioColorPalette::colorApplied, this, [this](const QColor &color) {
        m_selectedColor = color;
        accept();
    });
}

QColor ColorPickerDialog::selectedColor() const
{
    return m_selectedColor.isValid() ? m_selectedColor : m_palette->color();
}

void ColorPickerDialog::setOldColor(const QColor &color)
{
    m_palette->setOldColor(color);
}
