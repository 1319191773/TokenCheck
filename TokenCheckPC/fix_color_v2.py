import re

with open('T:/GitHubDemo/TokenCheck/TokenCheckPC/src/ui/component/studiocolorpalette.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Restore constructor
content = content.replace('setFixedHeight(24);\n    setMinimumWidth(200);\n    setCursor(Qt::SizeHorCursor);',
                          'setFixedWidth(24);\n    setMinimumHeight(100);\n    setCursor(Qt::SizeVerCursor);')

# 2. Restore paintEvent with improved visual design (wider track, better thumb)
paint_event_new = """void PrecisionTrack::paintEvent(QPaintEvent *)
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
}"""

content = re.sub(r'void PrecisionTrack::paintEvent\(QPaintEvent \*\).*?void PrecisionTrack::mousePressEvent', 
                 paint_event_new + '\n\nvoid PrecisionTrack::mousePressEvent', 
                 content, flags=re.DOTALL)

# 3. Restore pickValue for Vertical
pick_value_new = """void PrecisionTrack::pickValue(const QPoint &pos)
{
    int thumbH = 6;
    int margin = thumbH;
    int minY = margin;
    int maxY = height() - margin;
    qreal ratio = 1.0 - qBound(0.0, (qreal)(pos.y() - minY) / (maxY - minY), 1.0);
    setValue(qRound(ratio * 255));
    emit valueChanged(m_value);
}"""

content = re.sub(r'void PrecisionTrack::pickValue\(const QPoint &pos\).*?void ScrubberInput::updateStyleSheet',
                 pick_value_new + '\n\nvoid ScrubberInput::updateStyleSheet',
                 content, flags=re.DOTALL)

# 4. Restore the layout in setupUI
old_layout_code = """    QVBoxLayout *interactiveLayout = new QVBoxLayout();
    interactiveLayout->setSpacing(12);
    interactiveLayout->setAlignment(Qt::AlignCenter);

    m_wheel = new ColorWheel(220, this);
    interactiveLayout->addWidget(m_wheel, 0, Qt::AlignCenter);

    QVBoxLayout *tracksLayout = new QVBoxLayout();
    tracksLayout->setContentsMargins(0, 8, 0, 0);
    tracksLayout->setSpacing(8);

    QHBoxLayout *valLayout = new QHBoxLayout();
    valLayout->setSpacing(8);
    QLabel *lVal = new QLabel("VAL", this);
    lVal->setFixedWidth(28);
    lVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lVal->setStyleSheet(QString("font-size: 11px; font-weight: 700; color: %1; font-family: 'Consolas', monospace; background: transparent;").arg(td.name()));
    m_valueTrack = new PrecisionTrack(PrecisionTrack::ValueMode, this);
    m_valueTrack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valLayout->addWidget(lVal);
    valLayout->addWidget(m_valueTrack, 1);

    QHBoxLayout *alpLayout = new QHBoxLayout();
    alpLayout->setSpacing(8);
    QLabel *lAlp = new QLabel("OPC", this);
    lAlp->setFixedWidth(28);
    lAlp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lAlp->setStyleSheet(QString("font-size: 11px; font-weight: 700; color: %1; font-family: 'Consolas', monospace; background: transparent;").arg(td.name()));
    m_alphaTrack = new PrecisionTrack(PrecisionTrack::AlphaMode, this);
    m_alphaTrack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    alpLayout->addWidget(lAlp);
    alpLayout->addWidget(m_alphaTrack, 1);

    tracksLayout->addLayout(valLayout);
    tracksLayout->addLayout(alpLayout);
    interactiveLayout->addLayout(tracksLayout);
    mainLayout->addLayout(interactiveLayout);"""


new_layout_code = """    QHBoxLayout *interactiveLayout = new QHBoxLayout();
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
    mainLayout->addLayout(interactiveLayout);"""

content = content.replace(old_layout_code, new_layout_code)

with open('T:/GitHubDemo/TokenCheck/TokenCheckPC/src/ui/component/studiocolorpalette.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
