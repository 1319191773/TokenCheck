#ifndef DOUBLERANGESLIDER_H
#define DOUBLERANGESLIDER_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class DoubleRangeSlider : public QWidget
{
    Q_OBJECT

public:
    enum Option {
        NoHandle = 0x0,
        LeftHandle = 0x1,
        RightHandle = 0x2,
        DoubleHandles = LeftHandle | RightHandle
    };
    Q_DECLARE_FLAGS(Options, Option)

    DoubleRangeSlider(QWidget *aParent = nullptr);
    DoubleRangeSlider(Qt::Orientation ori, Options t = DoubleHandles, QWidget *aParent = nullptr);

    QSize minimumSizeHint() const override;

    int minimum() const;
    int maximum() const;
    int lowerValue() const;
    int upperValue() const;
    void setRange(int aMinimum, int aMaximum);
    void setTracking(bool enable);

    int GetLowerValue() const { return lowerValue(); }
    int GetUpperValue() const { return upperValue(); }
    void SetLowerValue(int v) { setLowerValue(v); }
    void SetUpperValue(int v) { setUpperValue(v); }
    void SetRange(int lo, int hi) { setRange(lo, hi); }

protected:
    void paintEvent(QPaintEvent *aEvent) override;
    void mousePressEvent(QMouseEvent *aEvent) override;
    void mouseMoveEvent(QMouseEvent *aEvent) override;
    void mouseReleaseEvent(QMouseEvent *aEvent) override;
    void changeEvent(QEvent *aEvent) override;

    QRectF firstHandleRect() const;
    QRectF secondHandleRect() const;
    QRectF handleRect(int aValue) const;

signals:
    void lowerValueChanged(int aLowerValue);
    void upperValueChanged(int aUpperValue);
    void rangeChanged(int aMin, int aMax);

public slots:
    void setLowerValue(int aLowerValue);
    void setUpperValue(int aUpperValue);
    void setMinimum(int aMinimum);
    void setMaximum(int aMaximum);

private:
    Q_DISABLE_COPY(DoubleRangeSlider)
    float currentPercentage();
    int validLength() const;

    int mMinimum;
    int mMaximum;
    int mLowerValue;
    int mUpperValue;
    bool mEnableTracking;
    bool mFirstHandlePressed;
    bool mSecondHandlePressed;
    int mInterval;
    int mDelta;
    QColor mBackgroudColorEnabled;
    QColor mBackgroudColorDisabled;
    QColor mBackgroudColor;
    Qt::Orientation orientation;
    Options type;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DoubleRangeSlider::Options)

#endif
