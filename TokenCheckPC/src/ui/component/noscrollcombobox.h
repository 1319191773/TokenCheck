#ifndef NOSCROLLCOMBOBOX_H
#define NOSCROLLCOMBOBOX_H

#include <QComboBox>
#include <QWheelEvent>

class NoScrollComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit NoScrollComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}

protected:
    void wheelEvent(QWheelEvent *event) override { event->ignore(); }
};

#endif
