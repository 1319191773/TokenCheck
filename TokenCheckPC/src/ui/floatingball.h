#ifndef FLOATINGBALL_H
#define FLOATINGBALL_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QPropertyAnimation>
#include "usagequery.h"

class DataManager;

class FloatingBall : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingBall(QWidget *parent = nullptr);
    ~FloatingBall();

    void setStatus(const QString &status);
    void applyBallSize();
    void setBallVisible(bool visible);
    bool isBallVisible() const;

signals:
    void singleClicked();
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onSingleClickTimeout();
    void onPulseTick();
    void onSnapCheck();
    void onLoadingTick();
    void onAccountChanged(const QString &name);
    void onAllDataUpdated();

private:
    void startPulse(bool success);
    void startLoading();
    void stopLoading();
    void switchAccount(int delta);
    void checkSnapToEdge();
    void snapToEdge();
    QRect screenRect() const;
    void animateTo(const QPoint &target);

    void paintGlmAccount(QPainter &p, int side, const UsageData &data);
    void paintDeepSeekAccount(QPainter &p, int side, const UsageData &data);
    void paintLoadingArc(QPainter &p, int side);

    DataManager *m_dm;

    QPoint m_dragPos;
    bool m_dragging;
    bool m_moved;
    bool m_snapped;
    bool m_hovering;
    int m_snapEdge;

    QString m_status;

    int m_ballSize;
    QTimer *m_clickTimer;
    bool m_pendingSingleClick;
    QTimer *m_pulseTimer;
    QTimer *m_snapCheckTimer;
    qreal m_glowOpacity;
    int m_pulseStep;
    int m_pulseMaxSteps;

    QTimer *m_loadingTimer;
    qreal m_loadingAngle;

    QPropertyAnimation *m_snapAnim;
    bool m_animating;

    static const int SNAP_THRESHOLD = 20;
};

#endif
