#include "floatingball.h"
#include "appsettings.h"
#include "datamanager.h"
#include "theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <cmath>

FloatingBall::FloatingBall(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , m_dm(&DataManager::instance())
    , m_dragging(false)
    , m_moved(false)
    , m_snapped(false)
    , m_hovering(false)
    , m_snapEdge(0)
    , m_ballSize(AppSettings::instance().ballSize())
    , m_clickTimer(new QTimer(this))
    , m_pendingSingleClick(false)
    , m_pulseTimer(new QTimer(this))
    , m_snapCheckTimer(new QTimer(this))
    , m_glowOpacity(0.0)
    , m_pulseStep(0)
    , m_pulseMaxSteps(0)
    , m_loadingTimer(new QTimer(this))
    , m_loadingAngle(0.0)
    , m_snapAnim(new QPropertyAnimation(this, "pos"))
    , m_animating(false)
{
    setFixedSize(m_ballSize, m_ballSize);
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::PointingHandCursor);

    m_clickTimer->setSingleShot(true);
    m_clickTimer->setInterval(300);
    connect(m_clickTimer, &QTimer::timeout, this, &FloatingBall::onSingleClickTimeout);

    m_pulseTimer->setInterval(50);
    connect(m_pulseTimer, &QTimer::timeout, this, &FloatingBall::onPulseTick);

    m_snapCheckTimer->setInterval(50);
    connect(m_snapCheckTimer, &QTimer::timeout, this, &FloatingBall::onSnapCheck);

    m_loadingTimer->setInterval(80);
    connect(m_loadingTimer, &QTimer::timeout, this, &FloatingBall::onLoadingTick);

    m_snapAnim->setDuration(200);
    m_snapAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_snapAnim, &QPropertyAnimation::finished, this, [this]() { m_animating = false; });

    connect(m_dm, &DataManager::accountChanged, this, &FloatingBall::onAccountChanged);
    connect(m_dm, &DataManager::allDataUpdated, this, &FloatingBall::onAllDataUpdated);

    QPoint savedPos = AppSettings::instance().ballPosition();
    move(savedPos);
}

FloatingBall::~FloatingBall()
{
    AppSettings::instance().setBallPosition(pos());
}

QRect FloatingBall::screenRect() const
{
    QScreen *screen = QGuiApplication::screenAt(pos());
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    return screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);
}

void FloatingBall::applyBallSize()
{
    m_ballSize = AppSettings::instance().ballSize();
    setFixedSize(m_ballSize, m_ballSize);
    update();
}

void FloatingBall::setBallVisible(bool visible)
{
    setVisible(visible);
}

bool FloatingBall::isBallVisible() const
{
    return isVisible();
}

void FloatingBall::setStatus(const QString &status)
{
    m_status = status;
    if (status == "..." || status == "Loading")
        startLoading();
    else
        stopLoading();
    update();
    if (status == "Err")
        startPulse(false);
}

void FloatingBall::startLoading()
{
    m_loadingAngle = 0;
    m_loadingTimer->start();
}

void FloatingBall::stopLoading()
{
    m_loadingTimer->stop();
}

void FloatingBall::onLoadingTick()
{
    m_loadingAngle = std::fmod(m_loadingAngle + 25.0, 360.0);
    update();
}

void FloatingBall::onAccountChanged(const QString &)
{
    update();
}

void FloatingBall::onAllDataUpdated()
{
    stopLoading();
    if (m_status == "..." || m_status == "Loading" || m_status == "Err")
        m_status.clear();
    update();
    startPulse(true);
}

void FloatingBall::startPulse(bool success)
{
    m_glowOpacity = success ? 0.8 : 0.6;
    m_pulseStep = 0;
    m_pulseMaxSteps = 16;
    m_pulseTimer->start();
}

void FloatingBall::onPulseTick()
{
    m_pulseStep++;
    m_glowOpacity = qMax(0.0, m_glowOpacity - 0.05);
    update();
    if (m_pulseStep >= m_pulseMaxSteps) {
        m_pulseTimer->stop();
        m_glowOpacity = 0.0;
        update();
    }
}

