<div align="center">

# <img src="TokenCheckPC/res/icon.png" width="64" align="center"> TokenCheck

**API 额度，尽在掌握** · *Keep your API quotas in check*

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Android-lightgrey.svg)]()
[![Qt](https://img.shields.io/badge/Qt-5.15%20PC%20%7C%206.8%20Android-41CD52.svg)]()
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)]()

实时监控 **GLM (智谱)** 与 **DeepSeek** API 用量的跨平台工具集。

*A cross-platform toolkit that monitors GLM & DeepSeek API usage in real time.*

<br>

</div>

---

## 📦 项目 · Projects

| | PC Edition | Android Edition |
|---|---|---|
| **目录** | [`TokenCheckPC/`](TokenCheckPC/) | [`TokenCheckAndroid/`](TokenCheckAndroid/) |
| **平台** | Windows x64 | Android arm64-v8a |
| **框架** | Qt 5.15 (Widgets) | Qt 6.8 (QML / Quick) |
| **语言** | C++17 | C++17 |
| **UI** | QPainter 悬浮球 + QWidgets | QML 声明式界面 |
| **特色** | 悬浮球、系统托盘、全局热键 | 桌面小组件、下拉刷新 |
| **状态** | ✅ 可构建 | ✅ 可构建 |

---

## ✨ 功能概览 · Feature Overview

<div align="center">

```
                        ┌─────────────────────────────────────┐
                        │           TokenCheck                │
                        │      API Usage Monitor              │
                        └──────────────┬──────────────────────┘
                    ┌──────────────────┴──────────────────┐
                    │                                     │
            ┌───────▼───────┐                     ┌───────▼───────┐
            │  PC (Windows)  │                     │  Android      │
            ├───────────────┤                     ├───────────────┤
            │ 🟢 悬浮球      │                     │ 📊 仪表盘     │
            │ 🔔 系统托盘    │                     │ 📱 Widget     │
            │ 📊 详情面板    │                     │ 🔄 下拉刷新   │
            │ ⌨️ 全局热键    │                     │ 💰 余额追踪   │
            │ 🎨 深浅主题    │                     │ 🌙 深色主题   │
            │ 🌐 中英双语    │                     │ 💾 离线缓存   │
            └───────┬───────┘                     └───────┬───────┘
                    │                                     │
                    └──────────────────┬──────────────────┘
                          ┌───────────▼───────────┐
                          │   共享核心 · Shared    │
                          │  ┌─────────────────┐  │
                          │  │ PlatformHandler  │  │
                          │  │  ├ GLM Handler   │  │
                          │  │  └ DeepSeek      │  │
                          │  │ UsageQuery       │  │
                          │  │ PlatformRegistry │  │
                          │  └─────────────────┘  │
                          └───────────────────────┘
```

</div>

### 🖥️ PC Edition — 核心特性

| 特性 | 说明 |
|------|------|
| **悬浮球** | 置顶透明悬浮球，进度环实时显示用量百分比与重置时间；边缘吸附、悬停展开、滚轮切换账号 |
| **系统托盘** | 右键菜单：立即查询、详情、账号切换、退出；低额度弹窗提醒 |
| **四合一主窗口** | 📊 详情 · 👤 账号 · 🎨 外观 · ⚙️ 通用 |
| **深色 / 浅色** | Catppuccin Mocha / Latte，一键切换无需重启 |
| **全局热键** | Win32 `RegisterHotKey`，自定义组合键切换悬浮球 / 立即刷新 |
| **中英双语** | `QTranslator` + `.qm` 翻译文件，覆盖全部 100+ UI 文案 |
| **HTTP/SOCKS5** | 可配置代理，支持 HTTP 和 SOCKS5 协议 |
| **离线缓存** | JSON 缓存上次成功数据，启动即显示 |

### 📱 Android Edition — 核心特性

| 特性 | 说明 |
|------|------|
| **桌面小组件** | 主屏 Widget 显示当前平台额度，点击切换账号 / 点击刷新 |
| **详细分析** | 每模型输入/输出 Token、请求次数、工具调用统计 |
| **DeepSeek 余额** | 余额与赠金实时追踪 |
| **下拉刷新** | 主页下拉手动触发查询 |
| **自动刷新** | 可配置 5 / 15 / 30 / 60 分钟后台刷新 |
| **离线缓存** | SharedPreferences 缓存，启动即显示上次数据 |

