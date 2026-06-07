#pragma once
#include "noscrollspinbox.h"

class ModernSpinBox : public NoScrollSpinBox {
    Q_OBJECT
public:
    explicit ModernSpinBox(QWidget *parent = nullptr);
};

class ModernDoubleSpinBox : public NoScrollDoubleSpinBox {
    Q_OBJECT
public:
    explicit ModernDoubleSpinBox(QWidget *parent = nullptr);
};
