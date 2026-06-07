#include "theme.h"
#include <QFile>
#include <QTextStream>

static const ThemePalette s_darkData = {
    {10, 10, 10},       {20, 20, 20},       {30, 30, 30},
    {38, 38, 38},       {46, 46, 46},       {237, 237, 237},
    {161, 161, 170},    {94, 106, 210},     {111, 123, 227},
    {83, 93, 185},      {239, 68, 68},      {248, 113, 113},
    {245, 158, 11},     {38, 38, 38},       {64, 64, 64},
    {255, 255, 255}
};

static const ThemePalette s_lightData = {
    {250, 250, 250},    {255, 255, 255},    {244, 244, 245},
    {228, 228, 231},    {212, 212, 216},    {24, 24, 27},
    {113, 113, 122},    {79, 70, 229},      {99, 102, 241},
    {67, 56, 202},      {220, 38, 38},      {239, 68, 68},
    {217, 119, 6},      {228, 228, 231},    {212, 212, 216},
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
    QFile file(":/style.qss");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }
    QTextStream in(&file);
    QString qss = in.readAll();
    file.close();

    qss.replace("@bg@", p.bg.name());
    qss.replace("@sf@", p.surface.name());
    qss.replace("@sa@", p.surfaceAlt.name());
    qss.replace("@sh@", p.surfaceHover.name());
    qss.replace("@sp@", p.surfacePressed.name());
    qss.replace("@tx@", p.text.name());
    qss.replace("@td@", p.textDim.name());
    qss.replace("@ac@", p.accent.name());
    qss.replace("@ah@", p.accentHover.name());
    qss.replace("@ap@", p.accentPressed.name());
    qss.replace("@dn@", p.danger.name());
    qss.replace("@dh@", p.dangerHover.name());
    qss.replace("@bd@", p.border.name());
    qss.replace("@bh@", p.borderHover.name());
    qss.replace("@pt@", p.primaryText.name());

    return qss;
}
