import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: settingsPage; title: "设置"
    property var stackRef: null
    property int refreshInterval: 5
    property int showToken: 1; property int showMcp: 1; property int showTime: 1

    Component.onCompleted: {
        refreshInterval = appSettings.autoRefreshInterval()
        showToken = appSettings.widgetShowToken(); showMcp = appSettings.widgetShowMcp(); showTime = appSettings.widgetShowTime()
    }
    function refreshPlatformList() { platformListView.model = 0; platformListView.model = appSettings.platformCount() }

    header: ToolBar {
        RowLayout { anchors.fill: parent; anchors.margins: 12
            ToolButton { font.pixelSize: 18; text: "\u25C0"
                onClicked: stackRef.pop()
                contentItem: Label { text: parent.text; font.pixelSize: 18; color: "#9CA3AF"; anchors.centerIn: parent }
            }
            Label { text: "设置"; font.pixelSize: 18; font.bold: true; color: "#E0E0E0"; Layout.fillWidth: true }
        }
    }
    Keys.onBackPressed: { stackRef.pop() }

    ScrollView { anchors.fill: parent; contentWidth: availableWidth
    ColumnLayout { anchors.fill: parent; anchors.margins: 16; spacing: 14

        // Divider + Title
        Rectangle { Layout.fillWidth: true; height: 2; color: "#3E3E3E" }
        Label { text: "\u2601 平台配置"; font.pixelSize: 20; font.bold: true; color: "#81D4FA" }

        // Add platform card
        Rectangle { Layout.fillWidth: true; radius: 12; color: "#1A1A2E"; border { width: 1; color: "#2D2D44" }
            ColumnLayout { anchors.fill: parent; anchors.margins: 16; spacing: 10
                Label { text: "选择平台"; font.pixelSize: 12; color: "#9CA3AF" }
                ComboBox { id: platformCombo; Layout.fillWidth: true; model: ["智谱 ZAI", "智谱 ZHIPU", "自定义"]; currentIndex: 0 }
                Label { text: "Auth Token"; font.pixelSize: 12; color: "#9CA3AF" }
                TextField { id: tokenField; Layout.fillWidth: true; placeholderText: "sk-..."; color: "#FFF"; echoMode: TextInput.Password
                    background: Rectangle { radius: 6; color: "#12122A"; border.color: "#444" } }
                Button { Layout.fillWidth: true; highlighted: true; text: "\u2795 添加平台"
                    onClicked: {
                        if (tokenField.text.length < 5) return
                        var n = platformCombo.currentIndex === 0 ? "ZAI" : platformCombo.currentIndex === 1 ? "ZHIPU" : "Custom"
                        appSettings.addPlatform(n, "https://open.bigmodel.cn", tokenField.text, "/api/monitor/usage")
                        tokenField.text = ""; refreshPlatformList(); usageQuery.query()
                    }
                }
            }
        }

        // Platform list
        ListView { id: platformListView; Layout.fillWidth: true
            implicitHeight: Math.min(platformListView.count * 46, 200)
            interactive: platformListView.count > 4; model: appSettings.platformCount()
            delegate: Rectangle {
                width: ListView.view.width; height: 40; radius: 6; color: "#1E1E2E"
                RowLayout { anchors.fill: parent; anchors.margins: 8
                    Label { text: appSettings.platformName(index); color: "#CCC"; Layout.fillWidth: true }
                    ToolButton { font.pixelSize: 14; text: "\u2716"
                        onClicked: { appSettings.removePlatform(index); refreshPlatformList() }
                        contentItem: Label { text: parent.text; font.pixelSize: 14; color: "#EF5350"; anchors.centerIn: parent }
                    }
                }
            }
        }

        // Widget config section
        Rectangle { Layout.fillWidth: true; height: 2; color: "#3E3E3E" }
        Label { text: "\u25A2 桌面插件配置"; font.pixelSize: 20; font.bold: true; color: "#FFD54F" }

        // Widget preview
        Rectangle { Layout.fillWidth: true; radius: 14; color: "#1A1A2E"; border { width: 1; color: "#2D2D44" }
            ColumnLayout { anchors.fill: parent; anchors.margins: 16; spacing: 10
                RowLayout { Layout.fillWidth: true
                    Label { text: "\u2666  ZAI"; font.pixelSize: 14; font.bold: true; color: "#FFF" }
                    Item { Layout.fillWidth: true }
                    Rectangle { radius: 8; color: "#2A2A3A"; implicitWidth: 56; implicitHeight: 20
                        Label { anchors.centerIn: parent; text: "Widget"; font.pixelSize: 10; color: "#9CA3AF" }
                    }
                }
                RowLayout {
                    Item { Layout.fillWidth: true; height: 50; visible: showToken === 1
                        ColumnLayout { anchors.centerIn: parent
                            Label { text: "45%"; font.pixelSize: 18; font.bold: true; color: "#10B981"; anchors.horizontalCenter: parent.horizontalCenter }
                            Label { text: "Token"; font.pixelSize: 10; color: "#9CA3AF"; anchors.horizontalCenter: parent.horizontalCenter }
                        }
                    }
                    Item { Layout.fillWidth: true; height: 50; visible: showMcp === 1
                        ColumnLayout { anchors.centerIn: parent
                            Label { text: "22%"; font.pixelSize: 18; font.bold: true; color: "#F59E0B"; anchors.horizontalCenter: parent.horizontalCenter }
                            Label { text: "MCP"; font.pixelSize: 10; color: "#9CA3AF"; anchors.horizontalCenter: parent.horizontalCenter }
                        }
                    }
                    Item { Layout.fillWidth: true; height: 50; visible: showTime === 1
                        ColumnLayout { anchors.centerIn: parent
                            Label { text: "14:30"; font.pixelSize: 18; font.bold: true; color: "#FFF"; anchors.horizontalCenter: parent.horizontalCenter }
                            Label { text: "重置"; font.pixelSize: 10; color: "#9CA3AF"; anchors.horizontalCenter: parent.horizontalCenter }
                        }
                    }
                }
            }
        }

        // Toggles
        Rectangle { Layout.fillWidth: true; radius: 12; color: "#1A1A2E"; border { width: 1; color: "#2D2D44" }
            ColumnLayout { anchors.fill: parent; anchors.margins: 4; spacing: 0

                RowLayout { height: 48; Layout.fillWidth: true; Layout.margins: 12
                    Label { text: "显示 Token 百分比"; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                    Rectangle { radius: 12; width: 44; height: 24; color: showToken === 1 ? "#2563EB" : "#555"
                        Rectangle { radius: 10; width: 18; height: 18; color: "#FFF"; x: showToken === 1 ? 23 : 3; y: 3 }
                        MouseArea { anchors.fill: parent; onClicked: showToken = (showToken === 1 ? 0 : 1) }
                    }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: "#2D2D44" }

                RowLayout { height: 48; Layout.fillWidth: true; Layout.margins: 12
                    Label { text: "显示 MCP 百分比"; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                    Rectangle { radius: 12; width: 44; height: 24; color: showMcp === 1 ? "#2563EB" : "#555"
                        Rectangle { radius: 10; width: 18; height: 18; color: "#FFF"; x: showMcp === 1 ? 23 : 3; y: 3 }
                        MouseArea { anchors.fill: parent; onClicked: showMcp = (showMcp === 1 ? 0 : 1) }
                    }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: "#2D2D44" }

                RowLayout { height: 48; Layout.fillWidth: true; Layout.margins: 12
                    Label { text: "显示重置时间"; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                    Rectangle { radius: 12; width: 44; height: 24; color: showTime === 1 ? "#2563EB" : "#555"
                        Rectangle { radius: 10; width: 18; height: 18; color: "#FFF"; x: showTime === 1 ? 23 : 3; y: 3 }
                        MouseArea { anchors.fill: parent; onClicked: showTime = (showTime === 1 ? 0 : 1) }
                    }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: "#2D2D44" }

                RowLayout { height: 44; Layout.fillWidth: true; Layout.margins: 12
                    Label { text: "刷新间隔 (分钟)"; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                    Slider { id: refreshSlider; from: 1; to: 60; value: refreshInterval }
                    Label { text: refreshSlider.value; font.pixelSize: 12; font.bold: true; color: "#9CA3AF"; width: 20 }
                }
            }
        }

        Item { Layout.fillHeight: true }

        Button { Layout.fillWidth: true; highlighted: true; text: "应用插件配置"
            onClicked: {
                appSettings.setAutoRefreshInterval(refreshSlider.value)
                appSettings.setWidgetShowToken(showToken); appSettings.setWidgetShowMcp(showMcp); appSettings.setWidgetShowTime(showTime)
                appSettings.syncWidgetConfig()
                if (refreshSlider.value > 0) usageQuery.setAutoRefresh(refreshSlider.value)
                else usageQuery.stopAutoRefresh()
                stackRef.pop()
            }
        }
    } }
}
