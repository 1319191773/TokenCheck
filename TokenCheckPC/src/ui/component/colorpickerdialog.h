#ifndef COLORPICKERDIALOG_H
#define COLORPICKERDIALOG_H

#include <QDialog>
#include <QColor>

class StudioColorPalette;

class ColorPickerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ColorPickerDialog(const QColor &initial, QWidget *parent = nullptr);

    QColor selectedColor() const;
    void setOldColor(const QColor &color);

private:
    StudioColorPalette *m_palette;
    QColor m_selectedColor;
};

#endif
