import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: detailPage; title: "用量详情"
    property var stackRef: null
    property var platforms: []

    Component.onCompleted: { platforms = [] }

    Connections {
        target: usageQuery
        function onQueryFinished(data) { platforms.push(data); platformsChanged() }
    }

    header: ToolBar {
        RowLayout { anchors.fill: parent; anchors.margins: 12
            ToolButton { font.pixelSize: 18; text: "\u25C0"
                onClicked: stackRef.pop()
                contentItem: Label { text: parent.text; font.pixelSize: 18; color: "#9CA3AF"; anchors.centerIn: parent }
            }
            Label { text: "用量详情"; font.pixelSize: 18; font.bold: true; color: "#E0E0E0"; Layout.fillWidth: true }
        }
    }
    Keys.onBackPressed: { stackRef.pop() }

    ScrollView { anchors.fill: parent; contentWidth: availableWidth
    ColumnLayout { anchors.fill: parent; anchors.margins: 16; spacing: 16

        Repeater {
            model: platforms.length
            delegate: Rectangle {
                Layout.fillWidth: true; radius: 14; color: "#1A1A2E"; border { width: 1; color: "#2D2D44" }
                ColumnLayout { anchors.fill: parent; anchors.margins: 16; spacing: 10

                    // Header
                    RowLayout { Layout.fillWidth: true; spacing: 8
                        Label { text: platforms[index].platformName; font.pixelSize: 18; font.bold: true; color: "#3B82F6" }
                        Item { Layout.fillWidth: true }
                        Rectangle { radius: 4; implicitWidth: 40; implicitHeight: 22
                            color: platforms[index].isValid ? "#064E3B" : "#450A0A"
                            Label { anchors.centerIn: parent; text: platforms[index].isValid ? "正常" : "异常"
                                font.pixelSize: 10; font.bold: true; color: platforms[index].isValid ? "#10B981" : "#F87171" }
                        }
                    }

                    // Summary
                    Label { text: "Token " + (platforms[index].tokenPercentage() >= 0 ? platforms[index].tokenPercentage().toFixed(1) + "%" : "--")
                        + "  \u00B7  MCP " + (platforms[index].mcpPercentage() >= 0 ? platforms[index].mcpPercentage().toFixed(1) + "%" : "--")
                        font.pixelSize: 12; color: "#9CA3AF" }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#2D2D44" }

                    // Models
                    ColumnLayout { visible: platforms[index].modelCount() > 0; spacing: 6
                        Label { text: "\u25A0 模型用量"; font.pixelSize: 12; font.bold: true; color: "#9CA3AF" }
                        Repeater { model: platforms[index].modelCount()
                            delegate: Rectangle { Layout.fillWidth: true; height: 38; radius: 8; color: "#12122A"
                                RowLayout { anchors.fill: parent; anchors.margins: 10
                                    Label { text: platforms[index].modelNames()[modelData]; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                                    Label { text: platforms[index].modelTokens(modelData) + " tokens"; font.pixelSize: 14; font.bold: true; color: "#80CBC4" }
                                    Label { text: platforms[index].modelRequests(modelData) + " reqs"; font.pixelSize: 12; color: "#7986CB" }
                                }
                            }
                        }
                    }

                    // Tools
                    ColumnLayout { visible: platforms[index].toolCount() > 0; spacing: 6
                        Label { text: "\u26A1 工具调用"; font.pixelSize: 12; font.bold: true; color: "#9CA3AF" }
                        Repeater { model: platforms[index].toolCount()
                            delegate: Rectangle { Layout.fillWidth: true; height: 38; radius: 8; color: "#12122A"
                                RowLayout { anchors.fill: parent; anchors.margins: 10
                                    Label { text: platforms[index].toolNames()[modelData]; font.pixelSize: 14; color: "#CCC"; Layout.fillWidth: true }
                                    Label { text: platforms[index].toolCalls(modelData) + " calls"; font.pixelSize: 14; font.bold: true; color: "#FFB74D" }
                                }
                            }
                        }
                    }
                }
            }
        }
    } }
}
