#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QColor>

class Progressbar : public QWidget
{
public:
    explicit Progressbar(QWidget *parent = nullptr);
    void setPercentage(double p);
    void setColor(const QColor &c);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    double m_pct = 0;
    QColor m_color;
    bool m_customColor = false;
    static QColor defaultColor(double pct);
};

#endif
