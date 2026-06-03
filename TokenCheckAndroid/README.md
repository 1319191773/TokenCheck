# TokenCheck

API token usage monitor for Android with home screen widget. Tracks GLM and DeepSeek API quotas in real time.

Built with Qt 6.8.2 / C++17 / QML, targeting Android arm64-v8a.

## Features

- **Multi-platform management** — add, edit, enable/disable multiple GLM and DeepSeek API accounts
- **Real-time dashboard** — Token usage, MCP quota, reset time at a glance
- **Detailed breakdown** — per-model input/output tokens, request count, tool call stats, quota details
- **Home screen widget** — shows current platform quota with tap-to-switch and tap-to-refresh
- **DeepSeek support** — balance display, granted balance tracking
- **Offline cache** — last successful query is cached locally for instant display on startup
- **Auto refresh** — configurable interval (5 / 15 / 30 / 60 minutes)
- **Pull-to-refresh** — pull down on the main page to manually refresh
- **Dark theme** — Catppuccin Mocha based dark UI

## Screenshots

*(Add screenshots here)*

## Build Requirements

- Qt 6.8.2 (Android arm64-v8a)
- Android SDK with API level 34 (android-34)
- Android NDK (Clang arm64-v8a)
- OpenSSL 3.x for Android (cached in build)
- CMake 3.16+
- C++17

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<Qt_android_toolchain_path> -DANDROID_ABI=arm64-v8a
cmake --build .
```

Or open in Qt Creator and build for Android.

## Project Structure

```
├── CMakeLists.txt
├── src/
│   ├── main.cpp              # App entry point, context properties
│   ├── usagequery.h/cpp      # API query engine (model/tool/quota endpoints)
│   ├── appsettings.h/cpp     # Settings singleton, platform config, offline cache
│   ├── widgetbridge.h/cpp    # Bridge data from Qt to Android widget SharedPreferences
│   ├── androidprefs.h/cpp    # Android SharedPreferences read/write via JNI
│   ├── platformconfig.h      # PlatformConfig DTO
│   ├── platform_descriptor.h # PlatformDescriptor (metadata)
│   ├── platform_handler.h    # PlatformHandler interface (Strategy pattern)
│   ├── platform_registry.h/cpp # PlatformRegistry (singleton)
│   ├── qml/
│   │   ├── qmldir            # Theme singleton registration
│   │   ├── Theme.qml          # Global theme (colors, spacing, fonts, helpers)
│   │   ├── MainPage.qml       # Navigation shell, data hub
│   │   ├── MainContentPage.qml # Card list with pull-to-refresh
│   │   ├── DetailPage.qml     # Full usage details per platform
│   │   ├── SettingsPage.qml   # Platform management, widget config
│   │   └── PlatformEditPage.qml # Add/edit platform form
│   └── platforms/
│       ├── glm/
│       │   ├── glm_handler.h/cpp   # GLM handler — 3 endpoints
│       │   └── qml/
│       │       ├── qmldir
│       │       ├── GlmCard.qml      # GLM account card
│       │       └── GlmDetail.qml    # GLM detail view
│       └── deepseek/
│           ├── deepseek_handler.h/cpp # DeepSeek handler — balance
│           └── qml/
│               ├── qmldir
│               ├── DeepSeekCard.qml    # DeepSeek account card
│               └── DeepSeekDetail.qml  # DeepSeek detail view
├── android/
│   ├── AndroidManifest.xml
│   ├── src/com/tokencheck/android/
│   │   ├── TokenCheckWidget.java    # Home screen widget provider
│   │   └── WidgetDataStore.java     # Thread-safe widget data store
│   └── res/                         # Layouts, drawables, mipmaps
└── README.md
```

## API Endpoints

| Platform | Endpoint | Description |
|----------|----------|-------------|
| GLM | `{prefix}/model-usage` | Per-model token usage and request count |
| GLM | `{prefix}/tool-usage` | Per-tool call count |
| GLM | `{prefix}/quota/limit` | Token and MCP quota with percentage, reset time |
| DeepSeek | `/user/balance` | Account balance info |

Default GLM `prefix` is `/api/monitor/usage`. Configurable per platform.

## Configuration

All settings stored in `tokencheck_prefs` (Android SharedPreferences):

- Platform list (name, base URL, auth token, API prefix, enabled)
- Widget display options (show Token / MCP / reset time, font size)
- Auto-refresh interval
- Offline cache (last successful API response)

## License

MIT License — use freely, just credit the original author.