void FloatingBall::switchAccount(int delta)
{
    auto names = m_dm->accountNames();
    if (names.size() <= 1)
        return;
    QString cur = m_dm->currentAccount();
    int idx = names.indexOf(cur);
    if (idx < 0)
        idx = 0;
    int newIdx = (idx + delta + names.size()) % names.size();
    m_dm->setCurrentAccount(names[newIdx]);
}

void FloatingBall::animateTo(const QPoint &target)
{
    if (m_animating) {
        m_snapAnim->stop();
        m_animating = false;
    }
    if (pos() == target)
        return;
    m_animating = true;
    m_snapAnim->setStartValue(pos());
    m_snapAnim->setEndValue(target);
    m_snapAnim->start();
}

void FloatingBall::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int side = m_ballSize;
    int cx = side / 2;
    int cy = side / 2;
    int radius = side / 2 - 4;
    int penWidth = qMax(AppSettings::instance().ringWidth(), side / 16);

    bool isLoading = m_loadingTimer->isActive();
    QColor baseRingColor = Theme::textDim;

    QString name = m_dm->currentAccount();
    UsageData curData = m_dm->data(name);

    if (!m_status.isEmpty() && m_status != "..." && m_status != "Loading") {
        baseRingColor = Theme::danger;
    } else if (curData.isValid) {
        if (curData.platformType == "deepseek") {
            double bal = curData.balanceTotal();
            baseRingColor = bal >= 0 ? (bal > 10 ? Theme::accent : (bal > 1 ? Theme::warning : Theme::danger)) : Theme::textDim;
        } else {
            double pct = curData.tokenPercentage();
            if (pct >= 0) {
                double usedRatio = pct / 100.0;
                baseRingColor = usedRatio < 0.5 ? Theme::accent : (usedRatio < 0.8 ? Theme::warning : Theme::danger);
            }
        }
    }

    if (m_glowOpacity > 0.01) {
        QColor glowColor = (m_status == "Err") ? Theme::danger : baseRingColor;
        glowColor.setAlphaF(m_glowOpacity * 0.5);
        p.setBrush(glowColor);
        p.setPen(Qt::NoPen);
        p.drawEllipse(cx - radius - 3, cy - radius - 3, (radius + 3) * 2, (radius + 3) * 2);
    }

    QPainterPath path;
    path.addEllipse(cx - radius, cy - radius, radius * 2, radius * 2);
    p.setClipPath(path);
    QColor ballBg = AppSettings::instance().ballBgColor();
    if (!ballBg.isValid())
        ballBg = Theme::bg;
    ballBg.setAlpha(qBound(0, AppSettings::instance().ballBgOpacity(), 255));
    p.fillRect(rect(), ballBg);
    p.setClipping(false);

    int arcRadius = radius - penWidth / 2 - 2;
    QRectF arcRect(cx - arcRadius, cy - arcRadius, arcRadius * 2, arcRadius * 2);
    p.setPen(QPen(Theme::border, penWidth, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(arcRect, 0, 360 * 16);

    if (isLoading) {
        paintLoadingArc(p, side);
    } else if (!m_status.isEmpty() && m_status != "..." && m_status != "Loading") {
        QColor pctUserColor = AppSettings::instance().pctColor();
        int pctUserSize = AppSettings::instance().pctFontSize();
        QColor pctColor = pctUserColor.isValid() ? pctUserColor : Qt::white;
        p.setPen(pctColor);
        QFont f = font();
        f.setPixelSize(pctUserSize > 0 ? pctUserSize : qMax(10, side / 8));
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, m_status);
    } else if (name.isEmpty() || !curData.isValid) {
        QColor timeUserColor = AppSettings::instance().timeColor();
        int timeUserSize = AppSettings::instance().timeFontSize();
        QColor tc = timeUserColor.isValid() ? timeUserColor : Theme::textDim;
        p.setPen(tc);
        QFont f = font();
        f.setPixelSize(timeUserSize > 0 ? timeUserSize : qMax(9, side / 9));
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, name.isEmpty() ? "No Data" : "Err");
    } else {
        if (curData.platformType == "deepseek")
            paintDeepSeekAccount(p, side, curData);
        else
            paintGlmAccount(p, side, curData);
    }

    auto names = m_dm->accountNames();
    if (names.size() > 1) {
        int dotSize = qMax(3, side / 25);
        int totalDots = names.size();
        int dotSpacing = dotSize + 3;
        int totalWidth = totalDots * dotSpacing;
        int startX = cx - totalWidth / 2 + dotSpacing / 2;
        int dotY = side - dotSize - 3;
        for (int i = 0; i < totalDots; i++) {
            p.setPen(Qt::NoPen);
            p.setBrush(names[i] == m_dm->currentAccount() ? Theme::accent : QColor(80, 80, 80));
            p.drawEllipse(startX + i * dotSpacing, dotY, dotSize, dotSize);
        }
    }
}

