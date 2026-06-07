#include "studiocolorpalette.h"
#include "theme.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QStackedWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtMath>
#include <QConicalGradient>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QTimer>

static QString rgba(const QColor &c, int alpha = -1)
{
    int a = alpha >= 0 ? alpha : c.alpha();
    return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(a / 255.0, 0, 'f', 2);
}

SplitPreview::SplitPreview(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(48, 48);
    setCursor(Qt::PointingHandCursor);
    setToolTip(QObject::tr("Left: Current | Right: Previous\nClick right to restore"));
}

void SplitPreview::setOldColor(const QColor &c) { m_oldColor = c; update(); }
void SplitPreview::setNewColor(const QColor &c) { m_newColor = c; update(); }

void SplitPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int s = qMin(width(), height());
    QRectF rect(0, 0, s, s);

    QPainterPath clip;
    clip.addEllipse(rect);
    painter.setClipPath(clip);

    painter.setClipping(false);
    QColor borderCol = Theme::border;
    borderCol.setAlpha(30);
    painter.setPen(QPen(borderCol, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect.adjusted(1, 1, -1, -1));
    painter.setClipping(true);

    painter.fillRect(QRectF(0, 0, s / 2.0, s), m_newColor);
    painter.fillRect(QRectF(s / 2.0, 0, s / 2.0, s), m_oldColor);
}

void SplitPreview::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().x() >= width() / 2)
        emit oldColorClicked();
}

ColorWheel::ColorWheel(int size, QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(size, size);
    setCursor(Qt::CrossCursor);
}

void ColorWheel::setColor(const QColor &color)
{
    m_hue = color.hueF() < 0 ? m_hue : color.hueF();
    m_saturation = color.saturationF();
    m_value = color.value();
    update();
}

QColor ColorWheel::color() const
{
    return QColor::fromHsvF(m_hue, m_saturation, m_value / 255.0);
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_cacheDirty || m_wheelCache.size() != size())
    {
        m_wheelCache = QPixmap(size());
        m_wheelCache.fill(Qt::transparent);
        QPainter cp(&m_wheelCache);
        cp.setRenderHint(QPainter::Antialiasing);

        QRectF rect(0, 0, width(), height());
        QPointF center = rect.center();
        qreal radius = qMin(width(), height()) / 2.0;

        QConicalGradient conical(center, 90);
        conical.setColorAt(0.0,   QColor::fromHsvF(0.0,       1.0, 1.0));
        conical.setColorAt(1.0/6, QColor::fromHsvF(1.0 / 6.0, 1.0, 1.0));
        conical.setColorAt(2.0/6, QColor::fromHsvF(2.0 / 6.0, 1.0, 1.0));
        conical.setColorAt(3.0/6, QColor::fromHsvF(3.0 / 6.0, 1.0, 1.0));
        conical.setColorAt(4.0/6, QColor::fromHsvF(4.0 / 6.0, 1.0, 1.0));
        conical.setColorAt(5.0/6, QColor::fromHsvF(5.0 / 6.0, 1.0, 1.0));
        conical.setColorAt(1.0,   QColor::fromHsvF(1.0,       1.0, 1.0));

        QPainterPath path;
        path.addEllipse(rect);
        cp.fillPath(path, conical);

        QRadialGradient radial(center, radius);
        radial.setColorAt(0.0, QColor(255, 255, 255, 255));
        radial.setColorAt(1.0, QColor(255, 255, 255, 0));
        cp.setCompositionMode(QPainter::CompositionMode_Screen);
        cp.fillPath(path, radial);

        m_cacheDirty = false;
    }

    painter.drawPixmap(0, 0, m_wheelCache);

    qreal angle = -m_hue * 360.0 - 90.0;
    qreal r = m_saturation * (width() / 2.0 - 8);
    QPointF center(width() / 2.0, height() / 2.0);
    qreal dx = r * qCos(qDegreesToRadians(angle));
    qreal dy = r * qSin(qDegreesToRadians(angle));
    QPointF thumbPos = center + QPointF(dx, dy);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 128));
    painter.drawEllipse(thumbPos, 8, 8);
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(thumbPos, 6, 6);
}

