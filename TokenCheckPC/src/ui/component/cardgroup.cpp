#include "cardgroup.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QPropertyAnimation>
#include <QPainter>
#include "toggleswitch.h"
#include "../theme.h"

CardGroup::CardGroup(const QString &title, QWidget *parent)
    : QWidget(parent), m_toggle(nullptr), m_expanded(true)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(0);

    m_header = new QWidget(this);
    m_header->installEventFilter(this);
    m_header->setCursor(Qt::PointingHandCursor);

    auto *headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(16, 12, 16, 12);
    headerLayout->setSpacing(8);

    m_iconLabel = new QLabel(QString::fromUtf8("\u25BC"), m_header); // Downward chevron
    m_iconLabel->setStyleSheet(QString("color: %1; font-size: 10px;").arg(Theme::textDim.name()));
    
    m_titleLabel = new QLabel(title, m_header);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    headerLayout->addWidget(m_iconLabel);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    m_contentContainer = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(16, 4, 16, 16);

    mainLayout->addWidget(m_header);
    mainLayout->addWidget(m_contentContainer);

    m_anim = new QPropertyAnimation(m_contentContainer, "maximumHeight", this);
    m_anim->setDuration(250);
    m_anim->setEasingCurve(QEasingCurve::OutQuad);
}

void CardGroup::addWidget(QWidget *widget)
{
    m_contentLayout->addWidget(widget);
}

void CardGroup::addLayout(QLayout *layout)
{
    m_contentLayout->addLayout(layout);
}

ToggleSwitch* CardGroup::addToggleSwitch()
{
    if (!m_toggle) {
        m_toggle = new ToggleSwitch(m_header);
        auto *hl = static_cast<QHBoxLayout*>(m_header->layout());
        hl->addWidget(m_toggle);
    }
    return m_toggle;
}

bool CardGroup::isExpanded() const
{
    return m_expanded;
}

void CardGroup::setExpanded(bool expanded)
{
    if (m_expanded == expanded) return;
    m_expanded = expanded;

    m_iconLabel->setText(m_expanded ? QString::fromUtf8("\u25BC") : QString::fromUtf8("\u25B6"));

    if (m_expanded) {
        m_anim->setStartValue(0);
        m_anim->setEndValue(m_contentLayout->sizeHint().height());
        m_contentContainer->setVisible(true);
    } else {
        m_anim->setStartValue(m_contentContainer->height());
        m_anim->setEndValue(0);
    }
    
    if (!m_expanded) {
        connect(m_anim, &QPropertyAnimation::finished, m_contentContainer, [this]() {
            if (!m_expanded) m_contentContainer->setVisible(false);
            disconnect(m_anim, &QPropertyAnimation::finished, nullptr, nullptr);
        });
    } else {
        disconnect(m_anim, &QPropertyAnimation::finished, nullptr, nullptr);
        connect(m_anim, &QPropertyAnimation::finished, m_contentContainer, [this]() {
            if (m_expanded) m_contentContainer->setMaximumHeight(QWIDGETSIZE_MAX);
            disconnect(m_anim, &QPropertyAnimation::finished, nullptr, nullptr);
        });
    }
    
    m_anim->start();
}

bool CardGroup::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_header && event->type() == QEvent::MouseButtonRelease) {
        // Toggle if not clicked on the ToggleSwitch itself
        setExpanded(!m_expanded);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void CardGroup::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    p.setBrush(Theme::surface);
    p.setPen(QPen(Theme::border, 1));
    p.drawRoundedRect(rect().adjusted(0,0,-1,-1), 8, 8);
}