void FloatingBall::paintLoadingArc(QPainter &p, int side)
{
    int cx = side / 2;
    int cy = side / 2;
    int radius = side / 2 - 4;
    int penWidth = qMax(AppSettings::instance().ringWidth(), side / 16);
    int arcRadius = radius - penWidth / 2 - 2;
    QRectF arcRect(cx - arcRadius, cy - arcRadius, arcRadius * 2, arcRadius * 2);

    p.setPen(QPen(Theme::accent, penWidth, Qt::SolidLine, Qt::RoundCap));
    int startAngle = static_cast<int>(m_loadingAngle) * 16;
    p.drawArc(arcRect, startAngle, 60 * 16);

    QColor pctUserColor = AppSettings::instance().pctColor();
    QColor pc = pctUserColor.isValid() ? pctUserColor : Qt::white;
    p.setPen(pc);
    QFont f = font();
    f.setPixelSize(qMax(9, side / 9));
    p.setFont(f);
    p.drawText(rect(), Qt::AlignCenter, "...");
}

void FloatingBall::paintGlmAccount(QPainter &p, int side, const UsageData &data)
{
    int cx = side / 2;
    int cy = side / 2;
    int radius = side / 2 - 4;

    QColor pctUserColor = AppSettings::instance().pctColor();
    int pctUserSize = AppSettings::instance().pctFontSize();
    QColor timeUserColor = AppSettings::instance().timeColor();
    int timeUserSize = AppSettings::instance().timeFontSize();
    QColor pctColor = pctUserColor.isValid() ? pctUserColor : Qt::white;
    QColor timeColorVal = timeUserColor.isValid() ? timeUserColor : Theme::textDim;

    int penWidth = qMax(AppSettings::instance().ringWidth(), side / 16);
    int arcRadius = radius - penWidth / 2 - 2;
    QRectF arcRect(cx - arcRadius, cy - arcRadius, arcRadius * 2, arcRadius * 2);

    double tokenPct = data.tokenPercentage();
    int usedPct = (tokenPct >= 0) ? static_cast<int>(tokenPct) : -1;

    if (usedPct >= 0) {
        QColor ringColor = usedPct < 50 ? Theme::accent : (usedPct < 80 ? Theme::warning : Theme::danger);
        p.setPen(QPen(ringColor, penWidth, Qt::SolidLine, Qt::RoundCap));
        int spanAngle = static_cast<int>(usedPct / 100.0 * 360 * 16);
        p.drawArc(arcRect, 90 * 16, -spanAngle);
    }

    int timeSize = timeUserSize > 0 ? timeUserSize : qMax(12, side * 2 / 9);
    int pctSize = pctUserSize > 0 ? pctUserSize : qMax(9, side * 3 / 20);
    int gap = 2;
    int blockH = timeSize + pctSize + gap;
    int timeY = cy - blockH / 2;

    QFont f = font();
    f.setPixelSize(timeSize);
    f.setBold(true);
    p.setFont(f);
    p.setPen(timeColorVal);
    if (!data.tokenResetTime().isEmpty()) {
        p.drawText(QRect(0, timeY, side, timeSize + 2), Qt::AlignCenter, data.tokenResetTime());
    } else if (!data.expiryInfo().isEmpty()) {
        QString shortInfo = data.expiryInfo();
        if (shortInfo.length() > 10)
            shortInfo = shortInfo.left(9) + "..";
        p.drawText(QRect(0, timeY, side, timeSize + 2), Qt::AlignCenter, shortInfo);
    }

    f.setPixelSize(pctSize);
    f.setBold(false);
    p.setFont(f);
    p.setPen(pctColor);
    if (usedPct >= 0) {
        QString pctText = QString::number(usedPct) + "%";
        int pctY = timeY + timeSize + gap;
        p.drawText(QRect(0, pctY, side, pctSize + 2), Qt::AlignCenter, pctText);
    } else {
        p.drawText(QRect(0, timeY + timeSize + gap, side, pctSize + 2), Qt::AlignCenter, "--");
    }
}

