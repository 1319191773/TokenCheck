#include "moderncombobox.h"
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include "../theme.h"

ModernComboBox::ModernComboBox(QWidget *parent)
    : QPushButton(parent), m_currentIndex(-1), m_arrowRotation(0)
{
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet("QPushButton { text-align: left; padding-left: 12px; padding-right: 32px; } QPushButton::menu-indicator { image: none; }");

    m_menu = new QMenu(this);
    setMenu(m_menu);

    m_anim = new QPropertyAnimation(this, "arrowRotation", this);
    m_anim->setDuration(150);
    m_anim->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_menu, &QMenu::aboutToShow, this, &ModernComboBox::onMenuAboutToShow);
    connect(m_menu, &QMenu::aboutToHide, this, &ModernComboBox::onMenuAboutToHide);
    connect(m_menu, &QMenu::triggered, this, &ModernComboBox::onActionTriggered);
}

void ModernComboBox::addItem(const QString &text, const QVariant &userData)
{
    QAction *action = m_menu->addAction(text);
    action->setData(m_itemData.size());
    m_itemData.append(userData);
    if (m_currentIndex == -1) {
        setCurrentIndex(0);
    }
}

void ModernComboBox::setCurrentIndex(int index)
{
    if (index >= 0 && index < m_itemData.size() && index != m_currentIndex) {
        m_currentIndex = index;
        setText(m_menu->actions().at(index)->text());
        emit currentIndexChanged(m_currentIndex);
    }
}

int ModernComboBox::currentIndex() const { return m_currentIndex; }

QString ModernComboBox::currentText() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_menu->actions().size())
        return m_menu->actions().at(m_currentIndex)->text();
    return "";
}

QVariant ModernComboBox::currentData() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_itemData.size())
        return m_itemData.at(m_currentIndex);
    return QVariant();
}

int ModernComboBox::findData(const QVariant &data) const {
    for (int i = 0; i < m_itemData.size(); ++i) {
        if (m_itemData.at(i) == data) return i;
    }
    return -1;
}

void ModernComboBox::onMenuAboutToShow()
{
    m_anim->setStartValue(m_arrowRotation);
    m_anim->setEndValue(180);
    m_anim->start();
}

void ModernComboBox::onMenuAboutToHide()
{
    m_anim->setStartValue(m_arrowRotation);
    m_anim->setEndValue(0);
    m_anim->start();
}

void ModernComboBox::onActionTriggered(QAction *action)
{
    int idx = action->data().toInt();
    setCurrentIndex(idx);
}

int ModernComboBox::arrowRotation() const { return m_arrowRotation; }
void ModernComboBox::setArrowRotation(int rotation) { 
    m_arrowRotation = rotation; 
    update(); 
}

void ModernComboBox::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(width() - 16, height() / 2);
    p.rotate(m_arrowRotation);

    QPainterPath path;
    path.moveTo(-4, -2);
    path.lineTo(0, 2);
    path.lineTo(4, -2);
    
    QPen pen(Theme::text, 2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.drawPath(path);
}
