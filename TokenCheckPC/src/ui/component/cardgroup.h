#ifndef CARDGROUP_H
#define CARDGROUP_H

#include <QWidget>

class QVBoxLayout;
class QLabel;
class ToggleSwitch;
class QPropertyAnimation;

class CardGroup : public QWidget
{
    Q_OBJECT
public:
    explicit CardGroup(const QString &title, QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void addLayout(QLayout *layout);

    ToggleSwitch* addToggleSwitch();

    void setExpanded(bool expanded);
    bool isExpanded() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QWidget *m_header;
    QLabel *m_titleLabel;
    QLabel *m_iconLabel;
    ToggleSwitch *m_toggle;
    
    QWidget *m_contentContainer;
    QVBoxLayout *m_contentLayout;

    bool m_expanded;
    QPropertyAnimation *m_anim;
};

#endif // CARDGROUP_H
