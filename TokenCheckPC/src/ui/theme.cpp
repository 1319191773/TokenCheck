#include "theme.h"

static const ThemePalette s_darkData = {
    {30, 30, 46},       {42, 42, 60},       {49, 49, 71},
    {61, 61, 92},       {69, 71, 90},       {205, 214, 244},
    {108, 112, 134},    {46, 204, 113},     {39, 174, 96},
    {33, 154, 82},      {231, 76, 60},      {192, 57, 43},
    {241, 196, 15},     {69, 71, 90},       {88, 91, 112},
    {30, 30, 46}
};

static const ThemePalette s_lightData = {
    {239, 241, 245},    {230, 233, 239},    {204, 208, 218},
    {188, 192, 204},    {172, 176, 190},    {76, 79, 105},
    {124, 127, 147},    {64, 160, 43},      {54, 133, 30},
    {45, 110, 24},      {210, 15, 57},      {176, 13, 48},
    {223, 142, 29},     {188, 192, 204},    {156, 160, 174},
    {255, 255, 255}
};

const ThemePalette Theme::s_dark = s_darkData;
const ThemePalette Theme::s_light = s_lightData;
ThemeId Theme::s_current = ThemeId::Dark;

QColor Theme::bg(30, 30, 46);
QColor Theme::surface(42, 42, 60);
QColor Theme::surfaceAlt(49, 49, 71);
QColor Theme::text(205, 214, 244);
QColor Theme::textDim(108, 112, 134);
QColor Theme::accent(46, 204, 113);
QColor Theme::accentHover(39, 174, 96);
QColor Theme::danger(231, 76, 60);
QColor Theme::warning(241, 196, 15);
QColor Theme::border(69, 71, 90);
QColor Theme::selection(88, 91, 112);

const ThemePalette &Theme::palette()
{
    return s_current == ThemeId::Dark ? s_dark : s_light;
}

void Theme::setTheme(ThemeId id)
{
    s_current = id;
    const ThemePalette &p = palette();
    bg = p.bg;
    surface = p.surface;
    surfaceAlt = p.surfaceAlt;
    text = p.text;
    textDim = p.textDim;
    accent = p.accent;
    accentHover = p.accentHover;
    danger = p.danger;
    warning = p.warning;
    border = p.border;
    selection = p.borderHover;
}

ThemeId Theme::currentTheme()
{
    return s_current;
}

QString Theme::globalStyleSheet()
{
    return buildQSS(palette());
}

