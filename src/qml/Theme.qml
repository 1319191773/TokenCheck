pragma Singleton
import QtQuick

QtObject {
    readonly property color bg: "#F3F4F6"
    readonly property color card: "#FFFFFF"
    readonly property color itemBg: "#F9FAFB"
    readonly property color primary: "#2563EB"
    readonly property color primaryLight: "#93C5FD"
    readonly property color ok: "#10B981"
    readonly property color okLight: "#ECFDF5"
    readonly property color warn: "#F59E0B"
    readonly property color bad: "#F43F5E"
    readonly property color badLight: "#FEF2F2"
    readonly property color badMid: "#FEE2E2"
    readonly property color text: "#111827"
    readonly property color muted: "#6B7280"
    readonly property color border: "#E5E7EB"
    readonly property color white: "#FFFFFF"

    readonly property int radiusSmall: 8
    readonly property int radiusMedium: 12
    readonly property int radiusLarge: 16

    readonly property int spacingTiny: 4
    readonly property int spacingSmall: 8
    readonly property int spacingMedium: 14
    readonly property int spacingLarge: 20

    readonly property int fontSizeTiny: 10
    readonly property int fontSizeSmall: 12
    readonly property int fontSizeNormal: 14
    readonly property int fontSizeMedium: 16
    readonly property int fontSizeLarge: 18
    readonly property int fontSizeTitle: 20

    function pctColor(p) {
        if (p >= 80) return bad
        if (p >= 50) return warn
        return ok
    }

    function fmtPct(p) {
        return p < 0 ? "--%" : p.toFixed(1) + "%"
    }
}
