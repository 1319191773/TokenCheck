#include "clickablecard.h"
#include <QMouseEvent>

ClickableCard::ClickableCard(QWidget *parent) : QFrame(parent) {}

void ClickableCard::mousePressEvent(QMouseEvent *) { if (onClick) onClick(); }