---

## 🏗️ 架构设计 · Architecture

### 共享架构 · Shared Architecture

```
                    ┌──────────────────────┐
                    │    PlatformHandler    │  ← 策略模式接口
                    │  (Strategy Pattern)   │
                    └──────┬───────┬───────┘
                  ┌────────┘       └────────┐
          ┌───────▼───────┐  ┌──────────────▼──┐
          │  GlmHandler   │  │ DeepSeekHandler  │
          │  (3 endpoints)│  │  (balance)       │
          └───────────────┘  └─────────────────┘

    ┌──────────────────────────────────────────────┐
    │              UsageQuery                       │
    │   Multi-platform parallel query engine        │
    │   · Retry with backoff  · Error refinement    │
    │   · Proxy support      · JSON cache           │
    └──────────────────────────────────────────────┘

    ┌──────────────────┐    ┌──────────────────────┐
    │  PlatformRegistry │    │   DataManager /       │
    │  (Singleton)      │    │   AppSettings         │
    │  Auto-discovery   │    │   JSON persistence    │
    └──────────────────┘    └──────────────────────┘
```

### 项目结构 · Project Structure

<table>
<tr><td width="50%">

#### TokenCheckPC/

```
├── CMakeLists.txt
├── res/
│   ├── app.ico           # EXE 图标
│   ├── app.rc            # 资源脚本
│   └── icon.png          # 应用图标
├── translations/
│   └── TokenCheckPC_zh_CN.ts
└── src/
    ├── main.cpp
    ├── core/
    │   ├── appsettings.h/cpp
    │   ├── datamanager.h/cpp
    │   ├── platformconfig.h
    │   ├── platform_handler.h
    │   └── platform_registry.h/cpp
    ├── network/
    │   └── usagequery.h/cpp
    ├── platforms/
    │   ├── glm/glm_handler.h/cpp
    │   └── deepseek/deepseek_handler.h/cpp
    └── ui/
        ├── floatingball.h/cpp
        ├── mainwindow.h/cpp
        ├── settingsdialog.h/cpp
        ├── trayiconmanager.h/cpp
        ├── globalhotkey.h/cpp
        └── theme.h/cpp
```

</td><td width="50%">

#### TokenCheckAndroid/

```
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── usagequery.h/cpp
│   ├── appsettings.h/cpp
│   ├── widgetbridge.h/cpp
│   ├── androidprefs.h/cpp
│   ├── platformconfig.h
│   ├── platform_handler.h
│   ├── platform_registry.h/cpp
│   ├── qml/
│   │   ├── Theme.qml
│   │   ├── MainPage.qml
│   │   ├── MainContentPage.qml
│   │   ├── DetailPage.qml
│   │   ├── SettingsPage.qml
│   │   └── PlatformEditPage.qml
│   └── platforms/
│       ├── glm/
│       │   ├── glm_handler.h/cpp
│       │   └── qml/GlmCard.qml, GlmDetail.qml
│       └── deepseek/
│           ├── deepseek_handler.h/cpp
│           └── qml/DeepSeekCard.qml, ...
├── android/
│   ├── AndroidManifest.xml
│   ├── src/.../TokenCheckWidget.java
│   │   └── WidgetDataStore.java
│   └── res/
│       ├── layout/tokencheck_widget.xml
│       └── mipmap-*/ic_launcher.png
└── README.md
```

</td></tr>
</table>

---

## 🚀 快速开始 · Quick Start

### PC (Windows)

