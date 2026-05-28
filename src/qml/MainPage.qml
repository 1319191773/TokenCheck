import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: app
    width: 400
    height: 800
    title: "GLM Usage"
    visible: true
    color: Theme.bg

    property var platformData: []
    property bool isRefreshing: false
    property string lastError: ""

    function rebuildPlatformData(data) {
        var f = false
        for (var i = 0; i < platformData.length; i++) {
            if (platformData[i].name === data.platformName) {
                platformData[i] = extractPlatformInfo(data)
                f = true
                break
            }
        }
        if (!f)
            platformData.push(extractPlatformInfo(data))
        platformDataChanged()
    }

    function extractPlatformInfo(data) {
        var d = {
            "name": data.platformName,
            "tokenPct": data.tokenPercentage(),
            "mcpPct": data.mcpPercentage(),
            "resetTime": data.tokenResetTime(),
            "valid": data.isValid,
            "error": data.errorMsg,
            "models": data.modelNames(),
            "modelProvider": [],
            "modelTokens": [],
            "modelInputTokens": [],
            "modelOutputTokens": [],
            "modelReq": [],
            "toolNames": [],
            "toolCalls": [],
            "quotas": []
        }
        for (var j = 0; j < data.modelCount(); j++) {
            d.modelProvider.push(data.modelProvider(j))
            d.modelTokens.push(data.modelTokens(j))
            d.modelInputTokens.push(data.modelInputTokens(j))
            d.modelOutputTokens.push(data.modelOutputTokens(j))
            d.modelReq.push(data.modelRequests(j))
        }
        for (var k = 0; k < data.toolCount(); k++) {
            d.toolNames.push(data.toolNames()[k])
            d.toolCalls.push(data.toolCalls(k))
        }
        for (var q = 0; q < data.quotaCount(); q++) {
            d.quotas.push({
                "type": data.quotaType(q),
                "percentage": data.quotaPercentage(q),
                "currentUsage": data.quotaCurrentUsage(q),
                "total": data.quotaTotal(q),
                "remaining": data.quotaRemaining(q),
                "unit": data.quotaUnit(q),
                "number": data.quotaNumber(q),
                "resetTime": data.quotaResetTime(q),
                "usageDetails": data.quotaUsageDetails(q)
            })
        }

        var totalTokens = 0
        for (var t = 0; t < d.modelTokens.length; t++)
            totalTokens += d.modelTokens[t]
        d.totalTokens = totalTokens

        var totalReqs = 0
        for (var r = 0; r < d.modelReq.length; r++)
            totalReqs += d.modelReq[r]
        d.totalReqs = totalReqs

        var totalToolCalls = 0
        for (var c = 0; c < d.toolCalls.length; c++)
            totalToolCalls += d.toolCalls[c]
        d.totalToolCalls = totalToolCalls

        return d
    }

    Connections {
        target: usageQuery
        function onQueryFinished(data) {
            rebuildPlatformData(data)
        }
        function onQueryAllFinished() {
            isRefreshing = false
            lastError = ""
        }
        function onQueryFailed(error) {
            isRefreshing = false
            lastError = error
        }
        function onCachedDataAvailable(json) {
            try {
                var arr = JSON.parse(json)
                if (arr.data) arr = arr.data
                for (var i = 0; i < arr.length; i++) {
                    var d = arr[i]
                    platformData.push({
                        "name": d.platformName || "",
                        "tokenPct": d.tokenPct !== undefined ? d.tokenPct : -1,
                        "mcpPct": d.mcpPct !== undefined ? d.mcpPct : -1,
                        "resetTime": d.resetTime || "",
                        "valid": d.isValid || false,
                        "error": d.errorMsg || "",
                        "models": (d.models || []).map(function(m) { return m.name }),
                        "modelProvider": (d.models || []).map(function(m) { return m.provider || "" }),
                        "modelTokens": (d.models || []).map(function(m) { return m.tokens }),
                        "modelInputTokens": (d.models || []).map(function(m) { return m.inputTokens || 0 }),
                        "modelOutputTokens": (d.models || []).map(function(m) { return m.outputTokens || 0 }),
                        "modelReq": (d.models || []).map(function(m) { return m.requests }),
                        "toolNames": (d.tools || []).map(function(t) { return t.name }),
                        "toolCalls": (d.tools || []).map(function(t) { return t.calls }),
                        "quotas": (d.quotas || []).map(function(q) {
                            return {
                                "type": q.type || "",
                                "percentage": q.percentage || 0,
                                "currentUsage": q.currentUsage || 0,
                                "total": q.total || 0,
                                "remaining": q.remaining || 0,
                                "unit": q.unit || 0,
                                "number": q.number || 0,
                                "resetTime": q.resetTime || "",
                                "usageDetails": q.usageDetails || ""
                            }
                        })
                    })
                }
                platformDataChanged()
            } catch(e) {}
        }
    }

    Component.onCompleted: {
        usageQuery.loadCache()
    }

    StackView {
        id: sv
        anchors.fill: parent
        initialItem: mainContentComp

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
        pushExit: Transition {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
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

    Component {
        id: mainContentComp
        MainContentPage {
            stackView: sv
            platformData: app.platformData
            isRefreshing: app.isRefreshing
            lastError: app.lastError

            onRefreshRequested: {
                app.platformData = []
                app.isRefreshing = true
                app.lastError = ""
                usageQuery.query()
            }

            onErrorDismissed: {
                app.lastError = ""
            }

            onPlatformClicked: function(index) {
                sv.push("DetailPage.qml", {
                    "stackView": sv,
                    "platformData": app.platformData[index]
                })
            }
        }
    }
}
