// EyedropperOverlay — 全屏取色放大镜，鼠标移动时实时放大显示，点击拾取像素颜色

#include "EyedropperOverlay.h"

#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCursor>

EyedropperOverlay::EyedropperOverlay(QWidget *parent)
    : QWidget(parent, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setCursor(Qt::BlankCursor);
    setMouseTracking(true);

    m_timer = new QTimer(this);
    m_timer->setInterval(16);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_cursorPos = QCursor::pos();
        update();
    });
}

void EyedropperOverlay::beginPick()
{
    QList<QScreen *> screens = QGuiApplication::screens();
    QRect virtualGeo = QGuiApplication::primaryScreen()->virtualGeometry();

    m_screenCapture = QPixmap(virtualGeo.size());
    m_screenCapture.fill(Qt::black);
    QPainter painter(&m_screenCapture);
    for (QScreen *screen : screens) {
        QPixmap grab = screen->grabWindow(0);
        QRect geo = screen->geometry();
        QPoint offset = geo.topLeft() - virtualGeo.topLeft();
        painter.drawPixmap(offset, grab);
    }
    painter.end();

    m_screenImage = m_screenCapture.toImage();

    setGeometry(virtualGeo);

    m_active = true;
    m_cursorPos = QCursor::pos();

    show();
    raise();
    setFocus();
    update();
    m_timer->start();
}

void EyedropperOverlay::paintEvent(QPaintEvent *)
{
    if (!m_active || m_screenCapture.isNull()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 参数
    const int gridSize = 13;
    const int pixelSize = 16;
    const int magnifierRadius = (gridSize * pixelSize) / 2;
    const int border = 3;

    // 将全局坐标转换为 overlay 本地坐标
    QPointF center(m_cursorPos.x() - geometry().x(),
                   m_cursorPos.y() - geometry().y());

    // 圆形裁剪区域
    QPainterPath clip;
    clip.addEllipse(center, magnifierRadius + border, magnifierRadius + border);
    painter.setClipPath(clip);

    // 绘制放大像素
    int halfGrid = gridSize / 2;
    const QImage &screenImage = m_screenImage;

    for (int dy = 0; dy < gridSize; ++dy)
    {
        for (int dx = 0; dx < gridSize; ++dx)
        {
            int px = m_cursorPos.x() - halfGrid + dx;
            int py = m_cursorPos.y() - halfGrid + dy;
            QColor pixelColor = screenImage.pixelColor(
                qBound(0, px, screenImage.width() - 1),
                qBound(0, py, screenImage.height() - 1));

            qreal drawX = center.x() - magnifierRadius + dx * pixelSize;
            qreal drawY = center.y() - magnifierRadius + dy * pixelSize;

            painter.fillRect(QRectF(drawX, drawY, pixelSize, pixelSize), pixelColor);

            // 网格线
            painter.setPen(QPen(QColor(255, 255, 255, 40), 0.5));
            painter.drawRect(QRectF(drawX, drawY, pixelSize, pixelSize));
        }
    }

    // 高亮中心像素
    painter.setClipping(false);
    qreal cx = center.x() - magnifierRadius + halfGrid * pixelSize;
    qreal cy = center.y() - magnifierRadius + halfGrid * pixelSize;
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRectF(cx, cy, pixelSize, pixelSize));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(QRectF(cx - 1, cy - 1, pixelSize + 2, pixelSize + 2));

    // 圆形边框
    painter.setPen(QPen(QColor(255, 255, 255, 200), border));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, magnifierRadius, magnifierRadius);
    painter.setPen(QPen(QColor(0, 0, 0, 100), 1));
    painter.drawEllipse(center, magnifierRadius + border, magnifierRadius + border);

    // 中心色值色块
    QColor centerColor = screenImage.pixelColor(
        qBound(0, m_cursorPos.x(), screenImage.width() - 1),
        qBound(0, m_cursorPos.y(), screenImage.height() - 1));

    qreal swatchY = center.y() + magnifierRadius + border + 8;
    qreal swatchW = 80;
    qreal swatchH = 24;
    painter.setPen(QPen(QColor(255, 255, 255, 180), 1));
    painter.setBrush(centerColor);
    painter.drawRoundedRect(QRectF(center.x() - swatchW / 2, swatchY, swatchW, swatchH), 4, 4);

    // hex 标签
    painter.setPen(Qt::white);
    QFont font("Consolas", 10);
    painter.setFont(font);
    painter.drawText(QRectF(center.x() - swatchW / 2, swatchY, swatchW, swatchH),
                     Qt::AlignCenter, centerColor.name().toUpper());
}


void EyedropperOverlay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QColor picked = m_screenImage.pixelColor(
            qBound(0, m_cursorPos.x(), m_screenImage.width() - 1),
            qBound(0, m_cursorPos.y(), m_screenImage.height() - 1));

        m_active = false;
        m_timer->stop();
        hide();
        emit colorPicked(picked);
    }
    else if (event->button() == Qt::RightButton)
    {
        m_active = false;
        m_timer->stop();
        hide();
        emit cancelled();
    }
}

void EyedropperOverlay::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        m_active = false;
        m_timer->stop();
        hide();
        emit cancelled();
    }
}
