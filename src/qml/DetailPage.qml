import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    background: Rectangle { color: Theme.bg }

    required property var stackView
    required property var platformData

    function fmtNum(n) {
        if (n === undefined || n === null) return "0"
        if (n >= 1000000) return (n / 1000000).toFixed(1) + "M"
        if (n >= 1000) return (n / 1000).toFixed(1) + "K"
        return "" + n
    }

    function fmtComma(n) {
        if (n === undefined || n === null) return "0"
        var s = "" + n
        var neg = s.charAt(0) === '-'
        if (neg) s = s.substring(1)
        var parts = []
        while (s.length > 3) {
            parts.unshift(s.substring(s.length - 3))
            s = s.substring(0, s.length - 3)
        }
        parts.unshift(s)
        return (neg ? "-" : "") + parts.join(",")
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
        contentHeight: detailContent.implicitHeight + 40

        ColumnLayout {
            id: detailContent
            anchors.fill: parent
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

            Rectangle {
                Layout.fillWidth: true
                visible: root.platformData !== null && root.platformData !== undefined
                radius: Theme.radiusLarge
                color: Theme.card
                border.width: 1
                border.color: Theme.border
                implicitHeight: headerCol.implicitHeight + 40

                ColumnLayout {
                    id: headerCol
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: Theme.spacingLarge
                    spacing: Theme.spacingMedium

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: root.platformData ? root.platformData.name : ""
                            font.pixelSize: Theme.fontSizeTitle
                            font.bold: true
                            color: Theme.primary
                            Layout.fillWidth: true
                        }
                        Rectangle {
                            visible: root.platformData
                            color: root.platformData && root.platformData.valid ? Theme.okLight : Theme.badLight
                            radius: 6
                            implicitWidth: 44
                            implicitHeight: 22
                            Text {
                                anchors.centerIn: parent
                                text: root.platformData && root.platformData.valid ? qsTr("正常") : qsTr("异常")
                                font.pixelSize: Theme.fontSizeTiny
                                font.bold: true
                                color: root.platformData && root.platformData.valid ? Theme.ok : Theme.bad
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingMedium
                        Text {
                            text: "Token " + (root.platformData ? Theme.fmtPct(root.platformData.tokenPct) : "--")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.muted
                        }
                        Text {
                            text: "MCP " + (root.platformData ? Theme.fmtPct(root.platformData.mcpPct) : "--")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.muted
                        }
                        Text {
                            visible: root.platformData && root.platformData.resetTime
                            text: qsTr("重置") + " " + (root.platformData ? root.platformData.resetTime : "--")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.muted
                        }
                    }
                }
            }

            Rectangle {
                visible: root.platformData !== null && root.platformData !== undefined
                Layout.fillWidth: true
                radius: Theme.radiusLarge
                color: Theme.card
                border.width: 1
                border.color: Theme.border
                implicitHeight: statsRow.implicitHeight + 32

                RowLayout {
                    id: statsRow
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMedium
                    spacing: 0

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.platformData ? fmtNum(root.platformData.totalTokens) : "0"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.primary
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("总 Token")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                    }
                    Rectangle { width: 1; height: 36; color: Theme.border }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.platformData ? fmtNum(root.platformData.totalReqs) : "0"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.ok
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("总请求")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                    }
                    Rectangle { width: 1; height: 36; color: Theme.border }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.platformData ? fmtNum(root.platformData.totalToolCalls) : "0"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.warn
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("工具调用")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                    }
                    Rectangle {
                        visible: root.platformData && root.platformData.models
                                 && root.platformData.models.length > 0
                        width: 1; height: 36; color: Theme.border
                    }
                    ColumnLayout {
                        visible: root.platformData && root.platformData.models
                                 && root.platformData.models.length > 0
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.platformData && root.platformData.models
                                  ? "" + root.platformData.models.length : "0"
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.text
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: qsTr("模型数")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                    }
                }
            }

            ColumnLayout {
                visible: root.platformData && root.platformData.quotas
                         && root.platformData.quotas.length > 0
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                Text {
                    text: qsTr("配额详情")
                    font.pixelSize: Theme.fontSizeNormal
                    font.bold: true
                    color: Theme.text
                }

                Repeater {
                    model: root.platformData && root.platformData.quotas
                           ? root.platformData.quotas.length : 0
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: quotaCol.implicitHeight + 24
                        radius: Theme.radiusSmall
                        color: Theme.itemBg

                        ColumnLayout {
                            id: quotaCol
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingMedium
                            anchors.rightMargin: Theme.spacingMedium
                            anchors.topMargin: 12
                            anchors.bottomMargin: 12
                            spacing: Theme.spacingTiny

                            property var q: root.platformData.quotas[modelData]
                            property bool hasDetail: q && (q.currentUsage > 0 || q.total > 0 || q.remaining > 0)

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: quotaCol.q ? quotaCol.q.type || qsTr("配额") : qsTr("配额")
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.bold: true
                                    color: Theme.text
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: quotaCol.q ? Theme.fmtPct(quotaCol.q.percentage) : "--%"
                                    font.pixelSize: Theme.fontSizeSmall
                                    font.bold: true
                                    color: quotaCol.q ? Theme.pctColor(quotaCol.q.percentage) : Theme.muted
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Theme.spacingLarge
                                visible: quotaCol.hasDetail

                                Text {
                                    text: qsTr("已用") + ": " + fmtComma(quotaCol.q.currentUsage)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                                Text {
                                    text: qsTr("总量") + ": " + fmtComma(quotaCol.q.total)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                                Text {
                                    text: qsTr("剩余") + ": " + fmtComma(quotaCol.q.remaining)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.ok
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Theme.spacingLarge
                                visible: !quotaCol.hasDetail && quotaCol.q
                                         && (quotaCol.q.unit > 0 || quotaCol.q.number > 0)

                                Text {
                                    text: qsTr("额度") + ": " + quotaCol.q.unit + " / " + quotaCol.q.number
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 4
                                radius: 2
                                color: Theme.border

                                Rectangle {
                                    height: 4
                                    radius: 2
                                    width: parent.width * Math.min(
                                        Math.max(quotaCol.q.percentage, 0) / 100, 1.0)
                                    color: Theme.pctColor(quotaCol.q.percentage)
                                }
                            }

                            Text {
                                visible: quotaCol.q && quotaCol.q.resetTime
                                text: qsTr("重置时间") + ": " + quotaCol.q.resetTime
                                font.pixelSize: Theme.fontSizeTiny
                                color: Theme.muted
                            }

                            Text {
                                visible: quotaCol.q && quotaCol.q.usageDetails
                                text: quotaCol.q.usageDetails
                                font.pixelSize: Theme.fontSizeTiny
                                color: Theme.muted
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                visible: root.platformData && root.platformData.models
                         && root.platformData.models.length > 0
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                Text {
                    text: qsTr("模型用量详情")
                    font.pixelSize: Theme.fontSizeNormal
                    font.bold: true
                    color: Theme.text
                }

                Repeater {
                    model: root.platformData && root.platformData.models
                           ? root.platformData.models.length : 0
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: modelDetailCol.implicitHeight + 24
                        radius: Theme.radiusSmall
                        color: Theme.itemBg

                        ColumnLayout {
                            id: modelDetailCol
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingMedium
                            anchors.rightMargin: Theme.spacingMedium
                            anchors.topMargin: 12
                            anchors.bottomMargin: 12
                            spacing: Theme.spacingTiny

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: root.platformData.models[modelData]
                                    font.pixelSize: Theme.fontSizeNormal
                                    font.bold: true
                                    color: Theme.text
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    wrapMode: Text.NoWrap
                                }
                                Text {
                                    text: root.platformData.modelProvider
                                          ? root.platformData.modelProvider[modelData] : ""
                                    visible: text.length > 0
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Theme.spacingLarge
                                Text {
                                    text: qsTr("输入") + ": " + fmtComma(
                                        root.platformData.modelInputTokens
                                        ? root.platformData.modelInputTokens[modelData] : 0)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                                Text {
                                    text: qsTr("输出") + ": " + fmtComma(
                                        root.platformData.modelOutputTokens
                                        ? root.platformData.modelOutputTokens[modelData] : 0)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                                Text {
                                    text: qsTr("合计") + ": " + fmtComma(
                                        root.platformData.modelTokens[modelData] || 0)
                                    font.pixelSize: Theme.fontSizeTiny
                                    font.bold: true
                                    color: Theme.primary
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: qsTr("请求次数") + ": " + fmtComma(
                                        root.platformData.modelReq[modelData] || 0)
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.muted
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 4
                                radius: 2
                                color: Theme.border
                                visible: root.platformData.totalTokens > 0

                                property real ratio: root.platformData.modelTokens[modelData]
                                                      / root.platformData.totalTokens

                                Rectangle {
                                    height: 4
                                    radius: 2
                                    width: parent.width * Math.min(parent.ratio, 1.0)
                                    color: Theme.primary
                                }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                visible: root.platformData && root.platformData.toolNames
                         && root.platformData.toolNames.length > 0
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                Text {
                    text: qsTr("工具调用详情")
                    font.pixelSize: Theme.fontSizeNormal
                    font.bold: true
                    color: Theme.text
                }

                Repeater {
                    model: root.platformData && root.platformData.toolNames
                           ? root.platformData.toolNames.length : 0
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        height: 48
                        radius: Theme.radiusSmall
                        color: Theme.itemBg

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Theme.spacingMedium
                            Text {
                                text: root.platformData.toolNames[modelData]
                                font.pixelSize: Theme.fontSizeNormal
                                font.bold: true
                                color: Theme.text
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                wrapMode: Text.NoWrap
                            }
                            Text {
                                text: fmtComma(root.platformData.toolCalls[modelData] || 0) + " " + qsTr("次调用")
                                font.pixelSize: Theme.fontSizeNormal
                                font.bold: true
                                color: Theme.warn
                            }
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true; height: 20 }
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
