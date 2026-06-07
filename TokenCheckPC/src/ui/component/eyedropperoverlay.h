#ifndef EYEDROPPEROVERLAY_H
#define EYEDROPPEROVERLAY_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>

// 全屏置顶的屏幕取色器：截屏后放大显示鼠标周围像素，左键取色，右键/Esc 取消
class EyedropperOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit EyedropperOverlay(QWidget *parent = nullptr);

    void beginPick();

signals:
    void colorPicked(const QColor &color);
    void cancelled();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPixmap m_screenCapture;
    QImage m_screenImage;
    QPoint m_cursorPos;
    bool m_active = false;
    QTimer *m_timer;
};

#endif // EYEDROPPEROVERLAY_H
