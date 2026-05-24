import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: app; width: 400; height: 780; title: "TokenCheck"
    visible: true; color: "#F3F4F6"

    readonly property color cBg: "#F9FAFB"; readonly property color cCard: "#FFFFFF"
    readonly property color cPri: "#2563EB"; readonly property color cOk: "#10B981"
    readonly property color cWarn: "#F59E0B"; readonly property color cBad: "#F43F5E"
    readonly property color cTxt: "#111827"; readonly property color cMuted: "#6B7280"
    readonly property color cBd: "#E5E7EB"

    property var platformData: []; property bool isRefreshing: false

    function fmtPct(p) { return p < 0 ? "--%" : p.toFixed(1)+"%" }
    function pc(p) { if (p>=80) return cBad; if (p>=50) return cWarn; return cOk }

    Component.onCompleted: { platformData = [] }

    Connections {
        target: usageQuery
        function onQueryFinished(data) {
            var f=false
            for(var i=0;i<platformData.length;i++) if(platformData[i].name===data.platformName)
                {platformData[i]={name:data.platformName,tokenPct:data.tokenPercentage(),mcpPct:data.mcpPercentage(),resetTime:data.tokenResetTime(),valid:data.isValid,error:data.errorMsg,
                    models:data.modelNames(),modelTokens:[],toolNames:[],modelReq:[],toolCalls:[]};
                 for(var j=0;j<data.modelCount();j++){platformData[i].modelTokens.push(data.modelTokens(j));platformData[i].modelReq.push(data.modelRequests(j))}
                 for(var k=0;k<data.toolCount();k++){platformData[i].toolNames.push(data.toolNames()[k]);platformData[i].toolCalls.push(data.toolCalls(k))}
                 f=true; break}
            if(!f) {
                var d={name:data.platformName,tokenPct:data.tokenPercentage(),mcpPct:data.mcpPercentage(),resetTime:data.tokenResetTime(),valid:data.isValid,error:data.errorMsg,
                    models:data.modelNames(),modelTokens:[],toolNames:[],modelReq:[],toolCalls:[]}
                for(var j=0;j<data.modelCount();j++){d.modelTokens.push(data.modelTokens(j));d.modelReq.push(data.modelRequests(j))}
                for(var k=0;k<data.toolCount();k++){d.toolNames.push(data.toolNames()[k]);d.toolCalls.push(data.toolCalls(k))}
                platformData.push(d)
            }
            platformDataChanged()
        }
        function onQueryAllFinished() { isRefreshing = false }
        function onQueryFailed(e) { isRefreshing = false }
    }

    StackView { id: sv; anchors.fill: parent; initialItem: main

        pushEnter: Transition { NumberAnimation { property:"x"; from:sv.width*0.3; to:0; duration:250; easing.type:Easing.OutQuad }
            NumberAnimation { property:"opacity"; from:0; to:1; duration:200 }
        }
        popEnter: Transition { NumberAnimation { property:"x"; from:-sv.width*0.3; to:0; duration:250; easing.type:Easing.OutQuad }
            NumberAnimation { property:"opacity"; from:0; to:1; duration:200 }
        }
        popExit: Transition { NumberAnimation { property:"x"; from:0; to:sv.width*0.3; duration:250; easing.type:Easing.OutQuad }
            NumberAnimation { property:"opacity"; from:1; to:0; duration:200 }
        }

        // ═══════ MAIN ═══════
        Page { id: main; title: "TokenCheck"
            background: Rectangle { color: cBg }
            Keys.onBackPressed: { if(sv.depth>1) sv.pop(); else Qt.quit() }

            header: ToolBar { background: Rectangle { color: cCard }
                RowLayout { anchors.fill: parent; anchors.margins: 16
                    ColumnLayout { spacing:0
                        Text { text:"TokenCheck"; font.pixelSize:20; font.bold:true; color:cTxt }
                        Text { text:"Connected "+platformData.length+" platforms"; font.pixelSize:12; color:cMuted }
                    }
                    Item { Layout.fillWidth:true }
                    ToolButton { font.pixelSize:22; text:"\u2699"
                        contentItem: Text { text:parent.text; font.pixelSize:22; color:cMuted; horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter }
                        onClicked: sv.push(settings)
                    }
                }
            }

            ColumnLayout { anchors.fill:parent; spacing:0

                ScrollView { Layout.fillWidth:true; Layout.fillHeight:true
                ColumnLayout { width:parent.width; spacing:16; anchors.margins:20

                    Rectangle { Layout.fillWidth:true; height:64; radius:12; color:cCard; border.width:1; border.color:cBd; visible:isRefreshing
                        RowLayout { anchors.centerIn:parent; spacing:12
                            BusyIndicator { width:20; height:20; running:isRefreshing }
                            Text { text:"Syncing..."; color:cPri; font.pixelSize:13; font.bold:true }
                        }
                    }

                    Repeater { model:platformData
                        delegate: Rectangle { id:card; Layout.fillWidth:true; radius:16; color:cCard; border.width:1; border.color:cBd; scale:1
                            Behavior on scale { NumberAnimation { duration:100; easing.type:Easing.OutQuad } }
                            MouseArea { anchors.fill:parent; onPressed: card.scale=0.97; onReleased: card.scale=1; onCanceled: card.scale=1
                                onClicked: sv.push(detail, {"pIndex":index})
                            }
                            ColumnLayout { anchors.fill:parent; anchors.margins:18; spacing:14
                                RowLayout { Layout.fillWidth:true
                                    Text { text:modelData.name; font.pixelSize:18; font.bold:true; color:cTxt; Layout.fillWidth:true }
                                    Rectangle { visible:!modelData.valid; radius:6; color:"#FEE2E2"; implicitWidth:52; implicitHeight:22
                                        Text { anchors.centerIn:parent; text:"Invalid"; font.pixelSize:10; font.bold:true; color:"#DC2626" }
                                    }
                                    Text { text:modelData.valid?"\u2714":"\u2718"; font.pixelSize:20; font.bold:true; color:modelData.valid?cOk:cBad }
                                }
                                ColumnLayout { Layout.fillWidth:true; spacing:4
                                    RowLayout { Layout.fillWidth:true
                                        Text { text:"Token"; font.pixelSize:12; color:cMuted; Layout.fillWidth:true }
                                        Text { text:fmtPct(modelData.tokenPct); font.pixelSize:12; font.bold:true; color:pc(modelData.tokenPct) }
                                    }
                                    Rectangle { Layout.fillWidth:true; height:5; radius:3; color:"#E5E7EB"
                                        Rectangle { height:5; radius:3; color:pc(modelData.tokenPct); width:parent.width*Math.min(Math.max(modelData.tokenPct,0),100)/100 }
                                    }
                                }
                                ColumnLayout { Layout.fillWidth:true; spacing:4
                                    RowLayout { Layout.fillWidth:true
                                        Text { text:"MCP"; font.pixelSize:12; color:cMuted; Layout.fillWidth:true }
                                        Text { text:fmtPct(modelData.mcpPct); font.pixelSize:12; font.bold:true; color:pc(modelData.mcpPct) }
                                    }
                                    Rectangle { Layout.fillWidth:true; height:5; radius:3; color:"#E5E7EB"
                                        Rectangle { height:5; radius:3; color:pc(modelData.mcpPct); width:parent.width*Math.min(Math.max(modelData.mcpPct,0),100)/100 }
                                    }
                                }
                                Rectangle { Layout.fillWidth:true; height:1; color:cBd }
                                RowLayout { Layout.fillWidth:true
                                    Text { text:"\u23F0 "+(modelData.resetTime||"--")+" reset"; font.pixelSize:12; color:cMuted; Layout.fillWidth:true }
                                    Text { text:"\u2192"; font.pixelSize:14; color:"#D1D5DB" }
                                }
                            }
                        }
                    }
                } }

                Rectangle { Layout.fillWidth:true; height:76; color:cCard; border.width:1; border.color:cBd
                    Button { id:btnRefresh; anchors.centerIn:parent; implicitWidth:320; implicitHeight:50
                        enabled:!isRefreshing; highlighted:true
                        contentItem: RowLayout { anchors.centerIn:parent; spacing:8
                            Text { text:"\u21BB"; font.pixelSize:18; font.bold:true; color:"white" }
                            Text { text:isRefreshing?"Refreshing...":"Refresh"; font.pixelSize:16; font.bold:true; color:"white" }
                        }
                        onClicked:{ platformData=[]; isRefreshing=true; usageQuery.query() }
                    }
                }
            }
        }

        // ═══════ SETTINGS ═══════
        Page { id: settings; title:"Settings"
            background: Rectangle { color:cBg }
            property int tab:0
            property int interval:5; property int sT:1; property int sM:1; property int sTm:1

            Component.onCompleted: { interval=appSettings.autoRefreshInterval(); sT=appSettings.widgetShowToken(); sM=appSettings.widgetShowMcp(); sTm=appSettings.widgetShowTime() }
            function rf() { plv.model=0; plv.model=appSettings.platformCount() }

            Keys.onBackPressed: { sv.pop() }

            header: ToolBar { background: Rectangle { color:cCard }
                RowLayout { anchors.fill:parent; anchors.margins:12
                    ToolButton { font.pixelSize:22; text:"\u2190"
                        contentItem: Text { text:parent.text; font.pixelSize:22; color:cMuted; horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter }
                        onClicked: sv.pop()
                    }
                    Text { text:"Settings"; font.pixelSize:18; font.bold:true; color:cTxt; Layout.fillWidth:true }
                }
            }

            ColumnLayout { anchors.fill:parent; spacing:0

                Rectangle { Layout.fillWidth:true; Layout.margins:16; height:42; radius:10; color:"#E5E7EB"
                    RowLayout { anchors.fill:parent; anchors.margins:3
                        Rectangle { Layout.fillWidth:true; Layout.fillHeight:true; radius:8; color:tab===0?cCard:"transparent"
                            Text { anchors.centerIn:parent; text:"\uD83D\uDD11 Platforms"; font.pixelSize:14; font.bold:tab===0; color:tab===0?cTxt:cMuted }
                            MouseArea { anchors.fill:parent; onClicked: tab=0 }
                        }
                        Rectangle { Layout.fillWidth:true; Layout.fillHeight:true; radius:8; color:tab===1?cCard:"transparent"
                            Text { anchors.centerIn:parent; text:"\uD83D\uDCE6 Widget"; font.pixelSize:14; font.bold:tab===1; color:tab===1?cTxt:cMuted }
                            MouseArea { anchors.fill:parent; onClicked: tab=1 }
                        }
                    }
                }

                ScrollView { Layout.fillWidth:true; Layout.fillHeight:true
                ColumnLayout { width:parent.width; spacing:14; anchors.margins:16

                    ColumnLayout { visible:tab===0; Layout.fillWidth:true; spacing:14
                        Rectangle { Layout.fillWidth:true; radius:16; color:cCard; border.width:1; border.color:cBd
                            ColumnLayout { anchors.fill:parent; anchors.margins:14; spacing:10
                                Text { text:"Platform Type"; font.pixelSize:12; color:cMuted }
                                ComboBox { id:pCombo; Layout.fillWidth:true; model:["ZAI (Zhipu)","ZHIPU","Custom"] }
                                Text { text:"Auth Token"; font.pixelSize:12; color:cMuted }
                                TextField { id:tField; Layout.fillWidth:true; placeholderText:"sk-..."; echoMode:TextInput.Password }
                                Button { Layout.fillWidth:true; highlighted:true; text:"+ Add"
                                    onClicked: { if(tField.text.length<5) return
                                        var n=pCombo.currentIndex===0?"ZAI":pCombo.currentIndex===1?"ZHIPU":"Custom"
                                        appSettings.addPlatform(n,"https://open.bigmodel.cn",tField.text,"/api/monitor/usage"); tField.text=""; rf(); usageQuery.query() }
                                }
                            }
                        }

                        ListView { id:plv; Layout.fillWidth:true; implicitHeight:Math.min(plv.count*54,260); interactive:plv.count>5; model:appSettings.platformCount()
                            delegate: Rectangle { width:ListView.view.width; height:44; radius:10; color:cCard; border.width:1; border.color:cBd
                                RowLayout { anchors.fill:parent; anchors.margins:12
                                    ColumnLayout { Layout.fillWidth:true; spacing:0
                                        Text { text:appSettings.platformName(index); font.pixelSize:14; font.bold:true; color:cTxt }
                                        Text { text:appSettings.platformAuthToken(index).substring(0,8)+"..."; font.pixelSize:11; color:cMuted }
                                    }
                                    ToolButton { font.pixelSize:14; text:"\u2716"
                                        contentItem: Text { text:parent.text; font.pixelSize:14; color:cBad; horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter }
                                        onClicked:{ appSettings.removePlatform(index); rf() }
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout { visible:tab===1; Layout.fillWidth:true; spacing:14
                        Rectangle { Layout.fillWidth:true; height:110; radius:20; color:cCard; border.width:1; border.color:cBd; scale:0.95
                            ColumnLayout { anchors.fill:parent; anchors.margins:18; spacing:10
                                RowLayout { Layout.fillWidth:true
                                    Rectangle { width:24; height:24; radius:6; color:cPri; Text { anchors.centerIn:parent; text:"Z"; color:"white" } }
                                    Text { text:"ZAI"; font.pixelSize:14; font.bold:true; Layout.fillWidth:true }
                                    Rectangle { radius:10; color:cBd; implicitWidth:56; implicitHeight:20
                                        Text { anchors.centerIn:parent; text:"Updated"; font.pixelSize:10; color:cMuted }
                                    }
                                }
                                RowLayout {
                                    ColumnLayout { Layout.fillWidth:true; visible:sT===1
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"45%"; font.pixelSize:18; font.bold:true; color:cOk }
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"TOKEN"; font.pixelSize:10; color:cMuted }
                                    }
                                    Rectangle { visible:sT===1&&sM===1; width:1; height:30; color:cBd }
                                    ColumnLayout { Layout.fillWidth:true; visible:sM===1
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"22%"; font.pixelSize:18; font.bold:true; color:cWarn }
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"MCP"; font.pixelSize:10; color:cMuted }
                                    }
                                    Rectangle { visible:sM===1&&sTm===1; width:1; height:30; color:cBd }
                                    ColumnLayout { Layout.fillWidth:true; visible:sTm===1
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"14:30"; font.pixelSize:18; font.bold:true; color:cTxt }
                                        Text { Layout.alignment:Qt.AlignHCenter; text:"Reset"; font.pixelSize:10; color:cMuted }
                                    }
                                }
                            }
                        }

                        Rectangle { Layout.fillWidth:true; radius:16; color:cCard; border.width:1; border.color:cBd
                            ColumnLayout { anchors.fill:parent; spacing:0

                                RowLayout { Layout.fillWidth:true; Layout.preferredHeight:54; anchors.margins:14
                                    Text { text:"Show Token %"; font.pixelSize:14; color:cTxt; Layout.fillWidth:true; Layout.leftMargin:14 }
                                    Rectangle { radius:12; width:44; height:24; color:sT===1?cPri:"#D1D5DB"
                                        Rectangle { radius:10; width:18; height:18; color:"#FFF"; x:sT===1?23:3; y:3 }
                                        MouseArea { anchors.fill:parent; onClicked: sT=(sT===1?0:1) }
                                    }
                                    Layout.rightMargin:14
                                }
                                Rectangle { Layout.fillWidth:true; height:1; color:cBd }

                                RowLayout { Layout.fillWidth:true; Layout.preferredHeight:54; anchors.margins:14
                                    Text { text:"Show MCP %"; font.pixelSize:14; color:cTxt; Layout.fillWidth:true; Layout.leftMargin:14 }
                                    Rectangle { radius:12; width:44; height:24; color:sM===1?cPri:"#D1D5DB"
                                        Rectangle { radius:10; width:18; height:18; color:"#FFF"; x:sM===1?23:3; y:3 }
                                        MouseArea { anchors.fill:parent; onClicked: sM=(sM===1?0:1) }
                                    }
                                    Layout.rightMargin:14
                                }
                                Rectangle { Layout.fillWidth:true; height:1; color:cBd }

                                RowLayout { Layout.fillWidth:true; Layout.preferredHeight:54; anchors.margins:14
                                    Text { text:"Show Reset Time"; font.pixelSize:14; color:cTxt; Layout.fillWidth:true; Layout.leftMargin:14 }
                                    Rectangle { radius:12; width:44; height:24; color:sTm===1?cPri:"#D1D5DB"
                                        Rectangle { radius:10; width:18; height:18; color:"#FFF"; x:sTm===1?23:3; y:3 }
                                        MouseArea { anchors.fill:parent; onClicked: sTm=(sTm===1?0:1) }
                                    }
                                    Layout.rightMargin:14
                                }
                                Rectangle { Layout.fillWidth:true; height:1; color:cBd }

                                RowLayout { Layout.fillWidth:true; Layout.preferredHeight:54; anchors.margins:14
                                    Text { text:"Refresh (min)"; font.pixelSize:14; color:cTxt; Layout.leftMargin:14 }
                                    Slider { Layout.fillWidth:true; from:1; to:60; stepSize:1; value:interval; onValueChanged:interval=value }
                                    Text { text:interval; font.pixelSize:14; font.bold:true; color:cMuted; Layout.rightMargin:14; Layout.minimumWidth:20 }
                                }
                            }
                        }

                        Button { Layout.fillWidth:true; highlighted:true; text:"Apply Widget Config"
                            onClicked: { appSettings.setWidgetShowToken(sT); appSettings.setWidgetShowMcp(sM); appSettings.setWidgetShowTime(sTm)
                                appSettings.setAutoRefreshInterval(interval); appSettings.syncWidgetConfig()
                                if(interval>0) usageQuery.setAutoRefresh(interval); else usageQuery.stopAutoRefresh(); sv.pop() }
                        }
                    }
                } }
            }
        }

        // ═══════ DETAIL ═══════
        Page { id: detail; title:"Detail"
            background: Rectangle { color:cBg }
            property int pIndex: -1
            property var dispData: (pIndex>=0 && pIndex<platformData.length) ? platformData[pIndex] : null

            Keys.onBackPressed: { sv.pop() }

            header: ToolBar { background: Rectangle { color:cCard }
                RowLayout { anchors.fill:parent; anchors.margins:12
                    ToolButton { font.pixelSize:22; text:"\u2190"
                        contentItem: Text { text:parent.text; font.pixelSize:22; color:cMuted; horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter }
                        onClicked: sv.pop()
                    }
                    Text { text:"Detail"; font.pixelSize:18; font.bold:true; color:cTxt; Layout.fillWidth:true }
                }
            }

            ScrollView { anchors.fill:parent; visible:dispData!==null
            ColumnLayout { width:parent.width; spacing:16; anchors.margins:16

                Rectangle { Layout.fillWidth:true; radius:16; color:cCard; border.width:1; border.color:cBd
                ColumnLayout { anchors.fill:parent; anchors.margins:18; spacing:12

                    RowLayout { Layout.fillWidth:true
                        Text { text:dispData.name; font.pixelSize:20; font.bold:true; color:cPri; Layout.fillWidth:true }
                        Rectangle { color:dispData.valid?"#ECFDF5":"#FEF2F2"; radius:6; implicitWidth:40; implicitHeight:22
                            Text { anchors.centerIn:parent; text:dispData.valid?"OK":"ERR"; font.pixelSize:10; font.bold:true; color:dispData.valid?cOk:cBad }
                        }
                    }
                    Text { text:"Token "+(dispData.tokenPct>=0?dispData.tokenPct.toFixed(1)+"%":"--")+" . MCP "+(dispData.mcpPct>=0?dispData.mcpPct.toFixed(1)+"%":"--"); font.pixelSize:12; color:cMuted }
                    Rectangle { Layout.fillWidth:true; height:1; color:cBd }

                    ColumnLayout { spacing:6; visible:dispData.models&&dispData.models.length>0; Layout.fillWidth:true
                        Text { text:"Models"; font.pixelSize:12; font.bold:true; color:cMuted }
                        Repeater { model:dispData.models.length
                            delegate: Rectangle { Layout.fillWidth:true; height:42; radius:8; color:cBg
                                RowLayout { anchors.fill:parent; anchors.margins:10
                                    Text { text:dispData.models[modelData]; font.pixelSize:14; color:cTxt; Layout.fillWidth:true }
                                    ColumnLayout { spacing:0
                                        Text { text:(dispData.modelTokens[modelData]||0)+" tokens"; font.pixelSize:14; font.bold:true; color:cTxt }
                                        Text { text:(dispData.modelReq[modelData]||0)+" reqs"; font.pixelSize:10; color:cMuted }
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout { spacing:6; visible:dispData.toolNames&&dispData.toolNames.length>0; Layout.fillWidth:true
                        Text { text:"Tools"; font.pixelSize:12; font.bold:true; color:cMuted }
                        Repeater { model:dispData.toolNames.length
                            delegate: Rectangle { Layout.fillWidth:true; height:42; radius:8; color:cBg
                                RowLayout { anchors.fill:parent; anchors.margins:10
                                    Text { text:dispData.toolNames[modelData]; font.pixelSize:14; color:cTxt; Layout.fillWidth:true }
                                    Text { text:(dispData.toolCalls[modelData]||0)+" calls"; font.pixelSize:14; font.bold:true; color:cWarn }
                                }
                            }
                        }
                    }
                } }
            } }
        }
    }
}
