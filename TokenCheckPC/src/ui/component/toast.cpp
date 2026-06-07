#include "toast.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include "../theme.h"

void Toast::show(QWidget *parent, const QString &message, bool isError, int durationMs)
{
    if (!parent) return;
    Toast *t = new Toast(parent, message, isError, durationMs);
    t->setAttribute(Qt::WA_DeleteOnClose);
    t->QWidget::show();
}

Toast::Toast(QWidget *parent, const QString &message, bool isError, int durationMs)
    : QWidget(parent), m_isError(isError)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_label = new QLabel(message, this);
    m_label->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 13px;").arg(Theme::bg.name()));
    m_label->setAlignment(Qt::AlignCenter);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 12);
    layout->addWidget(m_label);

    adjustSize();

    // Position it at the top center of the parent
    int startY = 0;
    int endY = 40; // 40px offset from top
    int posX = (parent->width() - width()) / 2;
    setGeometry(posX, startY, width(), height());

    auto *effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    effect->setOpacity(0.0);

    // Fade in
    auto *opacityAnim = new QPropertyAnimation(effect, "opacity", this);
    opacityAnim->setDuration(250);
    opacityAnim->setStartValue(0.0);
    opacityAnim->setEndValue(1.0);

    auto *posAnim = new QPropertyAnimation(this, "pos", this);
    posAnim->setDuration(250);
    posAnim->setEasingCurve(QEasingCurve::OutQuad);
    posAnim->setStartValue(QPoint(posX, startY));
    posAnim->setEndValue(QPoint(posX, endY));

    opacityAnim->start(QAbstractAnimation::DeleteWhenStopped);
    posAnim->start(QAbstractAnimation::DeleteWhenStopped);

    // Fade out and close
    QTimer::singleShot(durationMs, this, [this, effect]() {
        auto *fadeOut = new QPropertyAnimation(effect, "opacity", this);
        fadeOut->setDuration(250);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void Toast::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    // Draw rounded background
    p.setBrush(m_isError ? Theme::danger : Theme::accent);
    p.drawRoundedRect(rect(), 10, 10);
}
