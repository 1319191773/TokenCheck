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
    property bool refreshSuccess: false
    property bool refreshFromButton: false

    onIsRefreshingChanged: {
        if (!isRefreshing && refreshFromButton) {
            refreshSuccess = true
            refreshFromButton = false
            successTimer.start()
        }
    }

    Timer {
        id: successTimer
        interval: 1200
        onTriggered: root.refreshSuccess = false
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
                    text: "TokenCheck"
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
                    height: root.isRefreshing ? 44 : (pullProgress > 0 ? pullProgress * 44 : 0)
                    color: Theme.card
                    visible: height > 0
                    clip: true

                    Behavior on height {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutCubic
                        }
                    }

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: Theme.spacingSmall

                        Text {
                            id: pullArrow
                            text: root.isRefreshing ? "\u21BB" : "\u25BC"
                            font.pixelSize: 14
                            color: root.isRefreshing ? Theme.primary : (pullProgress >= 1.0 ? Theme.ok : Theme.muted)
                            scale: pullProgress > 0 && !root.isRefreshing
                                   ? 0.6 + Math.min(pullProgress, 1.0) * 0.4 : 1.0

                            Behavior on scale {
                                NumberAnimation { duration: 100 }
                            }

                            rotation: {
                                if (root.isRefreshing) return 0
                                if (pullProgress >= 1.0) return 180
                                return 0
                            }

                            Behavior on rotation {
                                NumberAnimation { duration: 200; easing.type: Easing.OutQuad }
                            }

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
                            color: root.isRefreshing ? Theme.primary : (pullProgress >= 1.0 ? Theme.ok : Theme.muted)
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
                            implicitHeight: (cardLoader.item ? cardLoader.item.implicitHeight : 80) + 32
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

                            Loader {
                                id: cardLoader
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: Theme.spacingLarge
                                source: resolveCardSource(modelData.platformType)
                                onLoaded: item.cardData = modelData
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
            height: 80
            color: Theme.card
            border.width: 1
            border.color: Theme.border

            Button {
                id: btnRefresh
                anchors.centerIn: parent
                width: parent.width - 40
                height: 48

                property bool pressAnim: false

                background: Rectangle {
                    radius: Theme.radiusMedium
                    color: root.refreshSuccess ? Theme.ok
                           : (root.isRefreshing ? Theme.primaryLight
                              : (btnRefresh.down ? Qt.darker(Theme.primary, 1.1) : Theme.primary))

                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }

                    scale: btnRefresh.pressAnim ? 0.96 : 1.0

                    Behavior on scale {
                        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                    }
                }

                contentItem: Item {
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: Theme.spacingSmall

                        Text {
                            id: refreshIcon
                            text: root.isRefreshing ? "\u21BB" : "\u21BB"
                            font.pixelSize: 20
                            font.bold: true
                            color: Theme.white
                            rotation: root.isRefreshing ? refreshIcon.rotation : 0

                            RotationAnimation on rotation {
                                running: root.isRefreshing
                                from: 0; to: 360
                                duration: 800
                                loops: Animation.Infinite
                            }

                            Behavior on text {
                                enabled: false
                            }
                        }

                        Text {
                            id: refreshLabel
                            text: root.refreshSuccess ? qsTr("刷新成功")
                                   : (root.isRefreshing ? qsTr("查询中...") : qsTr("立即刷新"))
                            font.pixelSize: Theme.fontSizeMedium
                            font.bold: true
                            color: Theme.white
                        }
                    }
                }

                onPressed: pressAnim = true
                onReleased: pressAnim = false
                onCanceled: pressAnim = false
                onClicked: {
                    refreshFromButton = true
                    root.refreshRequested()
                }
            }
        }
    }

    function resolveCardSource(type) {
        switch (type) {
        case "glm": return "../platforms/glm/qml/GlmCard.qml"
        case "deepseek": return "../platforms/deepseek/qml/DeepSeekCard.qml"
        default: return "../platforms/glm/qml/GlmCard.qml"
        }
    }
}
