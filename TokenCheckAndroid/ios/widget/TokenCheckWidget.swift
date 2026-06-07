import WidgetKit
import SwiftUI

struct Provider: TimelineProvider {
    func placeholder(in context: Context) -> SimpleEntry {
        SimpleEntry(date: Date(), name: "GLM", tokenPct: 45.0, mcpPct: 22.0, resetTime: "23:00")
    }

    func getSnapshot(in context: Context, completion: @escaping (SimpleEntry) -> Void) {
        let entry = loadEntry() ?? placeholder(in: context)
        completion(entry)
    }

    func getTimeline(in context: Context, completion: @escaping (Timeline<Entry>) -> Void) {
        let entry = loadEntry() ?? SimpleEntry(date: Date(), name: "--", tokenPct: -1, mcpPct: -1, resetTime: "")
        let timeline = Timeline(entries: [entry], policy: .atEnd)
        completion(timeline)
    }

    private func loadEntry() -> SimpleEntry? {
        guard let containerURL = FileManager.default.containerURL(
            forSecurityApplicationGroupIdentifier: "group.com.tokencheck.shared") else { return nil }

        let metaURL = containerURL.appendingPathComponent("widget_meta.json")
        guard let metaData = try? Data(contentsOf: metaURL),
              let meta = try? JSONSerialization.jsonObject(with: metaData) as? [String: Any],
              let count = meta["platformCount"] as? Int, count > 0 else { return nil }

        let platformURL = containerURL.appendingPathComponent("platform_0.json")
        guard let platformData = try? Data(contentsOf: platformURL),
              let platform = try? JSONSerialization.jsonObject(with: platformData) as? [String: Any],
              let name = platform["name"] as? String else { return nil }

        var tokenPct: Double = -1
        var mcpPct: Double = -1
        var resetTime: String = ""
        var balance: String = ""
        var granted: String = ""

        if let slots = platform["slots"] as? [[String: Any]] {
            for slot in slots {
                if let key = slot["dataKey"] as? String, let value = slot["value"] as? String {
                    if key == "token", let v = Double(value) { tokenPct = v }
                    if key == "mcp", let v = Double(value) { mcpPct = v }
                    if key == "time" { resetTime = value }
                    if key == "balance" { balance = value }
                    if key == "granted" { granted = value }
                }
            }
        }

        return SimpleEntry(date: Date(), name: name, tokenPct: tokenPct, mcpPct: mcpPct,
                           resetTime: resetTime, balance: balance, granted: granted)
    }
}

struct SimpleEntry: TimelineEntry {
    let date: Date
    let name: String
    let tokenPct: Double
    let mcpPct: Double
    let resetTime: String
    let balance: String
    let granted: String
}

struct TokenCheckWidgetEntryView: View {
    var entry: Provider.Entry

    var body: some View {
        VStack(spacing: 8) {
            Text(entry.name)
                .font(.headline)
                .fontWeight(.bold)

            if !entry.balance.isEmpty && entry.balance != "-1.0" {
                HStack {
                    Text("Balance")
                        .font(.caption)
                        .foregroundColor(.secondary)
                    Spacer()
                    Text(entry.balance)
                        .font(.caption)
                        .fontWeight(.bold)
                        .foregroundColor(.green)
                }
                if !entry.granted.isEmpty && entry.granted != "-1.0" {
                    HStack {
                        Text("Granted")
                            .font(.caption)
                            .foregroundColor(.secondary)
                        Spacer()
                        Text(entry.granted)
                            .font(.caption)
                            .fontWeight(.semibold)
                    }
                }
            } else {
                if entry.tokenPct >= 0 {
                    HStack {
                        Text("Token")
                            .font(.caption)
                            .foregroundColor(.secondary)
                        Spacer()
                        Text(String(format: "%.1f%%", entry.tokenPct))
                            .font(.caption)
                            .fontWeight(.bold)
                            .foregroundColor(colorForPct(entry.tokenPct))
                    }
                    ProgressView(value: entry.tokenPct, total: 100)
                        .tint(colorForPct(entry.tokenPct))
                }

                if entry.mcpPct >= 0 {
                    HStack {
                        Text("MCP")
                            .font(.caption)
                            .foregroundColor(.secondary)
                        Spacer()
                        Text(String(format: "%.1f%%", entry.mcpPct))
                            .font(.caption)
                            .fontWeight(.bold)
                            .foregroundColor(colorForPct(entry.mcpPct))
                    }
                }

                if !entry.resetTime.isEmpty {
                    Text("Reset: \(entry.resetTime)")
                        .font(.caption2)
                        .foregroundColor(.secondary)
                }
            }
        }
        .padding()
    }

    private func colorForPct(_ pct: Double) -> Color {
        if pct >= 80 { return .red }
        if pct >= 50 { return .orange }
        return .green
    }
}

struct TokenCheckWidget: Widget {
    let kind: String = "TokenCheckWidget"

    var body: some WidgetConfiguration {
        StaticConfiguration(kind: kind, provider: Provider()) { entry in
            TokenCheckWidgetEntryView(entry: entry)
                .containerBackground(.fill.tertiary, for: .widget)
        }
        .configurationDisplayName("TokenCheck")
        .description("Monitor API token usage.")
        .supportedFamilies([.systemSmall, .systemMedium])
    }
}
