#ifndef NOSCROLLSPINBOX_H
#define NOSCROLLSPINBOX_H

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWheelEvent>

class NoScrollSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit NoScrollSpinBox(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        if (hasFocus())
            QSpinBox::wheelEvent(event);
        else
            event->ignore();
    }
};

class NoScrollDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit NoScrollDoubleSpinBox(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        if (hasFocus())
            QDoubleSpinBox::wheelEvent(event);
        else
            event->ignore();
    }
};

#endif