```bash
# 前置：Qt 5.15.2 MinGW 81 64-bit + CMake 3.14+
cd TokenCheckPC
cmake -S . -B build -G "MinGW Makefiles" \
  -DCMAKE_PREFIX_PATH=<Qt_path>/5.15.2/mingw81_64 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

输出：`build/TokenCheckPC.exe` + `icon.png` + OpenSSL DLLs + 翻译文件

### Android

```bash
# 前置：Qt 6.8.2 Android + SDK API 34 + NDK
cd TokenCheckAndroid
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<Qt_android_toolchain> -DANDROID_ABI=arm64-v8a
cmake --build .
```

或在 Qt Creator 中直接打开 `CMakeLists.txt`，选择 Android Kit。

---

## 🔌 支持的 API · Supported APIs

| 平台 · Platform | 接口 · Endpoint | 返回数据 · Returns |
|---|---|---|
| **GLM (智谱)** | `{prefix}/model-usage` | 每模型 Token 用量、请求次数 |
| | `{prefix}/tool-usage` | 工具调用次数 |
| | `{prefix}/quota/limit` | Token / MCP 配额、已用百分比、重置时间 |
| **DeepSeek** | `/user/balance` | 账户余额、赠金余额、币种 |

GLM 默认 `prefix` = `/api/monitor/usage`，每账号可自定义。

---

## 🛠️ 技术栈 · Tech Stack

<table>
<tr>
<th></th>
<th>🪟 PC (Windows)</th>
<th>📱 Android</th>
</tr>
<tr>
<td><strong>Framework</strong></td>
<td>Qt 5.15 (Widgets)</td>
<td>Qt 6.8 (QML / Quick)</td>
</tr>
<tr>
<td><strong>Language</strong></td>
<td>C++17</td>
<td>C++17</td>
</tr>
<tr>
<td><strong>UI Engine</strong></td>
<td>QPainter + QWidgets</td>
<td>QML declarative</td>
</tr>
<tr>
<td><strong>Network</strong></td>
<td colspan="2" align="center">QNetworkAccessManager (HTTPS)</td>
</tr>
<tr>
<td><strong>Theme</strong></td>
<td>Dynamic QSS from Catppuccin palette</td>
<td>QML Theme singleton (Catppuccin Mocha)</td>
</tr>
<tr>
<td><strong>i18n</strong></td>
<td>QTranslator + .qm (100+ strings)</td>
<td>—</td>
</tr>
<tr>
<td><strong>Native</strong></td>
<td>Win32 RegisterHotKey</td>
<td>JNI SharedPreferences + AppWidget</td>
</tr>
<tr>
<td><strong>Data Store</strong></td>
<td>JSON file (QSettings)</td>
<td>SharedPreferences (JNI)</td>
</tr>
<tr>
<td><strong>Build</strong></td>
<td>CMake + MinGW</td>
<td>CMake + Android NDK</td>
</tr>
</table>

---

## ⚙️ 配置 · Configuration

### PC Edition

| 配置 · Setting | 默认 · Default | 说明 · Description |
|---|---|---|
| Theme | Dark | 深色 (Catppuccin Mocha) / 浅色 (Latte) |
| Language | System | 跟随系统 / EN / ZH (切换需重启) |
| Ball Size | 90 px | 悬浮球直径 (60–150 px) |
| Refresh Interval | 15 min | 自动刷新间隔 (1–120 分钟) |
| Notify Threshold | 20% | 已用超 80% 时弹窗警告 |
| Global Hotkeys | — | 自定义组合键 (Win32 API) |
| Proxy | None | HTTP / SOCKS5 |

### Android Edition

| 配置 · Setting | 默认 · Default | 说明 · Description |
|---|---|---|
| Widget Show Token | ✓ | 小组件显示 Token 百分比 |
| Widget Show MCP | ✓ | 小组件显示 MCP 百分比 |
| Widget Show Reset | ✓ | 小组件显示重置时间 |
| Widget Font Size | 14 | 小组件文字大小 (px) |
| Auto Refresh | 15 min | 后台自动刷新间隔 |

---

## 📸 截图 · Screenshots

> *TODO: 添加截图 / Add screenshots here*

---

## 🗺️ 路线图 · Roadmap

- [ ] 更多平台支持 (Claude, GPT, etc.) · *More platform support*
- [ ] 用量趋势图表 · *Usage trend charts*
- [ ] 数据导出 (CSV/JSON) · *Data export*
- [ ] 跨平台同步 · *Cross-device sync*

---

## 📄 许可证 · License

[MIT License](LICENSE) — 自由使用，请注明来源。

*Free to use, just credit the original author.*

---

<div align="center">

<br>

**TokenCheck** © ZaiTech · Built with ❤️ and Qt

<br>

*如果这个项目对你有帮助，请给个 ⭐ Star ！*

*If this project helps you, please consider giving it a ⭐ Star!*

</div>
