#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QWidget>
#include <QPropertyAnimation>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);
    bool isChecked() const;
    void setChecked(bool checked);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    int offset() const;
    void setOffset(int value);

    bool m_checked;
    bool m_hovered;
    int m_offset;
    QPropertyAnimation *m_anim;
};

#endif // TOGGLESWITCH_H
