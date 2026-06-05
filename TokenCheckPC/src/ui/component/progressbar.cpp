#include "progressbar.h"
#include <QPainter>

Progressbar::Progressbar(QWidget *parent) : QWidget(parent) { setFixedHeight(6); }

void Progressbar::setPercentage(double p) { m_pct = p; update(); }
void Progressbar::setColor(const QColor &c) { m_color = c; m_customColor = true; update(); }

void Progressbar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int r = height() / 2;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#45475A"));
    p.drawRoundedRect(rect(), r, r);
    if (m_pct >= 0) {
        int w = static_cast<int>(width() * qBound(0.0, m_pct / 100.0, 1.0));
        if (w > 0) {
            QColor c = m_customColor ? m_color : defaultColor(m_pct);
            p.setBrush(c);
            p.drawRoundedRect(0, 0, w, height(), r, r);
        }
    }
}

QColor Progressbar::defaultColor(double pct)
{
    if (pct < 50) return QColor("#2ECC71");
    if (pct < 80) return QColor("#F1C40F");
    return QColor("#E74C3C");
}
