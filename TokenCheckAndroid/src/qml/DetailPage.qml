import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: Theme.bg }

    required property var stackView
    required property var platformData

    header: Rectangle {
        color: Theme.card
        height: 70
        border.width: 1
        border.color: Theme.border

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingLarge
            spacing: 10

            ToolButton {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.preferredWidth: 44
                Layout.preferredHeight: 44
                text: "\u2190"
                font.pixelSize: 24
                font.bold: true
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 24
                    color: Theme.muted
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.stackView.pop()
            }
            Text {
                text: qsTr("用量详情")
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                color: Theme.text
                Layout.fillWidth: true
            }
        }
    }

    Flickable {
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: bodyLayout.implicitHeight + 40

        ColumnLayout {
            id: bodyLayout
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Theme.spacingLarge
            spacing: Theme.spacingLarge

            Item { Layout.fillWidth: true; height: 4 }

            Rectangle {
                Layout.fillWidth: true
                visible: root.platformData === null || root.platformData === undefined
                height: 100
                radius: Theme.radiusLarge
                color: Theme.card
                border.width: 1
                border.color: Theme.border

                Text {
                    anchors.centerIn: parent
                    text: qsTr("暂无数据")
                    font.pixelSize: Theme.fontSizeNormal
                    color: Theme.muted
                }
            }

            Loader {
                id: detailLoader
                Layout.fillWidth: true
                visible: root.platformData !== null && root.platformData !== undefined
                source: resolveDetailSource(root.platformData ? root.platformData.platformType : "")
                onLoaded: { item.platformData = root.platformData }
            }

            Item { Layout.fillWidth: true; height: 20 }
        }

        ScrollBar.vertical: ScrollBar {}
    }

    function resolveDetailSource(type) {
        switch (type) {
        case "glm": return "../platforms/glm/qml/GlmDetail.qml"
        case "deepseek": return "../platforms/deepseek/qml/DeepSeekDetail.qml"
        default: return "../platforms/glm/qml/GlmDetail.qml"
        }
    }
}
