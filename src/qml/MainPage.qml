import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: app
    width: 400
    height: 800
    title: "GLM Usage"
    visible: true
    color: cBg

    // === 现代扁平风全局色板 ===
    readonly property color cBg: "#F3F4F6"
    readonly property color cCard: "#FFFFFF"
    readonly property color cItemBg: "#F9FAFB"
    readonly property color cPri: "#2563EB"
    readonly property color cOk: "#10B981"
    readonly property color cWarn: "#F59E0B"
    readonly property color cBad: "#F43F5E"
    readonly property color cTxt: "#111827"
    readonly property color cMuted: "#6B7280"
    readonly property color cBd: "#E5E7EB"

    property var platformData: []
    property bool isRefreshing: false

    function fmtPct(p) {
        return p < 0 ? "--%" : p.toFixed(1) + "%"
    }
    function pc(p) {
        if (p >= 80)
            return cBad
        if (p >= 50)
            return cWarn
        return cOk
    }

    Component.onCompleted: {
        platformData = []
    }

    Connections {
        target: usageQuery
        function onQueryFinished(data) {
            var f = false
            for (var i = 0; i < platformData.length; i++) {
                if (platformData[i].name === data.platformName) {
                    platformData[i] = {
                        "name": data.platformName,
                        "tokenPct": data.tokenPercentage(),
                        "mcpPct": data.mcpPercentage(),
                        "resetTime": data.tokenResetTime(),
                        "valid": data.isValid,
                        "error": data.errorMsg,
                        "models": data.modelNames(),
                        "modelTokens": [],
                        "toolNames": [],
                        "modelReq": [],
                        "toolCalls": []
                    }
                    for (var j = 0; j < data.modelCount(); j++) {
                        platformData[i].modelTokens.push(data.modelTokens(j))
                        platformData[i].modelReq.push(data.modelRequests(j))
                    }
                    for (var k = 0; k < data.toolCount(); k++) {
                        platformData[i].toolNames.push(data.toolNames()[k])
                        platformData[i].toolCalls.push(data.toolCalls(k))
                    }
                    f = true
                    break
                }
            }
            if (!f) {
                var d = {
                    "name": data.platformName,
                    "tokenPct": data.tokenPercentage(),
                    "mcpPct": data.mcpPercentage(),
                    "resetTime": data.tokenResetTime(),
                    "valid": data.isValid,
                    "error": data.errorMsg,
                    "models": data.modelNames(),
                    "modelTokens": [],
                    "toolNames": [],
                    "modelReq": [],
                    "toolCalls": []
                }
                for (var m = 0; m < data.modelCount(); m++) {
                    d.modelTokens.push(data.modelTokens(m))
                    d.modelReq.push(data.modelRequests(m))
                }
                for (var n = 0; n < data.toolCount(); n++) {
                    d.toolNames.push(data.toolNames()[n])
                    d.toolCalls.push(data.toolCalls(n))
                }
                platformData.push(d)
            }
            platformDataChanged()
        }
        function onQueryAllFinished() {
            isRefreshing = false
        }
        function onQueryFailed(e) {
            isRefreshing = false
        }
    }

    // === 导航栈 (StackView) ===
    StackView {
        id: sv
        anchors.fill: parent
        initialItem: mainPageComp

        pushEnter: Transition {
            NumberAnimation {
                property: "x"
                from: sv.width * 0.2
                to: 0
                duration: 250
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 200
            }
        }
        popEnter: Transition {
            NumberAnimation {
                property: "x"
                from: -sv.width * 0.2
                to: 0
                duration: 250
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 200
            }
        }
        popExit: Transition {
            NumberAnimation {
                property: "x"
                from: 0
                to: sv.width * 0.2
                duration: 250
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 200
            }
        }
    }

    // ==========================================
    // 1. 主页组件 (Main View)
    // ==========================================
    Component {
        id: mainPageComp
        Page {
            background: Rectangle {
                color: cBg
            }
            Keys.onBackPressed: {
                if (sv.depth > 1)
                    sv.pop()
                else
                    Qt.quit()
            }

            header: Rectangle {
                color: cCard
                height: 70
                border.width: 1
                border.color: cBd
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    ColumnLayout {
                        spacing: 2
                        Layout.fillWidth: true
                        Text {
                            text: "GLM Usage"
                            font.pixelSize: 20
                            font.bold: true
                            color: cTxt
                        }
                        Text {
                            text: "已连接 " + platformData.length + " 个平台"
                            font.pixelSize: 12
                            color: cMuted
                        }
                    }
                    ToolButton {
                        text: "⚙"
                        font.pixelSize: 24
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 24
                            color: cMuted
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: sv.push(settingsPageComp)
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: parent.width
                        spacing: 16
                        anchors.margins: 20
                        Item {
                            Layout.fillWidth: true
                            height: 4
                        } // Top padding

                        // 刷新进度条
                        Rectangle {
                            Layout.fillWidth: true
                            height: 60
                            radius: 12
                            color: cCard
                            border.width: 1
                            border.color: cBd
                            visible: isRefreshing
                            RowLayout {
                                anchors.centerIn: parent
                                spacing: 12
                                BusyIndicator {
                                    width: 20
                                    height: 20
                                    running: isRefreshing
                                }
                                Text {
                                    text: "同步数据中..."
                                    color: cPri
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                        }

                        // 平台卡片
                        Repeater {
                            model: platformData
                            delegate: Rectangle {
                                id: card
                                Layout.fillWidth: true
                                radius: 16
                                border.width: 1
                                scale: 1
                                implicitHeight: cardCol.implicitHeight + 40 // 动态高度防坍塌
                                color: modelData.valid ? cCard : "#FFF1F2"
                                border.color: modelData.valid ? cBd : "#FFE4E6"
                                Behavior on scale {
                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.OutQuad
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: card.scale = 0.97
                                    onReleased: card.scale = 1
                                    onCanceled: card.scale = 1
                                    onClicked: sv.push(detailPageComp, {
                                                           "pIndex": index
                                                       })
                                }

                                ColumnLayout {
                                    id: cardCol
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.margins: 20
                                    spacing: 14

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Text {
                                            text: modelData.name
                                            font.pixelSize: 18
                                            font.bold: true
                                            color: cTxt
                                            Layout.fillWidth: true
                                        }
                                        Rectangle {
                                            visible: !modelData.valid
                                            radius: 6
                                            color: "#FEE2E2"
                                            implicitWidth: 56
                                            implicitHeight: 22
                                            Text {
                                                anchors.centerIn: parent
                                                text: "无效 Token"
                                                font.pixelSize: 10
                                                font.bold: true
                                                color: "#DC2626"
                                            }
                                        }
                                        Text {
                                            text: modelData.valid ? "✔" : "✘"
                                            font.pixelSize: 20
                                            color: modelData.valid ? cOk : cBad
                                        }
                                    }

                                    // Token 进度条
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text {
                                                text: "Token"
                                                font.pixelSize: 12
                                                color: cMuted
                                                Layout.fillWidth: true
                                            }
                                            Text {
                                                text: fmtPct(modelData.tokenPct)
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: pc(modelData.tokenPct)
                                            }
                                        }
                                        Rectangle {
                                            Layout.fillWidth: true
                                            height: 6
                                            radius: 3
                                            color: cBd
                                            Rectangle {
                                                height: 6
                                                radius: 3
                                                color: pc(modelData.tokenPct)
                                                width: parent.width * Math.min(
                                                           Math.max(
                                                               modelData.tokenPct,
                                                               0), 100) / 100
                                            }
                                        }
                                    }

                                    // MCP 进度条
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text {
                                                text: "MCP"
                                                font.pixelSize: 12
                                                color: cMuted
                                                Layout.fillWidth: true
                                            }
                                            Text {
                                                text: fmtPct(modelData.mcpPct)
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: pc(modelData.mcpPct)
                                            }
                                        }
                                        Rectangle {
                                            Layout.fillWidth: true
                                            height: 6
                                            radius: 3
                                            color: cBd
                                            Rectangle {
                                                height: 6
                                                radius: 3
                                                color: pc(modelData.mcpPct)
                                                width: parent.width * Math.min(
                                                           Math.max(
                                                               modelData.mcpPct,
                                                               0), 100) / 100
                                            }
                                        }
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 1
                                        color: cBd
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Text {
                                            text: "⏱ " + (modelData.resetTime
                                                          || "--") + " 重置"
                                            font.pixelSize: 12
                                            color: cMuted
                                            Layout.fillWidth: true
                                        }
                                        Text {
                                            text: "→"
                                            font.pixelSize: 16
                                            color: "#D1D5DB"
                                        }
                                    }
                                }
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                            height: 20
                        }
                    }
                }

                // 底部刷新按钮
                Rectangle {
                    Layout.fillWidth: true
                    height: 90
                    color: cCard
                    border.width: 1
                    border.color: cBd
                    Button {
                        id: btnRefresh
                        anchors.centerIn: parent
                        width: parent.width - 40
                        height: 50
                        enabled: !isRefreshing
                        background: Rectangle {
                            radius: 12
                            color: parent.enabled ? (parent.down ? Qt.darker(
                                                                       cPri,
                                                                       1.1) : cPri) : "#93C5FD"
                        }
                        contentItem: RowLayout {
                            anchors.centerIn: parent
                            spacing: 8
                            Text {
                                text: "↻"
                                font.pixelSize: 20
                                font.bold: true
                                color: "white"
                            }
                            Text {
                                text: isRefreshing ? "查询中..." : "立即刷新"
                                font.pixelSize: 16
                                font.bold: true
                                color: "white"
                            }
                        }
                        onClicked: {
                            platformData = []
                            isRefreshing = true
                            usageQuery.query()
                        }
                    }
                }
            }
        }
    }

    // ==========================================
    // 2. 设置视图组件 (Settings View)
    // ==========================================
    Component {
        id: settingsPageComp
        Page {
            background: Rectangle {
                color: cBg
            }
            property int tab: 0
            property int interval: 5
            property int sT: 1
            property int sM: 1
            property int sTm: 1
            property int platformCountCache: 0 // 状态驱动更新，避免函数调用非响应式

            Component.onCompleted: {
                interval = appSettings.autoRefreshInterval()
                sT = appSettings.widgetShowToken()
                sM = appSettings.widgetShowMcp()
                sTm = appSettings.widgetShowTime()
                platformCountCache = appSettings.platformCount()
            }
            function refreshPlatformList() {
                platformCountCache = appSettings.platformCount()
            }

            Keys.onBackPressed: {
                sv.pop()
            }

            header: Rectangle {
                color: cCard
                height: 70
                border.width: 1
                border.color: cBd
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10
                    ToolButton {
                        text: "←"
                        font.pixelSize: 24
                        font.bold: true
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 24
                            color: cMuted
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: sv.pop()
                    }
                    Text {
                        text: "设置"
                        font.pixelSize: 18
                        font.bold: true
                        color: cTxt
                        Layout.fillWidth: true
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // iOS 风格 Tab 切换器 (固定在顶部)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.margins: 20
                    Layout.topMargin: 20
                    height: 44
                    radius: 10
                    color: "#E5E7EB"
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: 4
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 8
                            color: tab === 0 ? cCard : "transparent"
                            Text {
                                anchors.centerIn: parent
                                text: "🔑 平台管理"
                                font.pixelSize: 14
                                font.bold: tab === 0
                                color: tab === 0 ? cTxt : cMuted
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: tab = 0
                            }
                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                }
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 8
                            color: tab === 1 ? cCard : "transparent"
                            Text {
                                anchors.centerIn: parent
                                text: "📦 桌面组件"
                                font.pixelSize: 14
                                font.bold: tab === 1
                                color: tab === 1 ? cTxt : cMuted
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: tab = 1
                            }
                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                }
                            }
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: parent.width
                        spacing: 20
                        anchors.margins: 20

                        // --- Tab 1: 平台管理 ---
                        ColumnLayout {
                            visible: tab === 0
                            Layout.fillWidth: true
                            spacing: 16

                            // 添加平台表单
                            Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: formCol.implicitHeight + 32
                                radius: 16
                                color: cCard
                                border.width: 1
                                border.color: cBd
                                ColumnLayout {
                                    id: formCol
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.margins: 16
                                    spacing: 12

                                    Text {
                                        text: "选择平台"
                                        font.pixelSize: 12
                                        color: cMuted
                                    }
                                    ComboBox {
                                        id: pCombo
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 44
                                        model: ["智谱 ZAI", "智谱 ZHIPU", "自定义"]
                                        background: Rectangle {
                                            color: cItemBg
                                            border.width: 1
                                            border.color: cBd
                                            radius: 8
                                        }
                                    }
                                    Text {
                                        text: "Auth Token"
                                        font.pixelSize: 12
                                        color: cMuted
                                    }
                                    TextField {
                                        id: tField
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 44
                                        placeholderText: "sk-..."
                                        echoMode: TextInput.Password
                                        color: cTxt
                                        background: Rectangle {
                                            color: cItemBg
                                            border.width: 1
                                            border.color: cBd
                                            radius: 8
                                        }
                                    }
                                    Item {
                                        Layout.fillWidth: true
                                        height: 4
                                    } // 小间距
                                    Button {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 44
                                        background: Rectangle {
                                            color: cTxt
                                            radius: 8
                                        }
                                        contentItem: Text {
                                            text: "＋ 添加平台"
                                            color: "white"
                                            font.pixelSize: 14
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: {
                                            if (tField.text.length < 5)
                                                return
                                            var n = pCombo.currentIndex
                                                    === 0 ? "ZAI" : pCombo.currentIndex
                                                            === 1 ? "ZHIPU" : "Custom"
                                            appSettings.addPlatform(
                                                        n,
                                                        "https://open.bigmodel.cn",
                                                        tField.text,
                                                        "/api/monitor/usage")
                                            tField.text = ""
                                            refreshPlatformList()
                                            usageQuery.query()
                                        }
                                    }
                                }
                            }

                            Text {
                                text: "已添加 (" + platformCountCache + ")"
                                font.pixelSize: 12
                                font.bold: true
                                color: cMuted
                            }

                            // 用 Repeater 替换 ListView 解决嵌套滚动导致的布局混乱
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                Repeater {
                                    model: platformCountCache
                                    delegate: Rectangle {
                                        Layout.fillWidth: true
                                        height: 52
                                        radius: 12
                                        color: cCard
                                        border.width: 1
                                        border.color: cBd
                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 14
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 2
                                                Text {
                                                    text: appSettings.platformName(
                                                              index)
                                                    font.pixelSize: 14
                                                    font.bold: true
                                                    color: cTxt
                                                }
                                                Text {
                                                    text: appSettings.platformAuthToken(
                                                              index).substring(
                                                              0, 6) + "..."
                                                    font.pixelSize: 11
                                                    color: cMuted
                                                }
                                            }
                                            ToolButton {
                                                text: "🗑"
                                                font.pixelSize: 16
                                                contentItem: Text {
                                                    text: parent.text
                                                    font.pixelSize: 16
                                                    color: cBad
                                                    verticalAlignment: Text.AlignVCenter
                                                }
                                                onClicked: {
                                                    appSettings.removePlatform(
                                                                index)
                                                    refreshPlatformList()
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            Item {
                                Layout.fillWidth: true
                                height: 20
                            }
                        }

                        // --- Tab 2: 桌面组件 ---
                        ColumnLayout {
                            visible: tab === 1
                            Layout.fillWidth: true
                            spacing: 16

                            // 预览卡片
                            Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: previewCol.implicitHeight + 32
                                radius: 16
                                color: cCard
                                border.width: 1
                                border.color: cBd
                                ColumnLayout {
                                    id: previewCol
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.margins: 16
                                    spacing: 12
                                    RowLayout {
                                        Layout.fillWidth: true
                                        Rectangle {
                                            width: 24
                                            height: 24
                                            radius: 6
                                            color: cPri
                                            Text {
                                                anchors.centerIn: parent
                                                text: "⚡"
                                                color: "white"
                                            }
                                        }
                                        Text {
                                            text: "ZAI"
                                            font.pixelSize: 14
                                            font.bold: true
                                            Layout.fillWidth: true
                                            color: cTxt
                                        }
                                        Rectangle {
                                            color: cItemBg
                                            radius: 10
                                            width: 60
                                            height: 20
                                            Text {
                                                anchors.centerIn: parent
                                                text: "刚刚更新"
                                                font.pixelSize: 10
                                                color: cMuted
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
                                                font.pixelSize: 18
                                                font.bold: true
                                                color: cOk
                                            }
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "TOKEN"
                                                font.pixelSize: 10
                                                color: cMuted
                                            }
                                        }
                                        Rectangle {
                                            visible: sT === 1 && sM === 1
                                            width: 1
                                            height: 30
                                            color: cBd
                                        }
                                        ColumnLayout {
                                            visible: sM === 1
                                            Layout.fillWidth: true
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "22%"
                                                font.pixelSize: 18
                                                font.bold: true
                                                color: cWarn
                                            }
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "MCP"
                                                font.pixelSize: 10
                                                color: cMuted
                                            }
                                        }
                                        Rectangle {
                                            visible: sM === 1 && sTm === 1
                                            width: 1
                                            height: 30
                                            color: cBd
                                        }
                                        ColumnLayout {
                                            visible: sTm === 1
                                            Layout.fillWidth: true
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "14:30"
                                                font.pixelSize: 18
                                                font.bold: true
                                                color: cTxt
                                            }
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: "重置"
                                                font.pixelSize: 10
                                                color: cMuted
                                            }
                                        }
                                    }
                                }
                            }

                            // 开关设置区
                            Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: switchesCol.implicitHeight
                                radius: 16
                                color: cCard
                                border.width: 1
                                border.color: cBd
                                ColumnLayout {
                                    id: switchesCol
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    spacing: 0

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 60
                                        anchors.margins: 16
                                        Text {
                                            text: "显示 Token 百分比"
                                            font.pixelSize: 14
                                            color: cTxt
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 16
                                        }
                                        Rectangle {
                                            radius: 12
                                            width: 44
                                            height: 24
                                            color: sT === 1 ? cPri : cBd
                                            Layout.rightMargin: 16
                                            Rectangle {
                                                radius: 10
                                                width: 20
                                                height: 20
                                                color: "white"
                                                y: 2
                                                x: sT === 1 ? 22 : 2
                                                Behavior on x {
                                                    NumberAnimation {
                                                        duration: 150
                                                        easing.type: Easing.OutQuad
                                                    }
                                                }
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: sT = (sT === 1 ? 0 : 1)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 1
                                        color: cBd
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 60
                                        anchors.margins: 16
                                        Text {
                                            text: "显示 MCP 百分比"
                                            font.pixelSize: 14
                                            color: cTxt
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 16
                                        }
                                        Rectangle {
                                            radius: 12
                                            width: 44
                                            height: 24
                                            color: sM === 1 ? cPri : cBd
                                            Layout.rightMargin: 16
                                            Rectangle {
                                                radius: 10
                                                width: 20
                                                height: 20
                                                color: "white"
                                                y: 2
                                                x: sM === 1 ? 22 : 2
                                                Behavior on x {
                                                    NumberAnimation {
                                                        duration: 150
                                                        easing.type: Easing.OutQuad
                                                    }
                                                }
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: sM = (sM === 1 ? 0 : 1)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 1
                                        color: cBd
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 60
                                        anchors.margins: 16
                                        Text {
                                            text: "显示重置时间"
                                            font.pixelSize: 14
                                            color: cTxt
                                            Layout.fillWidth: true
                                            Layout.leftMargin: 16
                                        }
                                        Rectangle {
                                            radius: 12
                                            width: 44
                                            height: 24
                                            color: sTm === 1 ? cPri : cBd
                                            Layout.rightMargin: 16
                                            Rectangle {
                                                radius: 10
                                                width: 20
                                                height: 20
                                                color: "white"
                                                y: 2
                                                x: sTm === 1 ? 22 : 2
                                                Behavior on x {
                                                    NumberAnimation {
                                                        duration: 150
                                                        easing.type: Easing.OutQuad
                                                    }
                                                }
                                            }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: sTm = (sTm === 1 ? 0 : 1)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 1
                                        color: cBd
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 60
                                        anchors.margins: 16
                                        Text {
                                            text: "刷新间隔 (分钟)"
                                            font.pixelSize: 14
                                            color: cTxt
                                            Layout.leftMargin: 16
                                        }
                                        Slider {
                                            Layout.fillWidth: true
                                            from: 1
                                            to: 60
                                            stepSize: 1
                                            value: interval
                                            onValueChanged: interval = value
                                        }
                                        Text {
                                            text: interval
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: cMuted
                                            Layout.rightMargin: 16
                                            Layout.minimumWidth: 20
                                        }
                                    }
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 44
                                background: Rectangle {
                                    color: cPri
                                    radius: 8
                                }
                                contentItem: Text {
                                    text: "应用插件配置"
                                    color: "white"
                                    font.pixelSize: 14
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    appSettings.setWidgetShowToken(sT)
                                    appSettings.setWidgetShowMcp(sM)
                                    appSettings.setWidgetShowTime(sTm)
                                    appSettings.setAutoRefreshInterval(interval)
                                    appSettings.syncWidgetConfig()
                                    if (interval > 0)
                                        usageQuery.setAutoRefresh(interval)
                                    else
                                        usageQuery.stopAutoRefresh()
                                    sv.pop()
                                }
                            }
                            Item {
                                Layout.fillWidth: true
                                height: 20
                            }
                        }
                    }
                }
            }
        }
    }

    // ==========================================
    // 3. 详情视图组件 (Detail View)
    // ==========================================
    Component {
        id: detailPageComp
        Page {
            background: Rectangle {
                color: cBg
            }
            property int pIndex: -1
            property var dispData: (pIndex >= 0
                                    && pIndex < platformData.length) ? platformData[pIndex] : null

            Keys.onBackPressed: {
                sv.pop()
            }

            header: Rectangle {
                color: cCard
                height: 70
                border.width: 1
                border.color: cBd
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10
                    ToolButton {
                        text: "←"
                        font.pixelSize: 24
                        font.bold: true
                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 24
                            color: cMuted
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: sv.pop()
                    }
                    Text {
                        text: "用量详情"
                        font.pixelSize: 18
                        font.bold: true
                        color: cTxt
                        Layout.fillWidth: true
                    }
                }
            }

            ScrollView {
                anchors.fill: parent
                visible: dispData !== null
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    anchors.margins: 20
                    Item {
                        Layout.fillWidth: true
                        height: 4
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: detailCol.implicitHeight + 40
                        radius: 16
                        color: cCard
                        border.width: 1
                        border.color: cBd

                        ColumnLayout {
                            id: detailCol
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 20
                            spacing: 16

                            // 标题信息
                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    text: dispData.name
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: cPri
                                    Layout.fillWidth: true
                                }
                                Rectangle {
                                    color: dispData.valid ? "#ECFDF5" : "#FEF2F2"
                                    radius: 6
                                    implicitWidth: 44
                                    implicitHeight: 22
                                    Text {
                                        anchors.centerIn: parent
                                        text: dispData.valid ? "正常" : "异常"
                                        font.pixelSize: 10
                                        font.bold: true
                                        color: dispData.valid ? cOk : cBad
                                    }
                                }
                            }
                            Text {
                                text: "Token " + (dispData.tokenPct
                                                  >= 0 ? dispData.tokenPct.toFixed(
                                                             1) + "%" : "--") + " · MCP "
                                      + (dispData.mcpPct >= 0 ? dispData.mcpPct.toFixed(
                                                                    1) + "%" : "--")
                                font.pixelSize: 12
                                color: cMuted
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: cBd
                            }

                            // 模型用量
                            ColumnLayout {
                                spacing: 8
                                visible: dispData.models
                                         && dispData.models.length > 0
                                Layout.fillWidth: true
                                Text {
                                    text: "■ 模型用量"
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: cMuted
                                }
                                Repeater {
                                    model: dispData.models.length
                                    delegate: Rectangle {
                                        Layout.fillWidth: true
                                        height: 48
                                        radius: 8
                                        color: cItemBg
                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            Text {
                                                text: dispData.models[modelData]
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: cTxt
                                                Layout.fillWidth: true
                                            }
                                            ColumnLayout {
                                                spacing: 2
                                                Text {
                                                    text: (dispData.modelTokens[modelData]
                                                           || 0) + " tokens"
                                                    font.pixelSize: 14
                                                    font.bold: true
                                                    color: cTxt
                                                    Layout.alignment: Qt.AlignRight
                                                }
                                                Text {
                                                    text: (dispData.modelReq[modelData]
                                                           || 0) + " reqs"
                                                    font.pixelSize: 10
                                                    color: cMuted
                                                    Layout.alignment: Qt.AlignRight
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                height: 8
                                visible: dispData.toolNames
                                         && dispData.toolNames.length > 0
                            }

                            // 工具调用
                            ColumnLayout {
                                spacing: 8
                                visible: dispData.toolNames
                                         && dispData.toolNames.length > 0
                                Layout.fillWidth: true
                                Text {
                                    text: "⚡ 工具调用"
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: cMuted
                                }
                                Repeater {
                                    model: dispData.toolNames.length
                                    delegate: Rectangle {
                                        Layout.fillWidth: true
                                        height: 48
                                        radius: 8
                                        color: cItemBg
                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            Text {
                                                text: dispData.toolNames[modelData]
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: cTxt
                                                Layout.fillWidth: true
                                            }
                                            Text {
                                                text: (dispData.toolCalls[modelData]
                                                       || 0) + " calls"
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: cWarn
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        height: 20
                    }
                }
            }
        }
    }
}
