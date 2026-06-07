#include "toggleswitch.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

#include "../theme.h"

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QWidget(parent), m_checked(false), m_hovered(false), m_offset(0)
{
    setFixedSize(40, 22);
    setCursor(Qt::PointingHandCursor);

    m_anim = new QPropertyAnimation(this, "offset", this);
    m_anim->setDuration(150);
    m_anim->setEasingCurve(QEasingCurve::OutQuad);
}

bool ToggleSwitch::isChecked() const { return m_checked; }

void ToggleSwitch::setChecked(bool checked)
{
    if (m_checked == checked) return;
    m_checked = checked;
    m_anim->setStartValue(m_offset);
    m_anim->setEndValue(m_checked ? width() - height() : 0);
    m_anim->start();
    emit toggled(m_checked);
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setChecked(!m_checked);
    }
    QWidget::mouseReleaseEvent(event);
}

void ToggleSwitch::enterEvent(QEvent *event)
{
    m_hovered = true;
    update();
    QWidget::enterEvent(event);
}

void ToggleSwitch::leaveEvent(QEvent *event)
{
    m_hovered = false;
    update();
    QWidget::leaveEvent(event);
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bgColor = m_checked ? Theme::accent : (m_hovered ? Theme::palette().surfaceHover : Theme::surfaceAlt);
    if (!m_checked) {
        bgColor = m_hovered ? Theme::palette().borderHover : Theme::border;
    }
    
    // Draw track
    QPainterPath path;
    path.addRoundedRect(rect(), height() / 2, height() / 2);
    p.setPen(Qt::NoPen);
    p.setBrush(bgColor);
    p.drawPath(path);

    // Draw handle
    int margin = 3;
    int handleRadius = (height() - margin * 2) / 2;
    int handleX = margin + m_offset;
    int handleY = margin;
    
    QColor handleColor = Qt::white;
    if (!m_checked) {
        handleColor = Theme::textDim;
    }
    
    p.setBrush(handleColor);
    p.drawEllipse(handleX, handleY, handleRadius * 2, handleRadius * 2);
}

int ToggleSwitch::offset() const { return m_offset; }
void ToggleSwitch::setOffset(int value) { m_offset = value; update(); }
