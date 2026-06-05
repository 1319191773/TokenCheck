#ifndef SPINSTYLE_H
#define SPINSTYLE_H

#include <QProxyStyle>
#include <QStyleOptionComplex>

class SpinBoxStyle : public QProxyStyle
{
public:
    explicit SpinBoxStyle(QStyle *base = nullptr);

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                            QPainter *p, const QWidget *w) const override;
};

#endif
