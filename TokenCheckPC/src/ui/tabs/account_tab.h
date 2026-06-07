#ifndef ACCOUNT_TAB_H
#define ACCOUNT_TAB_H

#include <QWidget>

class QListWidget;

class AccountTab : public QWidget
{
    Q_OBJECT
public:
    explicit AccountTab(QWidget *parent = nullptr);
    void reload();

private slots:
    void onQuickAdd();
    void onEditAccount();
    void onRemoveAccount();

private:
    void refreshAccountList();

    QListWidget *m_accountList;
};

#endif // ACCOUNT_TAB_H
