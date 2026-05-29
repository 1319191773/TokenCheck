import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: Theme.bg }

    required property var stackView
    property int editIndex: -1
    property string editName: ""
    property string editUrl: ""
    property string editToken: ""
    property string editPrefix: ""
    property bool editEnabled: true

    property bool tokenHidden: true
    property string realToken: root.editToken

    function maskToken(token) {
        if (token.length === 0) return ""
        if (token.length <= 8) return "\u2022".repeat(token.length)
        return token.substring(0, 4) + "\u2022".repeat(token.length - 8) + token.substring(token.length - 4)
    }

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
                text: root.editIndex < 0 ? qsTr("添加平台") : qsTr("编辑平台")
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
        contentHeight: editForm.implicitHeight + 40

        ColumnLayout {
            id: editForm
            anchors.fill: parent
            anchors.margins: Theme.spacingLarge
            spacing: Theme.spacingLarge

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                visible: root.editIndex < 0
                Label {
                    text: qsTr("快速选择")
                    color: Theme.muted
                    font.pixelSize: Theme.fontSizeSmall
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSmall
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        text: qsTr("智谱 GLM")
                        background: Rectangle {
                            radius: Theme.radiusSmall
                            color: Theme.primary
                            border.width: 1
                            border.color: Theme.border
                        }
                        contentItem: Text {
                            text: parent.text
                            color: Theme.white
                            font.pixelSize: Theme.fontSizeSmall
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            nameField.text = "智谱 GLM"
                            urlField.text = "https://open.bigmodel.cn"
                            prefixField.text = "/api/monitor/usage"
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        text: "DeepSeek"
                        background: Rectangle {
                            radius: Theme.radiusSmall
                            color: Theme.itemBg
                            border.width: 1
                            border.color: Theme.border
                        }
                        contentItem: Text {
                            text: parent.text
                            color: Theme.text
                            font.pixelSize: Theme.fontSizeSmall
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            nameField.text = "DeepSeek"
                            urlField.text = "https://api.deepseek.com"
                            prefixField.text = ""
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label {
                    text: qsTr("平台名称")
                    color: Theme.muted
                    font.pixelSize: Theme.fontSizeSmall
                }
                TextField {
                    id: nameField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    text: root.editName
                    color: Theme.text
                    background: Rectangle {
                        radius: Theme.radiusSmall
                        color: Theme.itemBg
                        border.width: 1
                        border.color: Theme.border
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label {
                    text: qsTr("API 基地址")
                    color: Theme.muted
                    font.pixelSize: Theme.fontSizeSmall
                }
                TextField {
                    id: urlField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    text: root.editUrl
                    color: Theme.text
                    background: Rectangle {
                        radius: Theme.radiusSmall
                        color: Theme.itemBg
                        border.width: 1
                        border.color: Theme.border
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: "Auth Token"
                        color: Theme.muted
                        font.pixelSize: Theme.fontSizeSmall
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        flat: true
                        Layout.preferredHeight: 28
                        contentItem: Text {
                            text: root.tokenHidden ? qsTr("显示") : qsTr("隐藏")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            root.tokenHidden = !root.tokenHidden
                            tokenArea.text = root.tokenHidden
                                             ? root.maskToken(root.realToken)
                                             : root.realToken
                        }
                    }
                }

                TextArea {
                    id: tokenArea
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    text: root.maskToken(root.realToken)
                    wrapMode: TextEdit.Wrap
                    color: Theme.text
                    font.pixelSize: Theme.fontSizeNormal
                    leftPadding: 12
                    rightPadding: 12
                    topPadding: 10
                    bottomPadding: 10
                    background: Rectangle {
                        radius: Theme.radiusSmall
                        color: Theme.itemBg
                        border.width: 1
                        border.color: Theme.border
                    }
                    onTextChanged: {
                        if (!root.tokenHidden) {
                            root.realToken = tokenArea.text
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Label {
                    text: qsTr("API 路径前缀")
                    color: Theme.muted
                    font.pixelSize: Theme.fontSizeSmall
                }
                TextField {
                    id: prefixField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    text: root.editPrefix
                    color: Theme.text
                    background: Rectangle {
                        radius: Theme.radiusSmall
                        color: Theme.itemBg
                        border.width: 1
                        border.color: Theme.border
                    }
                }
            }

            RowLayout {
                Layout.topMargin: Theme.spacingSmall
                Switch {
                    id: enabledSwitch
                    checked: root.editEnabled
                }
                Label {
                    text: qsTr("启用")
                    color: Theme.text
                }
            }

            Button {
                text: root.editIndex < 0 ? qsTr("添加") : qsTr("保存")
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                Layout.topMargin: Theme.spacingMedium
                highlighted: true
                background: Rectangle {
                    radius: Theme.radiusSmall
                    color: parent.enabled ? Theme.primary : Theme.primaryLight
                }
                contentItem: Text {
                    text: parent.text
                    color: Theme.white
                    font.pixelSize: Theme.fontSizeNormal
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                enabled: nameField.text.length > 0 && root.realToken.length >= 5
                onClicked: {
                    if (root.editIndex < 0) {
                        appSettings.addPlatform(nameField.text, urlField.text,
                                                root.realToken, prefixField.text,
                                                enabledSwitch.checked)
                    } else {
                        appSettings.setPlatform(root.editIndex, nameField.text,
                                                urlField.text, root.realToken,
                                                prefixField.text, enabledSwitch.checked)
                    }
                    root.stackView.pop()
                }
            }

            Item { Layout.fillWidth: true; height: 20 }
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
