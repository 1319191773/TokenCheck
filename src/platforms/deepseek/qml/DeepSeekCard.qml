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

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: qsTr("余额")
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.muted
        }
        Text {
            text: cardData.primaryBalance || "--"
            font.pixelSize: Theme.fontSizeNormal
            font.bold: true
            color: Theme.primary
            Layout.fillWidth: true
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
            text: qsTr("查看详情")
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
