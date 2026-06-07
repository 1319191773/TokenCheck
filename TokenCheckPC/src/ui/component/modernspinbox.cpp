#include "modernspinbox.h"

ModernSpinBox::ModernSpinBox(QWidget *parent) : NoScrollSpinBox(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

ModernDoubleSpinBox::ModernDoubleSpinBox(QWidget *parent) : NoScrollDoubleSpinBox(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}
