import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: Theme.bg }

    required property var stackView
    property int tab: 0
    property int interval: appSettings.autoRefreshInterval()
    property int sT: appSettings.widgetShowToken()
    property int sM: appSettings.widgetShowMcp()
    property int sTm: appSettings.widgetShowTime()

    ListModel {
        id: platformListModel
        Component.onCompleted: reloadPlatforms()

        function reloadPlatforms() {
            clear()
            var count = appSettings.platformCount()
            for (var i = 0; i < count; i++) {
                append({
                    "pIndex": i,
                    "pName": appSettings.platformName(i),
                    "pUrl": appSettings.platformBaseUrl(i)
                })
            }
        }
    }

    Connections {
        target: appSettings
        function onPlatformsChanged() { platformListModel.reloadPlatforms() }
    }

    Keys.onBackPressed: root.stackView.pop()

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
                text: qsTr("设置")
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                color: Theme.text
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingLarge
            Layout.topMargin: Theme.spacingLarge
            height: 44
            radius: 10
            color: Theme.border

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 8
                    color: root.tab === 0 ? Theme.card : "transparent"
                    Behavior on color { ColorAnimation { duration: 200 } }

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("平台管理")
                        font.pixelSize: Theme.fontSizeNormal
                        font.bold: root.tab === 0
                        color: root.tab === 0 ? Theme.text : Theme.muted
                    }
                    MouseArea { anchors.fill: parent; onClicked: root.tab = 0 }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 8
                    color: root.tab === 1 ? Theme.card : "transparent"
                    Behavior on color { ColorAnimation { duration: 200 } }

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("桌面组件")
                        font.pixelSize: Theme.fontSizeNormal
                        font.bold: root.tab === 1
                        color: root.tab === 1 ? Theme.text : Theme.muted
                    }
                    MouseArea { anchors.fill: parent; onClicked: root.tab = 1 }
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingLarge
                spacing: Theme.spacingLarge

                ColumnLayout {
                    visible: root.tab === 0
                    Layout.fillWidth: true
                    spacing: Theme.spacingMedium

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: formCol.implicitHeight + 32
                        radius: Theme.radiusLarge
                        color: Theme.card
                        border.width: 1
                        border.color: Theme.border

                        ColumnLayout {
                            id: formCol
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 16
                            spacing: Theme.spacingMedium

                            Text {
                                text: qsTr("选择平台")
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.muted
                            }

                            ComboBox {
                                id: pCombo
                                Layout.fillWidth: true
                                Layout.preferredHeight: 44
                                model: [qsTr("智谱 ZAI"), qsTr("智谱 ZHIPU"), qsTr("自定义")]
                                background: Rectangle {
                                    color: Theme.itemBg
                                    border.width: 1
                                    border.color: Theme.border
                                    radius: Theme.radiusSmall
                                }
                            }

                            Text {
                                text: "Auth Token"
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.muted
                            }

                            TextField {
                                id: tField
                                Layout.fillWidth: true
                                Layout.preferredHeight: 44
                                echoMode: TextInput.Password
                                color: Theme.text
                                background: Rectangle {
                                    color: Theme.itemBg
                                    border.width: 1
                                    border.color: Theme.border
                                    radius: Theme.radiusSmall
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 44
                                background: Rectangle { color: Theme.text; radius: Theme.radiusSmall }
                                contentItem: Text {
                                    text: qsTr("\uFF0B 添加平台")
                                    color: Theme.white
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    if (tField.text.length < 5) return
                                    var n = pCombo.currentIndex === 0 ? "ZAI"
                                          : pCombo.currentIndex === 1 ? "ZHIPU" : "Custom"
                                    appSettings.addPlatform(n, "https://open.bigmodel.cn",
                                                            tField.text, "/api/monitor/usage")
                                    tField.text = ""
                                    usageQuery.query()
                                }
                            }
                        }
                    }

                    Text {
                        text: qsTr("已添加 (%1)").arg(platformListModel.count)
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                        color: Theme.muted
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingSmall

                        Repeater {
                            model: platformListModel
                            delegate: Rectangle {
                                Layout.fillWidth: true
                                height: 64
                                radius: Theme.radiusMedium
                                color: Theme.card
                                border.width: 1
                                border.color: Theme.border

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2
                                        Text {
                                            text: model.pName
                                            font.pixelSize: Theme.fontSizeNormal
                                            font.bold: true
                                            color: Theme.text
                                        }
                                        Text {
                                            text: model.pUrl
                                            font.pixelSize: Theme.fontSizeTiny
                                            color: Theme.muted
                                            elide: Text.ElideRight
                                            wrapMode: Text.NoWrap
                                            Layout.fillWidth: true
                                        }
                                    }

                                    ToolButton {
                                        text: qsTr("编辑")
                                        font.pixelSize: Theme.fontSizeSmall
                                        contentItem: Text {
                                            text: parent.text
                                            font.pixelSize: Theme.fontSizeSmall
                                            color: Theme.primary
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: root.stackView.push(
                                            "PlatformEditPage.qml", {
                                                "stackView": root.stackView,
                                                "editIndex": model.pIndex,
                                                "editName": model.pName,
                                                "editUrl": model.pUrl,
                                                "editToken": appSettings.platformAuthToken(model.pIndex),
                                                "editPrefix": appSettings.platformApiPrefix(model.pIndex),
                                                "editEnabled": appSettings.platformEnabled(model.pIndex)
                                            })
                                    }

                                    ToolButton {
                                        text: "\u2716"
                                        font.pixelSize: 16
                                        contentItem: Text {
                                            text: parent.text
                                            font.pixelSize: 16
                                            color: Theme.bad
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: appSettings.removePlatform(model.pIndex)
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillWidth: true; height: 20 }
                }

                ColumnLayout {
                    visible: root.tab === 1
                    Layout.fillWidth: true
                    spacing: Theme.spacingMedium

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: previewCol.implicitHeight + 32
                        radius: Theme.radiusLarge
                        color: Theme.card
                        border.width: 1
                        border.color: Theme.border

                        ColumnLayout {
                            id: previewCol
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 16
                            spacing: Theme.spacingMedium

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: qsTr("桌面插件预览")
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.bold: true
                                    Layout.fillWidth: true
                                    color: Theme.text
                                }
                                Rectangle {
                                    color: Theme.itemBg
                                    radius: 10
                                    width: 60; height: 20
                                    Text {
                                        anchors.centerIn: parent
                                        text: qsTr("示例")
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: Theme.muted
                                    }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                ColumnLayout {
                                    visible: sT === 1
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "45%"
                                        font.pixelSize: Theme.fontSizeLarge
                                        font.bold: true
                                        color: Theme.ok
                                    }
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "TOKEN"
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: Theme.muted
                                    }
                                }
                                Rectangle {
                                    visible: sT === 1 && sM === 1
                                    width: 1; height: 30; color: Theme.border
                                }
                                ColumnLayout {
                                    visible: sM === 1
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "22%"
                                        font.pixelSize: Theme.fontSizeLarge
                                        font.bold: true
                                        color: Theme.warn
                                    }
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "MCP"
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: Theme.muted
                                    }
                                }
                                Rectangle {
                                    visible: sM === 1 && sTm === 1
                                    width: 1; height: 30; color: Theme.border
                                }
                                ColumnLayout {
                                    visible: sTm === 1
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "14:30"
                                        font.pixelSize: Theme.fontSizeLarge
                                        font.bold: true
                                        color: Theme.text
                                    }
                                    Text {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: qsTr("重置")
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: Theme.muted
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: switchesCol.implicitHeight
                        radius: Theme.radiusLarge
                        color: Theme.card
                        border.width: 1
                        border.color: Theme.border

                        ColumnLayout {
                            id: switchesCol
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 0

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                Layout.leftMargin: 16
                                Layout.rightMargin: 16
                                Text {
                                    text: qsTr("显示 Token 百分比")
                                    font.pixelSize: Theme.fontSizeNormal
                                    color: Theme.text
                                    Layout.fillWidth: true
                                }
                                Rectangle {
                                    radius: 12
                                    width: 44; height: 24
                                    color: root.sT === 1 ? Theme.primary : Theme.border
                                    Rectangle {
                                        radius: 10
                                        width: 20; height: 20
                                        color: Theme.white
                                        y: 2
                                        x: root.sT === 1 ? 22 : 2
                                        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                                    }
                                    MouseArea { anchors.fill: parent; onClicked: root.sT = root.sT === 1 ? 0 : 1 }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                Layout.leftMargin: 16
                                Layout.rightMargin: 16
                                Text {
                                    text: qsTr("显示 MCP 百分比")
                                    font.pixelSize: Theme.fontSizeNormal
                                    color: Theme.text
                                    Layout.fillWidth: true
                                }
                                Rectangle {
                                    radius: 12
                                    width: 44; height: 24
                                    color: root.sM === 1 ? Theme.primary : Theme.border
                                    Rectangle {
                                        radius: 10
                                        width: 20; height: 20
                                        color: Theme.white
                                        y: 2
                                        x: root.sM === 1 ? 22 : 2
                                        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                                    }
                                    MouseArea { anchors.fill: parent; onClicked: root.sM = root.sM === 1 ? 0 : 1 }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                Layout.leftMargin: 16
                                Layout.rightMargin: 16
                                Text {
                                    text: qsTr("显示重置时间")
                                    font.pixelSize: Theme.fontSizeNormal
                                    color: Theme.text
                                    Layout.fillWidth: true
                                }
                                Rectangle {
                                    radius: 12
                                    width: 44; height: 24
                                    color: root.sTm === 1 ? Theme.primary : Theme.border
                                    Rectangle {
                                        radius: 10
                                        width: 20; height: 20
                                        color: Theme.white
                                        y: 2
                                        x: root.sTm === 1 ? 22 : 2
                                        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                                    }
                                    MouseArea { anchors.fill: parent; onClicked: root.sTm = root.sTm === 1 ? 0 : 1 }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                Layout.leftMargin: 16
                                Layout.rightMargin: 16
                                Text {
                                    text: qsTr("刷新间隔 (分钟)")
                                    font.pixelSize: Theme.fontSizeNormal
                                    color: Theme.text
                                    Layout.fillWidth: true
                                }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 1; to: 60; stepSize: 1
                                    value: root.interval
                                    onValueChanged: root.interval = value
                                }
                                Text {
                                    text: root.interval
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.bold: true
                                    color: Theme.muted
                                    Layout.minimumWidth: 20
                                }
                            }
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44
                        background: Rectangle { color: Theme.primary; radius: Theme.radiusSmall }
                        contentItem: Text {
                            text: qsTr("应用插件配置")
                            color: Theme.white
                            font.pixelSize: Theme.fontSizeNormal
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            appSettings.setWidgetShowToken(root.sT)
                            appSettings.setWidgetShowMcp(root.sM)
                            appSettings.setWidgetShowTime(root.sTm)
                            appSettings.setAutoRefreshInterval(root.interval)
                            appSettings.syncWidgetConfig()
                            if (root.interval > 0)
                                usageQuery.setAutoRefresh(root.interval)
                            else
                                usageQuery.stopAutoRefresh()
                            root.stackView.pop()
                        }
                    }

                    Item { Layout.fillWidth: true; height: 20 }
                }
            }
        }
    }
}
