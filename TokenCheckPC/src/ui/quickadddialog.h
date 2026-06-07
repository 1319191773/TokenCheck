#ifndef QUICKADDDIALOG_H
#define QUICKADDDIALOG_H

#include <QDialog>
#include "platformconfig.h"

class QLineEdit;
class NoScrollComboBox;

class QuickAddDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddDialog(QWidget *parent = nullptr);
    PlatformConfig getConfig() const;

private:
    QLineEdit *m_nameEdit;
    NoScrollComboBox *m_typeCombo;
    QLineEdit *m_tokenEdit;
};

#endif
