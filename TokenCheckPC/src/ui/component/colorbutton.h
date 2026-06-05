#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include <QColor>

class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(const QColor &initial = QColor(), const QString &defaultLabel = tr("Default"), QWidget *parent = nullptr);

    QColor chosenColor() const;
    void setChosenColor(const QColor &c);
    void setDefaultLabel(const QString &label);

signals:
    void colorChanged(const QColor &color);

private slots:
    void pickColor();

private:
    QColor m_color;
    QString m_defaultLabel;
    void updateAppearance();
};

#endif
