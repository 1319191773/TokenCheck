import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: Theme.bg }

    required property var stackView
    required property var platformData
    required property bool isRefreshing
    property string lastError: ""

    signal refreshRequested()
    signal platformClicked(int index)
    signal errorDismissed()

    property real pullProgress: 0
    property bool pullTriggered: false

    Keys.onBackPressed: {
        if (root.stackView.depth > 1)
            root.stackView.pop()
        else
            Qt.quit()
    }

    header: Rectangle {
        color: Theme.card
        height: 70
        border.width: 1
        border.color: Theme.border

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingLarge

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                Text {
                    text: "GLM Usage"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.text
                }
                Text {
                    text: qsTr("已连接 %1 个平台").arg(root.platformData.length)
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.muted
                }
            }

            ToolButton {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredWidth: 44
                Layout.preferredHeight: 44
                text: "\u2699"
                font.pixelSize: 24
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 24
                    color: Theme.muted
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.stackView.push("SettingsPage.qml", {
                    "stackView": root.stackView
                })
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: Theme.card
            border.width: 1
            border.color: Theme.border
            visible: root.isRefreshing

            RowLayout {
                anchors.centerIn: parent
                spacing: Theme.spacingSmall
                BusyIndicator { width: 18; height: 18; running: root.isRefreshing }
                Text {
                    text: qsTr("同步数据中...")
                    color: Theme.primary
                    font.pixelSize: Theme.fontSizeSmall
                    font.bold: true
                }
            }
        }

        Flickable {
            id: flick
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: width
            contentHeight: contentCol.implicitHeight

            readonly property real overPull: -contentY

            onMovementEnded: {
                if (pullProgress >= 1.0 && !root.isRefreshing) {
                    root.pullTriggered = true
                    root.refreshRequested()
                }
                pullProgress = 0
                pullTriggered = false
            }

            onOverPullChanged: {
                if (!root.isRefreshing && overPull > 0)
                    pullProgress = Math.min(overPull / 60, 1.5)
                else if (!dragging)
                    pullProgress = 0
            }

            ColumnLayout {
                id: contentCol
                width: flick.width
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    height: root.isRefreshing ? 40 : (pullProgress > 0 ? pullProgress * 40 : 0)
                    color: Theme.card
                    visible: height > 0
                    clip: true

                    Behavior on height { NumberAnimation { duration: 150 } }

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: Theme.spacingSmall
                        Text {
                            text: root.isRefreshing ? "\u21BB" : (pullProgress >= 1.0 ? "\u2714" : "\u21BB")
                            font.pixelSize: 16
                            color: root.isRefreshing ? Theme.primary : (pullProgress >= 1.0 ? Theme.ok : Theme.muted)

                            RotationAnimation on rotation {
                                running: root.isRefreshing
                                from: 0; to: 360
                                duration: 800
                                loops: Animation.Infinite
                            }
                        }
                        Text {
                            text: root.isRefreshing ? qsTr("刷新中...")
                                   : (pullProgress >= 1.0 ? qsTr("松开刷新") : qsTr("下拉刷新"))
                            font.pixelSize: Theme.fontSizeSmall
                            color: root.isRefreshing ? Theme.primary : Theme.muted
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: Theme.spacingLarge
                    spacing: Theme.spacingLarge

                    Item { Layout.fillWidth: true; height: 4 }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 48
                        radius: Theme.radiusMedium
                        color: Theme.badLight
                        border.width: 1
                        border.color: Theme.badMid
                        visible: root.lastError.length > 0

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            Text {
                                text: root.lastError
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.bad
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            Text {
                                text: "\u2715"
                                font.pixelSize: 14
                                color: Theme.bad
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: root.errorDismissed()
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 120
                        radius: Theme.radiusLarge
                        color: Theme.card
                        border.width: 1
                        border.color: Theme.border
                        visible: root.platformData.length === 0 && !root.isRefreshing && root.lastError.length === 0

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: Theme.spacingSmall
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("暂无平台数据")
                                font.pixelSize: Theme.fontSizeNormal
                                color: Theme.muted
                            }
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("点击右上角 \u2699 添加平台")
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.border
                            }
                        }
                    }

                    Repeater {
                        model: root.platformData

                        delegate: Rectangle {
                            id: card
                            Layout.fillWidth: true
                            radius: Theme.radiusLarge
                            border.width: 1
                            scale: 1
                            implicitHeight: cardCol.implicitHeight + 40
                            color: modelData.valid ? Theme.card : Theme.badLight
                            border.color: modelData.valid ? Theme.border : Theme.badMid

                            Behavior on scale {
                                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onPressed: card.scale = 0.97
                                onReleased: card.scale = 1
                                onCanceled: card.scale = 1
                                onClicked: root.platformClicked(index)
                            }

                            ColumnLayout {
                                id: cardCol
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: Theme.spacingLarge
                                spacing: Theme.spacingMedium

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        text: modelData.name
                                        font.pixelSize: Theme.fontSizeLarge
                                        font.bold: true
                                        color: Theme.text
                                        Layout.fillWidth: true
                                    }
                                    Rectangle {
                                        radius: 6
                                        implicitHeight: 22
                                        implicitWidth: statusLabel.implicitWidth + 16
                                        color: modelData.valid ? Theme.okLight : Theme.badMid
                                        Text {
                                            id: statusLabel
                                            anchors.centerIn: parent
                                            text: modelData.valid ? qsTr("已连接") : qsTr("未连接")
                                            font.pixelSize: Theme.fontSizeTiny
                                            font.bold: true
                                            color: modelData.valid ? Theme.ok : Theme.bad
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
                                            text: Theme.fmtPct(modelData.tokenPct)
                                            font.pixelSize: Theme.fontSizeSmall
                                            font.bold: true
                                            color: Theme.pctColor(modelData.tokenPct)
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
                                            color: Theme.pctColor(modelData.tokenPct)
                                            width: parent.width * Math.min(Math.max(modelData.tokenPct, 0), 100) / 100
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
                                            text: Theme.fmtPct(modelData.mcpPct)
                                            font.pixelSize: Theme.fontSizeSmall
                                            font.bold: true
                                            color: Theme.pctColor(modelData.mcpPct)
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
                                            color: Theme.pctColor(modelData.mcpPct)
                                            width: parent.width * Math.min(Math.max(modelData.mcpPct, 0), 100) / 100
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
                                        text: qsTr("重置") + " " + (modelData.resetTime || "--")
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
                        }
                    }

                    Item { Layout.fillWidth: true; height: 20 }
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        Rectangle {
            Layout.fillWidth: true
            height: 90
            color: Theme.card
            border.width: 1
            border.color: Theme.border

            Button {
                id: btnRefresh
                anchors.centerIn: parent
                width: parent.width - 40
                height: 50
                enabled: !root.isRefreshing

                background: Rectangle {
                    radius: Theme.radiusMedium
                    color: parent.enabled
                        ? (parent.down ? Qt.darker(Theme.primary, 1.1) : Theme.primary)
                        : Theme.primaryLight
                }

                contentItem: Item {
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: Theme.spacingSmall
                        Text {
                            id: refreshIcon
                            text: "\u21BB"
                            font.pixelSize: 20
                            font.bold: true
                            color: Theme.white

                            RotationAnimation on rotation {
                                running: root.isRefreshing
                                from: 0; to: 360
                                duration: 800
                                loops: Animation.Infinite
                            }
                        }
                        Text {
                            text: root.isRefreshing ? qsTr("查询中...") : qsTr("立即刷新")
                            font.pixelSize: Theme.fontSizeMedium
                            font.bold: true
                            color: Theme.white
                        }
                    }
                }

                onClicked: root.refreshRequested()
            }
        }
    }
}