void ColorWheel::mousePressEvent(QMouseEvent *event) { pickColor(event->pos()); }
void ColorWheel::mouseMoveEvent(QMouseEvent *event)  { pickColor(event->pos()); }
void ColorWheel::mouseReleaseEvent(QMouseEvent *)    {}
void ColorWheel::resizeEvent(QResizeEvent *)          { m_cacheDirty = true; }

void ColorWheel::pickColor(const QPoint &pos)
{
    QPointF center(width() / 2.0, height() / 2.0);
    qreal dx = pos.x() - center.x();
    qreal dy = pos.y() - center.y();
    qreal radius = qMin(width(), height()) / 2.0;
    qreal dist = qSqrt(dx * dx + dy * dy);

    m_saturation = qBound(0.0, dist / radius, 1.0);

    qreal angle = qAtan2(dy, dx);
    if (angle < 0) angle += 2 * M_PI;
    qreal deg = qRadiansToDegrees(angle) + 90;
    if (deg >= 360) deg -= 360;
    m_hue = 1.0 - deg / 360.0;

    update();
    emit colorChanged(color());
}

PrecisionTrack::PrecisionTrack(TrackMode mode, QWidget *parent)
    : QWidget(parent), m_mode(mode), m_value(255), m_baseColor(Qt::cyan)
{
    setFixedWidth(24);
    setMinimumHeight(100);
    setCursor(Qt::SizeVerCursor);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");
}

void PrecisionTrack::setValue(int val)
{
    m_value = qBound(0, val, 255);
    update();
}

void PrecisionTrack::setBaseColor(const QColor &color)
{
    if (m_mode == ValueMode)
        m_baseColor = QColor::fromHsvF(qMax(0.0, color.hueF()), color.saturationF(), 1.0);
    else
        m_baseColor = color;
    update();
}

void PrecisionTrack::drawCheckerboard(QPainter &painter, const QRect &rect)
{
    int tileSize = 4;
    QColor c1(200, 200, 200);
    QColor c2(240, 240, 240);
    for (int y = rect.top(); y < rect.bottom(); y += tileSize)
    {
        for (int x = rect.left(); x < rect.right(); x += tileSize)
        {
            painter.fillRect(x, y, tileSize, tileSize,
                             ((x / tileSize + y / tileSize) % 2 == 0) ? c1 : c2);
        }
    }
}

void PrecisionTrack::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int trackWidth = 14;
    int centerX = width() / 2;
    QRect trackRect(centerX - trackWidth / 2, 0, trackWidth, height());

    if (m_mode == AlphaMode)
    {
        painter.save();
        QPainterPath clip;
        clip.addRoundedRect(trackRect, 4, 4);
        painter.setClipPath(clip);
        drawCheckerboard(painter, trackRect);
        painter.restore();
    }

    int thumbH = 6;
    int margin = thumbH;
    int minY = margin;
    int maxY = height() - margin;
    int trackRange = maxY - minY;

    qreal fillRatio = m_value / 255.0;
    int thumbY = maxY - qRound(trackRange * fillRatio);

    QLinearGradient grad(trackRect.x(), maxY, trackRect.x(), minY);
    if (m_mode == ValueMode)
    {
        grad.setColorAt(0, Qt::black);
        grad.setColorAt(1, m_baseColor);
    }
    else
    {
        QColor transColor = m_baseColor;
        transColor.setAlpha(0);
        grad.setColorAt(0, transColor);
        grad.setColorAt(1, m_baseColor);
    }
    painter.setBrush(grad);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(trackRect, 4, 4);
    
    QColor borderColor = Theme::border;
    borderColor.setAlpha(80);
    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(trackRect.adjusted(0, 0, -1, -1), 4, 4);

    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(trackRect.x() - 2, thumbY - 3, trackWidth + 3, 6);
    
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(trackRect.x() - 1, thumbY - 2, trackWidth + 1, 4);
}

void PrecisionTrack::mousePressEvent(QMouseEvent *event) { pickValue(event->pos()); }
void PrecisionTrack::mouseMoveEvent(QMouseEvent *event)  { pickValue(event->pos()); }
void PrecisionTrack::mouseReleaseEvent(QMouseEvent *)    {}