void FloatingBall::paintDeepSeekAccount(QPainter &p, int side, const UsageData &data)
{
    int cx = side / 2;
    int cy = side / 2;

    QColor pctUserColor = AppSettings::instance().pctColor();
    int pctUserSize = AppSettings::instance().pctFontSize();
    QColor pctColor = pctUserColor.isValid() ? pctUserColor : Qt::white;

    int radius = side / 2 - 4;
    int penWidth = qMax(AppSettings::instance().ringWidth(), side / 16);
    int arcRadius = radius - penWidth / 2 - 2;
    QRectF arcRect(cx - arcRadius, cy - arcRadius, arcRadius * 2, arcRadius * 2);

    struct BalanceEntry {
        QString currency;
        double amount;
    };
    QList<BalanceEntry> balances;
    bool showUSD = AppSettings::instance().dsShowUSD();
    for (const auto &q : data.quotaLimits) {
        if (q.type.startsWith("BALANCE_")) {
            BalanceEntry e;
            e.currency = q.type.mid(8);
            e.amount = q.total / 100.0;
            if (e.currency == "USD" && !showUSD)
                continue;
            balances.append(e);
        }
    }

    double primaryBal = balances.isEmpty() ? -1.0 : balances.first().amount;
    if (primaryBal >= 0) {
        const AppSettings &s = AppSettings::instance();
        double greenT = s.dsGreenThreshold();
        double yellowT = s.dsYellowThreshold();
        QColor greenC = s.dsGreenColor();
        QColor yellowC = s.dsYellowColor();
        QColor redC = s.dsRedColor();
        if (!balances.isEmpty() && balances.first().currency == "USD") {
            greenT = s.dsUsdGreenThreshold();
            yellowT = s.dsUsdYellowThreshold();
            greenC = s.dsUsdGreenColor();
            yellowC = s.dsUsdYellowColor();
            redC = s.dsUsdRedColor();
        }
        QColor ringColor;
        if (primaryBal > greenT) ringColor = greenC;
        else if (primaryBal > yellowT) ringColor = yellowC;
        else ringColor = redC;
        p.setPen(QPen(ringColor, penWidth, Qt::SolidLine, Qt::RoundCap));
        double totalRef = balances.first().currency == "USD"
                              ? AppSettings::instance().dsUsdTotalBalance()
                              : AppSettings::instance().dsTotalBalance();
        int spanAngle = static_cast<int>(qMin(primaryBal / qMax(totalRef, 0.01), 1.0) * 360 * 16);
        p.drawArc(arcRect, 90 * 16, -spanAngle);
    }

    QFont f = font();

    if (balances.size() <= 1) {
        int balSize = pctUserSize > 0 ? pctUserSize : qMax(11, side * 2 / 9);
        int curSize = qMax(8, side / 10);
        int gap = 1;
        int blockH = balSize + curSize + gap;
        int startY = cy - blockH / 2;

        f.setPixelSize(balSize);
        f.setBold(true);
        p.setFont(f);
        p.setPen(pctColor);
        if (primaryBal >= 0) {
            p.drawText(QRect(0, startY, side, balSize + 2),
                       Qt::AlignCenter, QString::number(primaryBal, 'f', 2));
        } else {
            p.drawText(QRect(0, startY, side, balSize + 2), Qt::AlignCenter, "--");
        }

        f.setPixelSize(curSize);
        f.setBold(false);
        p.setFont(f);
        p.setPen(Theme::textDim);
        if (!balances.isEmpty() && !balances.first().currency.isEmpty())
            p.drawText(QRect(0, startY + balSize + gap, side, curSize + 1),
                       Qt::AlignCenter, balances.first().currency);
    } else {
        int lineSize = pctUserSize > 0 ? pctUserSize : qMax(9, side / 7);
        int gap = 1;
        int count = balances.size();
        int blockH = lineSize * count + gap * (count - 1);
        int startY = cy - blockH / 2;

        for (int i = 0; i < count; i++) {
            f.setPixelSize(lineSize);
            f.setBold(true);
            p.setFont(f);
            p.setPen(pctColor);
            QString curSymbol = balances[i].currency == "USD" ? "$" : (balances[i].currency == "CNY" ? QString::fromUtf8("\xc2\xa5") : balances[i].currency);
            QString line = curSymbol + QString::number(balances[i].amount, 'f', 2);
            p.drawText(QRect(0, startY + i * (lineSize + gap), side, lineSize + 2),
                       Qt::AlignCenter, line);
        }
    }
}