QString Theme::buildQSS(const ThemePalette &p)
{
    QString bg = p.bg.name();
    QString sf = p.surface.name();
    QString sa = p.surfaceAlt.name();
    QString sh = p.surfaceHover.name();
    QString sp = p.surfacePressed.name();
    QString tx = p.text.name();
    QString td = p.textDim.name();
    QString ac = p.accent.name();
    QString ah = p.accentHover.name();
    QString ap = p.accentPressed.name();
    QString dn = p.danger.name();
    QString dh = p.dangerHover.name();
    QString bd = p.border.name();
    QString bh = p.borderHover.name();
    QString pt = p.primaryText.name();

    return

    "QWidget {"
    "  color: " + tx + ";"
    "  font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;"
    "}\n"

    "QDialog {"
    "  background-color: " + bg + ";"
    "}\n"

    "QWidget#MainWindowRoot {"
    "  background-color: " + bg + ";"
    "}\n"

    "QGroupBox {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 8px;"
    "  margin-top: 14px;"
    "  padding: 14px 12px 10px 12px;"
    "  font-weight: bold;"
    "}\n"
    "QGroupBox::title {"
    "  subcontrol-origin: margin;"
    "  subcontrol-position: top left;"
    "  left: 14px;"
    "  padding: 0 6px;"
    "  color: " + tx + ";"
    "}\n"

    "QPushButton {"
    "  background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  padding: 6px 16px;"
    "  min-height: 20px;"
    "  color: " + tx + ";"
    "}\n"
    "QPushButton:hover {"
    "  background-color: " + sh + ";"
    "  border-color: " + bh + ";"
    "}\n"
    "QPushButton:pressed {"
    "  background-color: " + sp + ";"
    "}\n"
    "QPushButton:disabled {"
    "  background-color: " + sf + ";"
    "  color: " + td + ";"
    "}\n"

    "QPushButton[class=\"primary\"] {"
    "  background-color: " + ac + ";"
    "  border-color: " + ah + ";"
    "  color: " + pt + ";"
    "  font-weight: bold;"
    "}\n"
    "QPushButton[class=\"primary\"]:hover {"
    "  background-color: " + ah + ";"
    "}\n"
    "QPushButton[class=\"primary\"]:pressed {"
    "  background-color: " + ap + ";"
    "}\n"

    "QPushButton[class=\"danger\"] {"
    "  background-color: " + dn + ";"
    "  border-color: " + dh + ";"
    "  color: #FFFFFF;"
    "}\n"
    "QPushButton[class=\"danger\"]:hover {"
    "  background-color: " + dh + ";"
    "}\n"

    "QComboBox {"
    "  background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  padding: 5px 10px;"
    "  min-height: 20px;"
    "  color: " + tx + ";"
    "}\n"
    "QComboBox:hover {"
    "  border-color: " + bh + ";"
    "}\n"
    "QComboBox::drop-down {"
    "  subcontrol-origin: padding;"
    "  subcontrol-position: center right;"
    "  width: 24px;"
    "  border: none;"
    "}\n"
    "QComboBox::down-arrow {"
    "  image: none;"
    "  border-left: 5px solid transparent;"
    "  border-right: 5px solid transparent;"
    "  border-top: 6px solid " + tx + ";"
    "  margin-right: 8px;"
    "}\n"
    "QComboBox QAbstractItemView {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 4px;"
    "  selection-background-color: " + sh + ";"
    "  selection-color: " + tx + ";"
    "  color: " + tx + ";"
    "  outline: none;"
    "}\n"

    "QSpinBox {"
    "  background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  padding: 4px 8px;"
    "  min-height: 20px;"
    "  color: " + tx + ";"
    "}\n"
    "QSpinBox:hover {"
    "  border-color: " + bh + ";"
    "}\n"
    "QSpinBox::up-button, QSpinBox::down-button {"
    "  background-color: " + sh + ";"
    "  border: none;"
    "  width: 18px;"
    "}\n"
    "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
    "  background-color: " + sp + ";"
    "}\n"
    "QSpinBox::up-arrow {"
    "  border-left: 4px solid transparent;"
    "  border-right: 4px solid transparent;"
    "  border-bottom: 5px solid " + tx + ";"
    "}\n"
    "QSpinBox::down-arrow {"
    "  border-left: 4px solid transparent;"
    "  border-right: 4px solid transparent;"
    "  border-top: 5px solid " + tx + ";"
    "}\n"

    "QLineEdit {"
    "  background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  padding: 5px 10px;"
    "  min-height: 20px;"
    "  color: " + tx + ";"
    "}\n"
    "QLineEdit:hover {"
    "  border-color: " + bh + ";"
    "}\n"
    "QLineEdit:focus {"
    "  border-color: " + ac + ";"
    "}\n"

    "QCheckBox {"
    "  spacing: 8px;"
    "  color: " + tx + ";"
    "}\n"
    "QCheckBox::indicator {"
    "  width: 18px;"
    "  height: 18px;"
    "  border: 2px solid " + bd + ";"
    "  border-radius: 4px;"
    "  background-color: " + sa + ";"
    "}\n"
    "QCheckBox::indicator:hover {"
    "  border-color: " + bh + ";"
    "}\n"
    "QCheckBox::indicator:checked {"
    "  background-color: " + ac + ";"
    "  border-color: " + ac + ";"
    "}\n"

    "QTabWidget::pane {"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 4px;"
    "  background-color: " + bg + ";"
    "  top: -1px;"
    "}\n"
    "QTabBar::tab {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-bottom: none;"
    "  border-top-left-radius: 6px;"
    "  border-top-right-radius: 6px;"
    "  padding: 8px 20px;"
    "  margin-right: 2px;"
    "  color: " + td + ";"
    "}\n"
    "QTabBar::tab:hover {"
    "  background-color: " + sa + ";"
    "  color: " + tx + ";"
    "}\n"
    "QTabBar::tab:selected {"
    "  background-color: " + bg + ";"
    "  color: " + ac + ";"
    "  font-weight: bold;"
    "  border-bottom: 2px solid " + ac + ";"
    "}\n"

    "QProgressBar {"
    "  background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 9px;"
    "  text-align: center;"
    "  color: " + tx + ";"
    "  font-size: 11px;"
    "  min-height: 18px;"
    "  max-height: 18px;"
    "}\n"
    "QProgressBar::chunk {"
    "  border-radius: 8px;"
    "}\n"

    "QTableWidget {"
    "  background-color: " + sf + ";"
    "  alternate-background-color: " + sa + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  gridline-color: " + sh + ";"
    "  color: " + tx + ";"
    "  selection-background-color: " + sh + ";"
    "  selection-color: " + tx + ";"
    "  outline: none;"
    "}\n"
    "QTableWidget::item {"
    "  padding: 4px 8px;"
    "}\n"
    "QTableWidget::item:hover {"
    "  background-color: " + sh + ";"
    "}\n"
    "QHeaderView::section {"
    "  background-color: " + sa + ";"
    "  color: " + td + ";"
    "  border: none;"
    "  border-bottom: 2px solid " + bd + ";"
    "  padding: 6px 8px;"
    "  font-weight: bold;"
    "}\n"

    "QListWidget {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  color: " + tx + ";"
    "  outline: none;"
    "}\n"
    "QListWidget::item {"
    "  padding: 8px 12px;"
    "  border-bottom: 1px solid " + sh + ";"
    "}\n"
    "QListWidget::item:hover {"
    "  background-color: " + sa + ";"
    "}\n"
    "QListWidget::item:selected {"
    "  background-color: " + sh + ";"
    "  border-left: 3px solid " + ac + ";"
    "}\n"

    "QScrollBar:vertical {"
    "  background-color: transparent;"
    "  width: 8px;"
    "  margin: 0;"
    "}\n"
    "QScrollBar::handle:vertical {"
    "  background-color: " + bd + ";"
    "  border-radius: 4px;"
    "  min-height: 30px;"
    "}\n"
    "QScrollBar::handle:vertical:hover {"
    "  background-color: " + bh + ";"
    "}\n"
    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
    "  height: 0;"
    "}\n"
    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
    "  background: none;"
    "}\n"

    "QScrollBar:horizontal {"
    "  background-color: transparent;"
    "  height: 8px;"
    "  margin: 0;"
    "}\n"
    "QScrollBar::handle:horizontal {"
    "  background-color: " + bd + ";"
    "  border-radius: 4px;"
    "  min-width: 30px;"
    "}\n"
    "QScrollBar::handle:horizontal:hover {"
    "  background-color: " + bh + ";"
    "}\n"
    "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
    "  width: 0;"
    "}\n"
    "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
    "  background: none;"
    "}\n"

    "QMenu {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 8px;"
    "  padding: 6px 0;"
    "}\n"
    "QMenu::item {"
    "  padding: 6px 28px 6px 20px;"
    "  color: " + tx + ";"
    "}\n"
    "QMenu::item:selected {"
    "  background-color: " + sh + ";"
    "  border-radius: 4px;"
    "  margin: 0 4px;"
    "  padding: 6px 24px 6px 16px;"
    "}\n"
    "QMenu::separator {"
    "  height: 1px;"
    "  background-color: " + bd + ";"
    "  margin: 4px 12px;"
    "}\n"
    "QMenu::indicator {"
    "  width: 16px;"
    "  height: 16px;"
    "  margin-left: 4px;"
    "}\n"

    "QToolTip {"
    "  background-color: " + sf + ";"
    "  color: " + tx + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 6px;"
    "  padding: 6px 10px;"
    "}\n"

    "QScrollArea {"
    "  background-color: transparent;"
    "  border: none;"
    "}\n"
    "QScrollArea > QWidget > QWidget {"
    "  background-color: transparent;"
    "}\n"

    "QDialogButtonBox QPushButton {"
    "  min-width: 80px;"
    "}\n"

    "QLabel {"
    "  color: " + tx + ";"
    "}\n"
    "QLabel[class=\"dim\"] {"
    "  color: " + td + ";"
    "}\n"
    "QLabel[class=\"status-ok\"] {"
    "  color: " + ac + ";"
    "  font-weight: bold;"
    "}\n"
    "QLabel[class=\"status-err\"] {"
    "  color: " + dn + ";"
    "  font-weight: bold;"
    "}\n"
    "QLabel[class=\"stat-value\"] {"
    "  font-size: 16px;"
    "  font-weight: bold;"
    "  color: " + tx + ";"
    "}\n"
    "QLabel[class=\"stat-label\"] {"
    "  font-size: 11px;"
    "  color: " + td + ";"
    "}\n"

    "QFrame[class=\"stat-card\"] {"
    "  background-color: " + sf + ";"
    "  border: 1px solid " + bd + ";"
    "  border-radius: 8px;"
    "  padding: 12px;"
    "}\n";
}
