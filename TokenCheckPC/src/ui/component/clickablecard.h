#ifndef CLICKABLECARD_H
#define CLICKABLECARD_H

#include <QFrame>
#include <functional>

class ClickableCard : public QFrame
{
public:
    explicit ClickableCard(QWidget *parent = nullptr);
    std::function<void()> onClick;

protected:
    void mousePressEvent(QMouseEvent *) override;
};

#endif