void PrecisionTrack::pickValue(const QPoint &pos)
{
    int thumbH = 6;
    int margin = thumbH;
    int minY = margin;
    int maxY = height() - margin;
    qreal ratio = 1.0 - qBound(0.0, (qreal)(pos.y() - minY) / (maxY - minY), 1.0);
    setValue(qRound(ratio * 255));
    emit valueChanged(m_value);
}

void ScrubberInput::updateStyleSheet(bool hover)
{
    QColor textCol = hover ? Theme::text : Theme::textDim;
    QColor editBg = hover ? Theme::surfaceAlt : Theme::surface;
    QColor editBorder = hover ? Theme::border : Theme::border;
    QColor editText = hover ? Theme::text : Theme::textDim;

    setStyleSheet("border: none; background: transparent;");
    m_labelTitle->setStyleSheet(
        QString("QLabel { font-size: 10px; font-weight: 600; color: %1; background: transparent; }")
            .arg(textCol.name()));
    m_editValue->setStyleSheet(
        QString("QLineEdit { font-family: 'Consolas', 'Courier New', monospace; "
                "font-size: 12px; color: %1; "
                "border: 1px solid %2; border-radius: 6px; "
                "background: %3; padding: 1px 4px; }"
                "QLineEdit:focus { border: 1px solid %4; background: %5; }")
            .arg(editText.name(),
                 editBorder.name(),
                 editBg.name(),
                 Theme::accent.name(),
                 Theme::surface.name()));
}

ScrubberInput::ScrubberInput(const QString &label, int min, int max,
                             int value, QWidget *parent)
    : QWidget(parent), m_min(min), m_max(max)
{
    setCursor(Qt::SizeHorCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setSpacing(2);

    m_labelTitle = new QLabel(label, this);
    m_labelTitle->setAlignment(Qt::AlignCenter);

    m_editValue = new QLineEdit(QString::number(value), this);
    m_editValue->setAlignment(Qt::AlignCenter);
    m_editValue->setFixedHeight(20);
    m_editValue->setFocusPolicy(Qt::ClickFocus);
    m_editValue->installEventFilter(this);

    layout->addWidget(m_labelTitle);
    layout->addWidget(m_editValue);

    updateStyleSheet(false);
    setFocusPolicy(Qt::ClickFocus);

    connect(m_editValue, &QLineEdit::editingFinished, this, [this]() {
        bool ok = false;
        int val = m_editValue->text().toInt(&ok);
        if (ok)
        {
            val = qBound(m_min, val, m_max);
            m_editValue->setText(QString::number(val));
            emit valueChanged(val);
        }
    });
}

int ScrubberInput::value() const
{
    bool ok = false;
    int val = m_editValue->text().toInt(&ok);
    return ok ? val : 0;
}

void ScrubberInput::setValue(int val)
{
    m_editValue->setText(QString::number(val));
}

void ScrubberInput::mousePressEvent(QMouseEvent *event)
{
    m_lastX = event->globalPos().x();
    m_dragging = true;
}

bool ScrubberInput::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_editValue)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton)
            {
                m_editValue->clearFocus();
                m_lastX = me->globalPos().x();
                m_dragging = true;
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove && m_dragging)
        {
            auto *me = static_cast<QMouseEvent *>(event);
            int dx = me->globalPos().x() - m_lastX;
            m_lastX = me->globalPos().x();
            qreal step = (m_max - m_min) / 200.0;
            int newVal = qBound(m_min, value() + qRound(dx * step), m_max);
            setValue(newVal);
            emit valueChanged(newVal);
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            m_dragging = false;
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ScrubberInput::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragging) return;
    int dx = event->globalPos().x() - m_lastX;
    m_lastX = event->globalPos().x();
    qreal step = (m_max - m_min) / 200.0;
    int newVal = qBound(m_min, value() + qRound(dx * step), m_max);
    setValue(newVal);
    emit valueChanged(newVal);
}

void ScrubberInput::mouseReleaseEvent(QMouseEvent *)
{
    m_dragging = false;
}

void ScrubberInput::enterEvent(QEvent *)
{
    if (!m_editValue->hasFocus())
        updateStyleSheet(true);
}

void ScrubberInput::leaveEvent(QEvent *)
{
    if (!m_editValue->hasFocus())
        updateStyleSheet(false);
}

StudioColorPalette::StudioColorPalette(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(340);
    setStyleSheet("background: transparent;");
    setFocusPolicy(Qt::ClickFocus);
    setupUI();
    applyThemeColors();
    setColor(QColor(6, 182, 212));
}

