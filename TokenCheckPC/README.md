# TokenCheckPC

Desktop API token usage monitor for Windows. Tracks GLM and DeepSeek API quotas in real time with a floating ball, system tray icon, and detailed dashboard.

Built with Qt 5.15.2 / C++17 / CMake, targeting Windows x64 (MinGW).

## Features

- **Floating ball** — always-on-top translucent ball with progress ring, showing usage percentage and reset time. Edge-snap, hover-reveal, glow animation. Scroll wheel to switch accounts
- **System tray** — tray icon with context menu: query now, show detail, account switching, quit. Low-quota balloon notifications
- **Dashboard** — tabbed MainWindow with 4 tabs:
  - **Detail** — account selector, summary cards (tokens/requests/models/tools), quota progress bars, per-model usage table, tool call stats
  - **Accounts** — account list with quick add (3 fields) and full edit (all fields)
  - **Appearance** — dark/light theme (Catppuccin Mocha/Latte), floating ball size, time/percent font size and color
  - **General** — language (EN/ZH), refresh interval, auto-start, notification threshold, global hotkeys, HTTP/SOCKS5 proxy
- **Multi-platform** — GLM (ZhiPu) and DeepSeek accounts, queried in parallel
- **Offline cache** — last successful query cached to JSON, shown instantly on startup
- **Dark / Light theme** — instant switching via Catppuccin palettes, no restart
- **i18n** — English and Simplified Chinese (restart required to switch)
- **Global hotkeys** — Windows native `RegisterHotKey` for toggle ball visibility and force refresh
- **Auto refresh** — configurable interval (1–120 minutes)
- **Proxy** — HTTP or SOCKS5 proxy support

## Screenshots

*(Add screenshots here)*

## Build Requirements

- Qt 5.15.2 (MinGW 81 64-bit)
- MinGW compiler (GCC 13.1+)
- CMake 3.14+
- C++17
- OpenSSL 1.1.x DLLs (for HTTPS requests)

## Build

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=<Qt_path>/5.15.2/mingw81_64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Output: `build/TokenCheckPC.exe` along with `icon.png`, translation `.qm`, and OpenSSL DLLs.

## Project Structure

```
├── CMakeLists.txt
├── res/
│   ├── app.ico              # Windows exe icon (multi-size)
│   ├── app.rc               # Resource script for exe icon
│   └── icon.png             # App icon (tray + window)
├── translations/
│   └── TokenCheckPC_zh_CN.ts  # Chinese translations
└── src/
    ├── main.cpp             # Entry point, signal wiring
    ├── core/
    │   ├── appsettings.h/cpp       # JSON settings persistence
    │   ├── datamanager.h/cpp       # Centralized data store (singleton)
    │   ├── platformconfig.h        # PlatformConfig DTO
    │   ├── platform_descriptor.h   # PlatformDescriptor (metadata)
    │   ├── platform_handler.h      # PlatformHandler interface (Strategy pattern)
    │   └── platform_registry.h/cpp # PlatformRegistry (singleton)
    ├── network/
    │   └── usagequery.h/cpp        # Multi-platform parallel query engine
    ├── platforms/
    │   ├── glm/
    │   │   └── glm_handler.h/cpp   # GLM (ZhiPu) handler — 3 endpoints
    │   └── deepseek/
    │       └── deepseek_handler.h/cpp # DeepSeek handler — balance endpoint
    └── ui/
        ├── floatingball.h/cpp      # Floating ball widget
        ├── mainwindow.h/cpp        # Main window with 4 tabs
        ├── settingsdialog.h/cpp    # HotkeyButton, QuickAddDialog, AccountEditDialog
        ├── trayiconmanager.h/cpp   # System tray icon + notifications
        ├── globalhotkey.h/cpp      # Windows RegisterHotKey wrapper
        └── theme.h/cpp             # ThemeId, ThemePalette, dynamic QSS
```

## API Endpoints

| Platform | Endpoint | Description |
|----------|----------|-------------|
| GLM | `{prefix}/model-usage` | Per-model token usage and request count |
| GLM | `{prefix}/tool-usage` | Per-tool call count |
| GLM | `{prefix}/quota/limit` | Token and MCP quota with percentage, reset time |
| DeepSeek | `/user/balance` | Account balance info |

Default GLM `prefix` is `/api/monitor/usage`. Configurable per account.

## Configuration

Settings stored in `QStandardPaths::AppDataLocation/settings.json`:

- Platform list (name, base URL, auth token, API prefix, enabled)
- Theme (Dark / Light)
- Language (system default / EN / ZH)
- Floating ball size, time/percent font size and color
- Auto-refresh interval, auto-start on login
- Notification threshold (1–50%, default 20%)
- Global hotkey sequences (toggle ball, refresh)
- HTTP/SOCKS5 proxy settings

## License

MIT License — use freely, just credit the original author.
