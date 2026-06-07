#ifndef BALL_TAB_H
#define BALL_TAB_H

#include <QWidget>
#include <QColor>

class QLabel;
class ToggleSwitch;
class ModernSpinBox;
class ModernDoubleSpinBox;
class DoubleRangeSlider;
class ColorButton;

class BallTab : public QWidget
{
    Q_OBJECT
public:
    explicit BallTab(QWidget *parent = nullptr);
    void reload();
    void applySettings();

signals:
    void previewBallChanged();

private slots:
    void updateGlmLabels();
    void updateDsLabels();
    void updateDsUsdLabels();
    void applyPreview();

private:
    ModernSpinBox *m_ballSizeSpin;
    ModernSpinBox *m_ballBgOpacitySpin;
    ModernSpinBox *m_ringWidthSpin;
    ModernSpinBox *m_pctFontSpin;
    ModernSpinBox *m_timeFontSpin;
    ColorButton *m_pctColorBtn;
    ColorButton *m_timeColorBtn;
    ColorButton *m_ballBgColorBtn;
    QColor m_chosenPctColor;
    QColor m_chosenTimeColor;

    DoubleRangeSlider *m_glmRangeSlider;
    QLabel *m_glmGreenLabel;
    QLabel *m_glmYellowLabel;
    QLabel *m_glmRedLabel;
    ColorButton *m_glmGreenColorBtn;
    ColorButton *m_glmYellowColorBtn;
    ColorButton *m_glmRedColorBtn;

    DoubleRangeSlider *m_dsRangeSlider;
    QLabel *m_dsGreenLabel;
    QLabel *m_dsYellowLabel;
    QLabel *m_dsRedLabel;
    ModernDoubleSpinBox *m_dsTotalBalanceSpin;
    ColorButton *m_dsGreenColorBtn;
    ColorButton *m_dsYellowColorBtn;
    ColorButton *m_dsRedColorBtn;

    ToggleSwitch *m_dsShowUSDCheck;
    QWidget *m_dsUsdWidget;
    ModernDoubleSpinBox *m_dsUsdTotalBalanceSpin;
    DoubleRangeSlider *m_dsUsdRangeSlider;
    QLabel *m_dsUsdGreenLabel;
    QLabel *m_dsUsdYellowLabel;
    QLabel *m_dsUsdRedLabel;
    ColorButton *m_dsUsdGreenColorBtn;
    ColorButton *m_dsUsdYellowColorBtn;
    ColorButton *m_dsUsdRedColorBtn;
};

#endif // BALL_TAB_H
