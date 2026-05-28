# TokenCheck

GLM API usage monitor for Android with home screen widget.

Built with Qt 6.8.2 / C++17 / QML, targeting Android arm64-v8a.

## Features

- Multi-platform management — add, edit, enable/disable multiple GLM API accounts
- Real-time dashboard — Token usage, MCP quota, reset time at a glance
- Detailed breakdown — per-model input/output tokens, request count, tool call stats, quota details
- Home screen widget — shows current platform quota with tap-to-switch and tap-to-refresh
- Offline cache — last successful query is cached locally for instant display on startup
- Auto refresh — configurable interval (5 / 15 / 30 / 60 minutes)
- Pull-to-refresh — pull down on the main page to manually refresh

## Screenshots

*(Add screenshots here)*

## Build Requirements

- Qt 6.8.2 (Android)
- Android SDK with API level 34 (android-34)
- Android NDK (Clang arm64-v8a)
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
│   └── qml/
│       ├── qmldir            # Theme singleton registration
│       ├── Theme.qml          # Global theme (colors, spacing, fonts, helpers)
│       ├── MainPage.qml       # Navigation shell, data hub
│       ├── MainContentPage.qml # Card list with pull-to-refresh
│       ├── DetailPage.qml     # Full usage details per platform
│       ├── SettingsPage.qml   # Platform management, widget config
│       └── PlatformEditPage.qml # Add/edit platform form
├── android/
│   ├── AndroidManifest.xml
│   ├── src/com/glm/usage/
│   │   ├── GlmUsageWidget.java    # Home screen widget provider
│   │   └── WidgetDataStore.java   # Thread-safe widget data store
│   └── res/                       # Layouts, drawables, mipmaps
└── README.md
```

## API Endpoints Used

| Endpoint | Description |
|----------|-------------|
| `{prefix}/model-usage` | Per-model token usage and request count |
| `{prefix}/tool-usage` | Per-tool call count |
| `{prefix}/quota/limit` | Token and MCP quota with percentage, remaining, reset time |

Default `prefix` is `/api/monitor/usage`. Configurable per platform.

## Configuration

All settings stored in `glm_usage_prefs` (Android SharedPreferences):

- Platform list (name, base URL, auth token, API prefix, enabled)
- Widget display options (show Token / MCP / reset time, font size)
- Auto-refresh interval
- Offline cache (last successful API response)

## License

MIT
