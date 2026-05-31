import QtQuick
import QtQuick.Layouts
import "../../../qml"

ColumnLayout {
    property var cardData
    Layout.fillWidth: true
    spacing: Theme.spacingMedium

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: cardData.name
            font.pixelSize: Theme.fontSizeLarge
            font.bold: true
            color: Theme.text
            Layout.fillWidth: true
        }
        Rectangle {
            radius: 6
            implicitHeight: 22
            implicitWidth: statusLabel.implicitWidth + 16
            color: cardData.valid ? Theme.okLight : Theme.badMid
            Text {
                id: statusLabel
                anchors.centerIn: parent
                text: cardData.valid ? qsTr("已连接") : qsTr("未连接")
                font.pixelSize: Theme.fontSizeTiny
                font.bold: true
                color: cardData.valid ? Theme.ok : Theme.bad
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingTiny
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "Token"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.muted
                Layout.fillWidth: true
            }
            Text {
                text: Theme.fmtPct(cardData.tokenPct)
                font.pixelSize: Theme.fontSizeSmall
                font.bold: true
                color: Theme.pctColor(cardData.tokenPct)
            }
        }
        Rectangle {
            Layout.fillWidth: true
            height: 6
            radius: 3
            color: Theme.border
            Rectangle {
                height: 6
                radius: 3
                color: Theme.pctColor(cardData.tokenPct)
                width: parent.width * Math.min(Math.max(cardData.tokenPct, 0), 100) / 100
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingTiny
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "MCP"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.muted
                Layout.fillWidth: true
            }
            Text {
                text: Theme.fmtPct(cardData.mcpPct)
                font.pixelSize: Theme.fontSizeSmall
                font.bold: true
                color: Theme.pctColor(cardData.mcpPct)
            }
        }
        Rectangle {
            Layout.fillWidth: true
            height: 6
            radius: 3
            color: Theme.border
            Rectangle {
                height: 6
                radius: 3
                color: Theme.pctColor(cardData.mcpPct)
                width: parent.width * Math.min(Math.max(cardData.mcpPct, 0), 100) / 100
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true
        height: 1
        color: Theme.border
    }

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: qsTr("重置") + " " + (cardData.resetTime || "--")
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.muted
            Layout.fillWidth: true
        }
        Text {
            text: ">"
            font.pixelSize: 14
            color: Theme.muted
        }
    }
}
