#ifndef MODERNCOMBOBOX_H
#define MODERNCOMBOBOX_H

#include <QPushButton>
#include <QVariant>

class QMenu;
class QPropertyAnimation;

class ModernComboBox : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int arrowRotation READ arrowRotation WRITE setArrowRotation)
public:
    explicit ModernComboBox(QWidget *parent = nullptr);
    void addItem(const QString &text, const QVariant &userData = QVariant());
    void setCurrentIndex(int index);
    int currentIndex() const;
    QString currentText() const;
    QVariant currentData() const;
    int findData(const QVariant &data) const;

signals:
    void currentIndexChanged(int index);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onMenuAboutToShow();
    void onMenuAboutToHide();
    void onActionTriggered(QAction *action);

private:
    int arrowRotation() const;
    void setArrowRotation(int rotation);

    QMenu *m_menu;
    QList<QVariant> m_itemData;
    int m_currentIndex;
    int m_arrowRotation;
    QPropertyAnimation *m_anim;
};

#endif // MODERNCOMBOBOX_H
