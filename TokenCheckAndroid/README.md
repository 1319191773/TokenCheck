# TokenCheck

API token usage monitor for Android and iOS with home screen / lock screen widget. Tracks GLM and DeepSeek API quotas in real time.

Built with Qt 6.8.2 / C++17 / QML, targeting Android arm64-v8a and iOS (arm64).

## Features

- **Multi-platform management** — add, edit, enable/disable multiple GLM and DeepSeek API accounts
- **Real-time dashboard** — Token usage, MCP quota, reset time at a glance
- **Detailed breakdown** — per-model input/output tokens, request count, tool call stats, quota details
- **Home screen widget (Android)** — shows current platform quota with tap-to-switch and tap-to-refresh
- **Lock screen widget (iOS)** — WidgetKit extension shows token/MCP usage and balance
- **DeepSeek support** — balance display, granted balance tracking
- **Offline cache** — last successful query is cached locally for instant display on startup
- **Auto refresh** — configurable interval (5 / 15 / 30 / 60 minutes)
- **Pull-to-refresh** — pull down on the main page to manually refresh
- **Dark theme** — Catppuccin Mocha based dark UI

## Screenshots

*(Add screenshots here)*

## Build Requirements

### Android
- Qt 6.8.2 (Android arm64-v8a)
- Android SDK with API level 34 (android-34)
- Android NDK (Clang arm64-v8a)
- OpenSSL 3.x for Android (cached in build)

### iOS
- macOS with Xcode 15+
- Qt 6.8.2 (iOS arm64)
- Apple Developer account (for device deployment)

### Common
- CMake 3.16+
- C++17

## Build

### Android

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<Qt_android_toolchain_path> -DANDROID_ABI=arm64-v8a
cmake --build .
```

Or open in Qt Creator and build for Android.

### iOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<Qt_ios_toolchain_path> -DPLATFORM=IOS
cmake --build .
```

The iOS WidgetKit extension (`ios/widget/`) must be built separately in Xcode:
1. Open the generated Xcode project or create a new WidgetKit target
2. Add `ios/widget/TokenCheckWidget.swift` and `ios/widget/TokenCheckWidgetBundle.swift`
3. Set the Widget target's App Group to `group.com.tokencheck.shared`
4. Build and deploy alongside the main app

## Project Structure

```
├── CMakeLists.txt
├── src/
│   ├── main.cpp              # App entry point, context properties
│   ├── usagequery.h/cpp      # API query engine (model/tool/quota endpoints)
│   ├── appsettings.h/cpp     # Settings singleton, platform config, offline cache
│   ├── widgetbridge.h/cpp    # Bridge data from Qt to Android widget SharedPreferences
│   ├── androidprefs.h/cpp    # Android SharedPreferences read/write via JNI
│   ├── iosappgroups.h/mm     # iOS App Group data sharing bridge (ObjC++)
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
├── ios/
│   ├── Info.plist                  # iOS app bundle configuration
│   ├── LaunchScreen.storyboard     # iOS launch screen
│   └── widget/
│       ├── TokenCheckWidget.swift        # WidgetKit widget (SwiftUI)
│       └── TokenCheckWidgetBundle.swift  # Widget entry point
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

### Android
All settings stored in `tokencheck_prefs` (Android SharedPreferences):
- Platform list (name, base URL, auth token, API prefix, enabled)
- Widget display options (show Token / MCP / reset time, font size)
- Auto-refresh interval
- Offline cache (last successful API response)

### iOS
Settings stored via QSettings (NSUserDefaults). Widget data shared via App Group container:
- `widget_config.json` — widget display preferences
- `platform_N.json` — per-platform usage data (name, slots with values)
- `widget_meta.json` — timestamp and platform count

## License

MIT License — use freely, just credit the original author.
