#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

enum class ThemeId {
    Dark,
    Light
};

struct ThemePalette {
    QColor bg;
    QColor surface;
    QColor surfaceAlt;
    QColor surfaceHover;
    QColor surfacePressed;
    QColor text;
    QColor textDim;
    QColor accent;
    QColor accentHover;
    QColor accentPressed;
    QColor danger;
    QColor dangerHover;
    QColor warning;
    QColor border;
    QColor borderHover;
    QColor primaryText;
};

struct Theme {
    static QColor bg;
    static QColor surface;
    static QColor surfaceAlt;
    static QColor text;
    static QColor textDim;
    static QColor accent;
    static QColor accentHover;
    static QColor danger;
    static QColor warning;
    static QColor border;
    static QColor selection;

    static void setTheme(ThemeId id);
    static ThemeId currentTheme();
    static QString globalStyleSheet();

    static const ThemePalette &palette();

private:
    static ThemeId s_current;
    static const ThemePalette s_dark;
    static const ThemePalette s_light;
    static QString buildQSS(const ThemePalette &p);
};

#endif
