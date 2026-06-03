import QtQuick
import QtQuick.Layouts
import "../../../qml"

ColumnLayout {
    id: rootContent
    property var platformData
    Layout.fillWidth: true
    spacing: Theme.spacingLarge

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

    Rectangle {
        Layout.fillWidth: true
        visible: rootContent.platformData !== null && rootContent.platformData !== undefined
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
                    text: rootContent.platformData ? rootContent.platformData.name : ""
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primary
                    Layout.fillWidth: true
                }
                Rectangle {
                    visible: rootContent.platformData
                    color: rootContent.platformData && rootContent.platformData.valid
                           ? Theme.okLight : Theme.badLight
                    radius: 6
                    implicitWidth: 44
                    implicitHeight: 22
                    Text {
                        anchors.centerIn: parent
                        text: rootContent.platformData && rootContent.platformData.valid
                              ? qsTr("正常") : qsTr("异常")
                        font.pixelSize: Theme.fontSizeTiny
                        font.bold: true
                        color: rootContent.platformData && rootContent.platformData.valid
                               ? Theme.ok : Theme.bad
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingMedium
                Text {
                    text: "Token " + (rootContent.platformData
                           ? Theme.fmtPct(rootContent.platformData.tokenPct) : "--")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.muted
                }
                Text {
                    text: "MCP " + (rootContent.platformData
                           ? Theme.fmtPct(rootContent.platformData.mcpPct) : "--")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.muted
                }
                Text {
                    visible: rootContent.platformData && rootContent.platformData.resetTime
                    text: qsTr("重置") + " " + (rootContent.platformData
                           ? rootContent.platformData.resetTime : "--")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.muted
                }
            }
        }
    }

    Rectangle {
        visible: rootContent.platformData !== null && rootContent.platformData !== undefined
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
                    text: rootContent.platformData ? fmtNum(rootContent.platformData.totalTokens) : "0"
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
                    text: rootContent.platformData ? fmtNum(rootContent.platformData.totalReqs) : "0"
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
                    text: rootContent.platformData ? fmtNum(rootContent.platformData.totalToolCalls) : "0"
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
                visible: rootContent.platformData && rootContent.platformData.models
                         && rootContent.platformData.models.length > 0
                width: 1; height: 36; color: Theme.border
            }
            ColumnLayout {
                visible: rootContent.platformData && rootContent.platformData.models
                         && rootContent.platformData.models.length > 0
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: rootContent.platformData && rootContent.platformData.models
                          ? "" + rootContent.platformData.models.length : "0"
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
        visible: rootContent.platformData && rootContent.platformData.quotas
                 && rootContent.platformData.quotas.length > 0
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        Text {
            text: qsTr("配额详情")
            font.pixelSize: Theme.fontSizeNormal
            font.bold: true
            color: Theme.text
        }

        Repeater {
            model: rootContent.platformData && rootContent.platformData.quotas
                   ? rootContent.platformData.quotas.length : 0
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

                    property var q: rootContent.platformData.quotas[modelData]
                    property bool hasDetail: q && (q.currentUsage > 0 || q.total > 0 || q.remaining > 0)

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: quotaCol.q ? fmtQuotaType(quotaCol.q.type) : qsTr("配额")
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
                        visible: quotaCol.q && quotaCol.q.percentage >= 0

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
        visible: rootContent.platformData && rootContent.platformData.models
                 && rootContent.platformData.models.length > 0
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        Text {
            text: qsTr("模型用量详情")
            font.pixelSize: Theme.fontSizeNormal
            font.bold: true
            color: Theme.text
        }

        Repeater {
            model: rootContent.platformData && rootContent.platformData.models
                   ? rootContent.platformData.models.length : 0
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
                            text: rootContent.platformData.models[modelData]
                            font.pixelSize: Theme.fontSizeNormal
                            font.bold: true
                            color: Theme.text
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            wrapMode: Text.NoWrap
                        }
                        Text {
                            text: rootContent.platformData.modelProvider
                                  ? rootContent.platformData.modelProvider[modelData] : ""
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
                                rootContent.platformData.modelInputTokens
                                ? rootContent.platformData.modelInputTokens[modelData] : 0)
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                        Text {
                            text: qsTr("输出") + ": " + fmtComma(
                                rootContent.platformData.modelOutputTokens
                                ? rootContent.platformData.modelOutputTokens[modelData] : 0)
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                        Text {
                            text: qsTr("合计") + ": " + fmtComma(
                                rootContent.platformData.modelTokens[modelData] || 0)
                            font.pixelSize: Theme.fontSizeTiny
                            font.bold: true
                            color: Theme.primary
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: qsTr("请求次数") + ": " + fmtComma(
                                rootContent.platformData.modelReq[modelData] || 0)
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.muted
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 4
                        radius: 2
                        color: Theme.border
                        visible: rootContent.platformData.totalTokens > 0

                        property real ratio: rootContent.platformData.modelTokens[modelData]
                                              / rootContent.platformData.totalTokens

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
        visible: rootContent.platformData && rootContent.platformData.toolNames
                 && rootContent.platformData.toolNames.length > 0
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        Text {
            text: qsTr("工具调用详情")
            font.pixelSize: Theme.fontSizeNormal
            font.bold: true
            color: Theme.text
        }

        Repeater {
            model: rootContent.platformData && rootContent.platformData.toolNames
                   ? rootContent.platformData.toolNames.length : 0
            delegate: Rectangle {
                Layout.fillWidth: true
                height: 48
                radius: Theme.radiusSmall
                color: Theme.itemBg

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingMedium
                    Text {
                        text: rootContent.platformData.toolNames[modelData]
                        font.pixelSize: Theme.fontSizeNormal
                        font.bold: true
                        color: Theme.text
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                    }
                    Text {
                        text: fmtComma(rootContent.platformData.toolCalls[modelData] || 0)
                              + " " + qsTr("次调用")
                        font.pixelSize: Theme.fontSizeNormal
                        font.bold: true
                        color: Theme.warn
                    }
                }
            }
        }
    }

    function fmtQuotaType(type) {
        var t = (type || "").toLowerCase()
        if (t.indexOf("token") >= 0) return qsTr("Token 配额")
        if (t.indexOf("time") >= 0) return qsTr("MCP 工具配额")
        return type || qsTr("配额")
    }
}
