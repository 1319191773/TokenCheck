#ifndef STUDIOCOLORPALETTE_H
#define STUDIOCOLORPALETTE_H

#include <QWidget>
#include <QColor>

class QLabel;
class QLineEdit;
class QPushButton;
class QFrame;
class QStackedWidget;

class SplitPreview : public QWidget
{
    Q_OBJECT
public:
    explicit SplitPreview(QWidget *parent = nullptr);

    void setOldColor(const QColor &c);
    void setNewColor(const QColor &c);

signals:
    void oldColorClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_oldColor;
    QColor m_newColor;
};

class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(int size = 220, QWidget *parent = nullptr);

    void setColor(const QColor &color);
    void setValueOnly(int v) { m_value = v; }
    QColor color() const;
    qreal hue() const { return m_hue; }
    qreal saturation() const { return m_saturation; }

signals:
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void pickColor(const QPoint &pos);
    qreal m_hue = 0.0;
    qreal m_saturation = 1.0;
    int   m_value = 255;
    QPixmap m_wheelCache;
    bool m_cacheDirty = true;
};

class PrecisionTrack : public QWidget
{
    Q_OBJECT
public:
    enum TrackMode { ValueMode, AlphaMode };

    explicit PrecisionTrack(TrackMode mode, QWidget *parent = nullptr);

    void setValue(int val);
    int value() const { return m_value; }
    void setBaseColor(const QColor &color);

signals:
    void valueChanged(int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void pickValue(const QPoint &pos);
    void drawCheckerboard(QPainter &painter, const QRect &rect);

    TrackMode m_mode;
    int m_value = 255;
    QColor m_baseColor;
};

class ScrubberInput : public QWidget
{
    Q_OBJECT
public:
    explicit ScrubberInput(const QString &label, int min, int max,
                           int value, QWidget *parent = nullptr);

    int value() const;
    void setValue(int val);

signals:
    void valueChanged(int value);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateStyleSheet(bool hover);

    QLabel *m_labelTitle;
    QLineEdit *m_editValue;
    int m_min, m_max;
    int m_lastX = 0;
    bool m_dragging = false;
};

class StudioColorPalette : public QWidget
{
    Q_OBJECT
public:
    explicit StudioColorPalette(QWidget *parent = nullptr);

    QColor color() const;
    void setColor(const QColor &color);
    void setOldColor(const QColor &color);

signals:
    void colorApplied(const QColor &color);

private:
    void setupUI();
    void updateFromWheel();
    void updateFromValueTrack(int v);
    void updateFromAlphaTrack(int a);
    void updateFromHSV();
    void updateFromRGB();
    void syncAll(bool updateWheel, bool updateTracks,
                 bool updateHSVScrubbers, bool updateRGBScrubbers);
    void updateHexDisplay();
    void updatePreviews();

    void showHexEdit();
    void commitHexEdit();
    void cancelHexEdit();
    void updateHexWidth();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void applyThemeColors();

    QColor m_color;
    QColor m_oldColor;

    SplitPreview *m_splitPreview;
    QStackedWidget *m_hexStack;
    QLabel *m_hexLabel;
    QLineEdit *m_hexEdit;

    ColorWheel *m_wheel;
    PrecisionTrack *m_valueTrack;
    PrecisionTrack *m_alphaTrack;

    ScrubberInput *m_scrubH;
    ScrubberInput *m_scrubS;
    ScrubberInput *m_scrubV;
    ScrubberInput *m_scrubR;
    ScrubberInput *m_scrubG;
    ScrubberInput *m_scrubB;

    bool m_updating = false;
};

#endif // STUDIOCOLORPALETTE_H
