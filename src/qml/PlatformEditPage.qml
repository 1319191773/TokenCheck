import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: platformEditPage
    title: "编辑平台"

    property var stackRef: null
    property int editIndex: -1
    property string editName: ""
    property string editUrl: ""
    property string editToken: ""
    property string editPrefix: ""
    property bool editEnabled: true

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: "\u25C0"
                font.pixelSize: 18
                onClicked: stackRef.pop()
            }
            Label {
                text: platformEditPage.title
                font.pixelSize: 18
                color: "#E0E0E0"
                Layout.fillWidth: true
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 14

            Label { text: "平台名称"; color: "#999"; font.pixelSize: 13 }
            TextField {
                id: nameField
                Layout.fillWidth: true
                text: editName
                placeholderText: "例如: 智谱 ZAI"
                color: "#FFF"
                background: Rectangle { radius: 6; color: "#1E1E2E"; border.color: "#444" }
            }

            Label { text: "API 基地址"; color: "#999"; font.pixelSize: 13 }
            TextField {
                id: urlField
                Layout.fillWidth: true
                text: editUrl
                placeholderText: "https://open.bigmodel.cn"
                color: "#FFF"
                background: Rectangle { radius: 6; color: "#1E1E2E"; border.color: "#444" }
            }

            Label { text: "Auth Token"; color: "#999"; font.pixelSize: 13 }
            TextField {
                id: tokenField
                Layout.fillWidth: true
                text: editToken
                placeholderText: "输入 API Token"
                echoMode: TextInput.Password
                color: "#FFF"
                background: Rectangle { radius: 6; color: "#1E1E2E"; border.color: "#444" }
            }
            Button {
                text: tokenField.echoMode === TextInput.Password ? "显示" : "隐藏"
                flat: true
                onClicked: {
                    tokenField.echoMode = tokenField.echoMode === TextInput.Password
                        ? TextInput.Normal : TextInput.Password
                }
            }

            Label { text: "API 路径前缀"; color: "#999"; font.pixelSize: 13 }
            TextField {
                id: prefixField
                Layout.fillWidth: true
                text: editPrefix
                placeholderText: "/api/monitor/usage"
                color: "#FFF"
                background: Rectangle { radius: 6; color: "#1E1E2E"; border.color: "#444" }
            }

            RowLayout {
                Switch { id: enabledSwitch; checked: editEnabled }
                Label { text: "启用"; color: "#CCC" }
            }

            Item { Layout.fillHeight: true }

            Button {
                text: editIndex < 0 ? "添加" : "保存"
                Layout.fillWidth: true
                highlighted: true
                onClicked: {
                    if (editIndex < 0) {
                        appSettings.addPlatform(nameField.text, urlField.text,
                                                 tokenField.text, prefixField.text)
                    } else {
                        appSettings.setPlatform(editIndex, nameField.text, urlField.text,
                                                tokenField.text, prefixField.text,
                                                enabledSwitch.checked)
                    }
                    stackRef.pop()
                }
            }
        }
    }
}