void FloatingBall::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_moved = false;
        m_dragPos = event->globalPos() - pos();
        m_pendingSingleClick = false;
        m_clickTimer->stop();
        if (m_animating) {
            m_snapAnim->stop();
            m_animating = false;
        }
    }
}

void FloatingBall::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->globalPos() - m_dragPos - pos();
        if (!m_moved && delta.manhattanLength() > 4)
            m_moved = true;
        if (m_moved) {
            m_pendingSingleClick = false;
            m_clickTimer->stop();
            if (m_snapped) {
                m_snapped = false;
                m_snapEdge = 0;
                m_snapCheckTimer->stop();
            }
            move(event->globalPos() - m_dragPos);
        }
    }
}

void FloatingBall::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_dragging) {
            m_dragging = false;
            if (m_moved) {
                checkSnapToEdge();
                AppSettings::instance().setBallPosition(pos());
            } else {
                m_pendingSingleClick = true;
                m_clickTimer->start();
            }
        }
    }
}

void FloatingBall::mouseDoubleClickEvent(QMouseEvent *)
{
    m_clickTimer->stop();
    m_pendingSingleClick = false;
    m_dragging = false;
    emit doubleClicked();
}

void FloatingBall::onSingleClickTimeout()
{
    if (m_pendingSingleClick) {
        m_pendingSingleClick = false;
        auto names = m_dm->accountNames();
        if (names.size() > 1)
            switchAccount(1);
        emit singleClicked();
    }
}

void FloatingBall::enterEvent(QEvent *)
{
    m_hovering = true;
}

void FloatingBall::leaveEvent(QEvent *)
{
    m_hovering = false;
}

void FloatingBall::onSnapCheck()
{
    if (!m_snapped)
        return;

    QPoint mousePos = QCursor::pos();
    QRect ballRect = geometry();
    QRect sRect = screenRect();

    QRect hitRect = ballRect;
    if (m_snapEdge == 1)
        hitRect.setLeft(sRect.left());
    else if (m_snapEdge == 2)
        hitRect.setRight(sRect.right());

    bool inZone = hitRect.contains(mousePos);
    if (inZone && !m_hovering) {
        m_hovering = true;
        QPoint target;
        if (m_snapEdge == 1)
            target = QPoint(sRect.left(), y());
        else
            target = QPoint(sRect.right() - m_ballSize, y());
        animateTo(target);
    } else if (!inZone && m_hovering) {
        m_hovering = false;
        snapToEdge();
    }
}

void FloatingBall::checkSnapToEdge()
{
    QRect sRect = screenRect();
    int x = pos().x();
    int distLeft = x - sRect.left();
    int distRight = sRect.right() - (x + m_ballSize);

    if (distLeft < SNAP_THRESHOLD)
        m_snapEdge = 1;
    else if (distRight < SNAP_THRESHOLD)
        m_snapEdge = 2;
    else
        m_snapEdge = 0;

    if (m_snapEdge != 0) {
        m_snapped = true;
        snapToEdge();
        m_snapCheckTimer->start();
    } else {
        m_snapped = false;
        m_snapCheckTimer->stop();
    }
}

void FloatingBall::snapToEdge()
{
    QRect sRect = screenRect();
    int overlap = m_ballSize * 2 / 3;
    QPoint target;

    if (m_snapEdge == 1)
        target = QPoint(sRect.left() - m_ballSize + overlap, pos().y());
    else if (m_snapEdge == 2)
        target = QPoint(sRect.right() - overlap, pos().y());
    else
        return;

    int yPos = target.y();
    if (yPos < sRect.top())
        yPos = sRect.top();
    if (yPos + m_ballSize > sRect.bottom())
        yPos = sRect.bottom() - m_ballSize;
    target.setY(yPos);

    animateTo(target);
}