void StudioColorPalette::applyThemeColors()
{
    QColor bg = Theme::bg;
    QColor sf = Theme::surface;
    QColor sa = Theme::surfaceAlt;
    QColor sh = Theme::palette().surfaceHover;
    QColor bd = Theme::border;
    QColor tx = Theme::text;
    QColor td = Theme::textDim;
    QColor ac = Theme::accent;

    QString gridFrameStyle =
        QString("QFrame#dataGridFrame { background: %1; border-radius: 12px; "
                "border: 1px solid %2; }")
            .arg(rgba(sa, 25), rgba(bd, 15));

    if (auto *gridFrame = findChild<QFrame *>("dataGridFrame"))
    {
        gridFrame->setStyleSheet(gridFrameStyle);
    }

    m_hexLabel->setStyleSheet(
        QString("font-family: 'Consolas', 'Courier New', monospace; "
                "font-size: 22px; font-weight: bold; color: %1;")
            .arg(tx.name()));

    m_hexEdit->setStyleSheet(
        QString("font-family: 'Consolas', 'Courier New', monospace; "
                "font-size: 22px; font-weight: bold; color: %1; "
                "border: 1px solid %2; border-radius: 4px; background: %3; "
                "padding: 2px 6px;")
            .arg(tx.name(), ac.name(), sf.name()));

    update();
}

