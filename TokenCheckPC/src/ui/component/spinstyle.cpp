#include "spinstyle.h"
#include <QStyleOptionSpinBox>
#include <QPainter>
#include <QSpinBox>

SpinBoxStyle::SpinBoxStyle(QStyle *base)
    : QProxyStyle(base)
{
}

void SpinBoxStyle::drawComplexControl(ComplexControl cc,
                                      const QStyleOptionComplex *opt,
                                      QPainter *p, const QWidget *w) const
{
    if (cc != QStyle::CC_SpinBox) {
        QProxyStyle::drawComplexControl(cc, opt, p, w);
        return;
    }

    QProxyStyle::drawComplexControl(cc, opt, p, w);

    const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
    if (!sb)
        return;

    QPalette pal = sb->palette;
    QColor arrowColor = pal.color(QPalette::Text);

    if (sb->subControls & QStyle::SC_SpinBoxUp) {
        QRect upRect = subControlRect(cc, sb, SC_SpinBoxUp, w);
        if (upRect.isValid()) {
            int cx = upRect.center().x();
            int cy = upRect.center().y();
            QPolygon upArrow;
            upArrow << QPoint(cx - 4, cy + 2)
                    << QPoint(cx + 4, cy + 2)
                    << QPoint(cx, cy - 3);
            p->save();
            p->setRenderHint(QPainter::Antialiasing);
            p->setBrush(arrowColor);
            p->setPen(Qt::NoPen);
            p->drawPolygon(upArrow);
            p->restore();
        }
    }

    if (sb->subControls & QStyle::SC_SpinBoxDown) {
        QRect downRect = subControlRect(cc, sb, SC_SpinBoxDown, w);
        if (downRect.isValid()) {
            int cx = downRect.center().x();
            int cy = downRect.center().y();
            QPolygon downArrow;
            downArrow << QPoint(cx - 4, cy - 2)
                      << QPoint(cx + 4, cy - 2)
                      << QPoint(cx, cy + 3);
            p->save();
            p->setRenderHint(QPainter::Antialiasing);
            p->setBrush(arrowColor);
            p->setPen(Qt::NoPen);
            p->drawPolygon(downArrow);
            p->restore();
        }
    }
}
