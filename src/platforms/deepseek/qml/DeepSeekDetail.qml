import QtQuick
import QtQuick.Layouts
import "../../../qml"

ColumnLayout {
    id: rootContent
    property var platformData
    Layout.fillWidth: true
    spacing: Theme.spacingLarge

    function fmtComma(n) {
        if (n === undefined || n === null) return "0"
        var s = "" + n
        var neg = s.charAt(0) === '-'
        if (neg) s = s.substring(1)
        var parts = []
        while (s.length > 3) {
            parts.unshift(s.substring(s.length - 3))
            s = s.substring(0, s.length - 3)
        }
        parts.unshift(s)
        return (neg ? "-" : "") + parts.join(",")
    }

    Rectangle {
        Layout.fillWidth: true
        visible: rootContent.platformData !== null && rootContent.platformData !== undefined
        radius: Theme.radiusLarge
        color: Theme.card
        border.width: 1
        border.color: Theme.border
        implicitHeight: headerCol.implicitHeight + 40

        ColumnLayout {
            id: headerCol
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Theme.spacingLarge
            spacing: Theme.spacingMedium

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: rootContent.platformData ? rootContent.platformData.name : ""
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primary
                    Layout.fillWidth: true
                }
                Rectangle {
                    visible: rootContent.platformData
                    color: rootContent.platformData && rootContent.platformData.valid
                           ? Theme.okLight : Theme.badLight
                    radius: 6
                    implicitWidth: 44
                    implicitHeight: 22
                    Text {
                        anchors.centerIn: parent
                        text: rootContent.platformData && rootContent.platformData.valid
                              ? qsTr("正常") : qsTr("异常")
                        font.pixelSize: Theme.fontSizeTiny
                        font.bold: true
                        color: rootContent.platformData && rootContent.platformData.valid
                               ? Theme.ok : Theme.bad
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingMedium
                Text {
                    text: qsTr("主余额") + ": " + (rootContent.platformData
                           ? (rootContent.platformData.primaryBalance || "0.00") : "0.00")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primary
                    font.bold: true
                }
            }
        }
    }

    ColumnLayout {
        visible: rootContent.platformData && rootContent.platformData.quotas
                 && rootContent.platformData.quotas.length > 0
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        Text {
            text: qsTr("余额明细")
            font.pixelSize: Theme.fontSizeNormal
            font.bold: true
            color: Theme.text
        }

        Repeater {
            model: rootContent.platformData && rootContent.platformData.quotas
                   ? rootContent.platformData.quotas.length : 0
            delegate: Rectangle {
                Layout.fillWidth: true
                implicitHeight: balCol.implicitHeight + 24
                radius: Theme.radiusSmall
                color: Theme.itemBg

                ColumnLayout {
                    id: balCol
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingMedium
                    anchors.rightMargin: Theme.spacingMedium
                    anchors.topMargin: 12
                    anchors.bottomMargin: 12
                    spacing: Theme.spacingTiny

                    property var bq: rootContent.platformData.quotas[modelData]

                    Text {
                        text: balCol.bq ? balCol.bq.type.replace("BALANCE_", "") : ""
                        font.pixelSize: Theme.fontSizeNormal
                        font.bold: true
                        color: Theme.text
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingLarge
                        Text {
                            text: qsTr("总额") + ": " + (balCol.bq
                                   ? (balCol.bq.total / 100).toFixed(2) : "0.00")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.text
                            font.bold: true
                        }
                        Text {
                            text: qsTr("赠金") + ": " + (balCol.bq
                                   ? (balCol.bq.remaining / 100).toFixed(2) : "0.00")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.muted
                        }
                        Text {
                            text: qsTr("充值") + ": " + (balCol.bq
                                   ? (balCol.bq.currentUsage / 100).toFixed(2) : "0.00")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.muted
                        }
                    }
                }
            }
        }
    }
}