void StudioColorPalette::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 16);
    mainLayout->setSpacing(14);

    QColor bg = Theme::bg;
    QColor sf = Theme::surface;
    QColor sa = Theme::surfaceAlt;
    QColor sh = Theme::palette().surfaceHover;
    QColor bd = Theme::border;
    QColor tx = Theme::text;
    QColor td = Theme::textDim;
    QColor ac = Theme::accent;

    QHBoxLayout *headerLayout = new QHBoxLayout();

    m_splitPreview = new SplitPreview(this);
    connect(m_splitPreview, &SplitPreview::oldColorClicked, this, [this]() {
        setColor(m_oldColor);
    });

    m_hexStack = new QStackedWidget(this);

    m_hexLabel = new QLabel("#06B6D4", this);
    m_hexLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_hexLabel->setStyleSheet(
        QString("font-family: 'Consolas', 'Courier New', monospace; "
                "font-size: 26px; font-weight: bold; color: %1; background: transparent;")
            .arg(tx.name()));
    m_hexLabel->setCursor(Qt::IBeamCursor);
    m_hexLabel->installEventFilter(this);

    m_hexEdit = new QLineEdit("#06B6D4", this);
    m_hexEdit->setAlignment(Qt::AlignCenter);
    m_hexEdit->setStyleSheet(
        QString("font-family: 'Consolas', 'Courier New', monospace; "
                "font-size: 26px; font-weight: bold; color: %1; "
                "border: 1px solid %2; border-radius: 4px; background: %3; "
                "padding: 2px 6px;")
            .arg(tx.name(), ac.name(), sf.name()));

    m_hexStack->addWidget(m_hexLabel);
    m_hexStack->addWidget(m_hexEdit);
    m_hexStack->setFixedHeight(40);
    m_hexStack->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_hexStack->setStyleSheet("background: transparent;");

    connect(m_hexEdit, &QLineEdit::editingFinished, this, &StudioColorPalette::commitHexEdit);

    headerLayout->addWidget(m_splitPreview);
    headerLayout->addStretch();
    headerLayout->addWidget(m_hexStack, 0, Qt::AlignRight | Qt::AlignVCenter);
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *interactiveLayout = new QHBoxLayout();
    interactiveLayout->setSpacing(20);
    interactiveLayout->setAlignment(Qt::AlignCenter);

    m_wheel = new ColorWheel(220, this);
    interactiveLayout->addWidget(m_wheel);

    QHBoxLayout *tracksLayout = new QHBoxLayout();
    tracksLayout->setContentsMargins(12, 0, 0, 0);
    tracksLayout->setSpacing(16);

    QVBoxLayout *valLayout = new QVBoxLayout();
    valLayout->setSpacing(4);
    QLabel *lVal = new QLabel("VAL", this);
    lVal->setAlignment(Qt::AlignCenter);
    lVal->setStyleSheet(QString("font-size: 11px; font-weight: 700; color: %1; font-family: 'Consolas', monospace; background: transparent;").arg(td.name()));
    m_valueTrack = new PrecisionTrack(PrecisionTrack::ValueMode, this);
    m_valueTrack->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    valLayout->addWidget(lVal);
    valLayout->addWidget(m_valueTrack, 1);

    QVBoxLayout *alpLayout = new QVBoxLayout();
    alpLayout->setSpacing(4);
    QLabel *lAlp = new QLabel("OPC", this);
    lAlp->setAlignment(Qt::AlignCenter);
    lAlp->setStyleSheet(QString("font-size: 11px; font-weight: 700; color: %1; font-family: 'Consolas', monospace; background: transparent;").arg(td.name()));
    m_alphaTrack = new PrecisionTrack(PrecisionTrack::AlphaMode, this);
    m_alphaTrack->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    alpLayout->addWidget(lAlp);
    alpLayout->addWidget(m_alphaTrack, 1);

    tracksLayout->addLayout(valLayout);
    tracksLayout->addLayout(alpLayout);
    interactiveLayout->addLayout(tracksLayout);
    mainLayout->addLayout(interactiveLayout);

    QFrame *gridFrame = new QFrame(this);
    gridFrame->setObjectName("dataGridFrame");
    gridFrame->setStyleSheet("QFrame { background: transparent; border: none; }");

    QGridLayout *dataGrid = new QGridLayout(gridFrame);
    dataGrid->setContentsMargins(8, 8, 8, 8);
    dataGrid->setSpacing(10);

    m_scrubH = new ScrubberInput(QString::fromUtf8("HUE\xC2\xB0"), 0, 359, 0, this);
    m_scrubS = new ScrubberInput("SAT%",    0, 100, 0, this);
    m_scrubV = new ScrubberInput("VAL%",    0, 100, 0, this);
    m_scrubR = new ScrubberInput("RED",     0, 255, 0, this);
    m_scrubG = new ScrubberInput("GRN",     0, 255, 0, this);
    m_scrubB = new ScrubberInput("BLU",     0, 255, 0, this);

    dataGrid->addWidget(m_scrubH, 0, 0);
    dataGrid->addWidget(m_scrubS, 0, 1);
    dataGrid->addWidget(m_scrubV, 0, 2);
    dataGrid->addWidget(m_scrubR, 1, 0);
    dataGrid->addWidget(m_scrubG, 1, 1);
    dataGrid->addWidget(m_scrubB, 1, 2);
    mainLayout->addWidget(gridFrame);

    QHBoxLayout *actionLayout = new QHBoxLayout();

    QPushButton *btnApply = new QPushButton(tr("Apply Color"), this);
    btnApply->setProperty("class", "primary");
    btnApply->setFixedHeight(32);
    btnApply->setFixedWidth(120);
    connect(btnApply, &QPushButton::clicked, this, [this]() {
        m_oldColor = m_color;
        m_splitPreview->setOldColor(m_oldColor);
        emit colorApplied(m_color);
    });

    actionLayout->addStretch();
    actionLayout->addWidget(btnApply);
    mainLayout->addLayout(actionLayout);

    connect(m_wheel, &ColorWheel::colorChanged, this, &StudioColorPalette::updateFromWheel);
    connect(m_valueTrack, &PrecisionTrack::valueChanged, this, &StudioColorPalette::updateFromValueTrack);
    connect(m_alphaTrack, &PrecisionTrack::valueChanged, this, &StudioColorPalette::updateFromAlphaTrack);

    connect(m_scrubH, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromHSV);
    connect(m_scrubS, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromHSV);
    connect(m_scrubV, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromHSV);
    connect(m_scrubR, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromRGB);
    connect(m_scrubG, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromRGB);
    connect(m_scrubB, &ScrubberInput::valueChanged, this, &StudioColorPalette::updateFromRGB);
}

bool StudioColorPalette::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_hexLabel && event->type() == QEvent::MouseButtonPress)
    {
        showHexEdit();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void StudioColorPalette::mousePressEvent(QMouseEvent *)
{
    if (m_hexStack->currentIndex() == 1)
        commitHexEdit();
    setFocus();
}

void StudioColorPalette::showHexEdit()
{
    m_hexEdit->setText(m_color.name().toUpper());
    m_hexStack->setCurrentIndex(1);
    m_hexEdit->setFocus();
    m_hexEdit->selectAll();
    updateHexWidth();
}

void StudioColorPalette::commitHexEdit()
{
    if (m_updating) return;
    QString text = m_hexEdit->text().trimmed();
    QColor c(text);
    if (c.isValid())
    {
        c.setAlpha(m_color.alpha());
        m_color = c;
        syncAll(true, true, true, true);
    }
    m_hexStack->setCurrentIndex(0);
}

void StudioColorPalette::cancelHexEdit()
{
    m_hexStack->setCurrentIndex(0);
}

void StudioColorPalette::updateHexWidth()
{
    QFontMetrics fm(m_hexLabel->font());
    int textW = fm.horizontalAdvance(m_hexLabel->text());
    m_hexStack->setFixedWidth(textW + 16);
}

QColor StudioColorPalette::color() const { return m_color; }

void StudioColorPalette::setColor(const QColor &color)
{
    m_color = color;
    syncAll(true, true, true, true);
}

void StudioColorPalette::setOldColor(const QColor &color)
{
    m_oldColor = color;
    m_splitPreview->setOldColor(color);
}

void StudioColorPalette::updateFromWheel()
{
    if (m_updating) return;
    m_color = m_wheel->color();
    m_color.setAlpha(m_alphaTrack->value());
    syncAll(false, true, true, true);
}

void StudioColorPalette::updateFromValueTrack(int v)
{
    if (m_updating) return;
    qreal h = m_color.hueF() < 0 ? m_wheel->hue() : m_color.hueF();
    qreal s = m_color.saturationF();
    m_color = QColor::fromHsvF(h, s, v / 255.0, m_alphaTrack->value() / 255.0);
    m_wheel->setValueOnly(v);
    syncAll(false, true, true, true);
}

void StudioColorPalette::updateFromAlphaTrack(int a)
{
    if (m_updating) return;
    m_color.setAlpha(a);
    syncAll(false, false, false, false);
}

void StudioColorPalette::updateFromHSV()
{
    if (m_updating) return;
    int h = m_scrubH->value();
    int s = m_scrubS->value();
    int v = m_scrubV->value();
    m_color.setHsv(h, s * 255 / 100, v * 255 / 100, m_color.alpha());
    if (s > 0)
        syncAll(true, true, false, true);
    else
        syncAll(false, true, false, true);
}

void StudioColorPalette::updateFromRGB()
{
    if (m_updating) return;
    int r = m_scrubR->value();
    int g = m_scrubG->value();
    int b = m_scrubB->value();
    m_color.setRgb(r, g, b, m_color.alpha());
    syncAll(m_color.saturation() > 0, true, true, false);
}

void StudioColorPalette::syncAll(bool updateWheel, bool updateTracks,
                                 bool updateHSVScrubbers, bool updateRGBScrubbers)
{
    m_updating = true;

    if (updateWheel)
        m_wheel->setColor(m_color);

    if (updateTracks)
    {
        m_valueTrack->setValue(m_color.value());
        m_valueTrack->setBaseColor(m_color);
        m_alphaTrack->setValue(m_color.alpha());
        m_alphaTrack->setBaseColor(m_color);
    }

    if (updateHSVScrubbers)
    {
        int hue = m_color.saturation() > 0 ? m_color.hue() : qRound(m_wheel->hue() * 359);
        m_scrubH->setValue(qMax(0, hue));
        m_scrubS->setValue(qRound(m_color.saturationF() * 100));
        m_scrubV->setValue(qRound(m_color.valueF() * 100));
    }

    if (updateRGBScrubbers)
    {
        m_scrubR->setValue(m_color.red());
        m_scrubG->setValue(m_color.green());
        m_scrubB->setValue(m_color.blue());
    }

    updateHexDisplay();
    updatePreviews();

    m_updating = false;
}

void StudioColorPalette::updateHexDisplay()
{
    if (m_hexStack->currentIndex() == 0)
        m_hexLabel->setText(m_color.name().toUpper());
    updateHexWidth();
}

void StudioColorPalette::updatePreviews()
{
    m_splitPreview->setNewColor(m_color);
}
