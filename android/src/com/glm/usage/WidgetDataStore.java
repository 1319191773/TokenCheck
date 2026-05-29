package com.glm.usage;

import android.content.Context;
import android.content.SharedPreferences;

public class WidgetDataStore {
    private static final String PREFS_NAME = "glm_usage_prefs";
    private static volatile SharedPreferences prefs;
    private static final Object lock = new Object();

    public static void init(Context context) {
        if (prefs == null) {
            synchronized (lock) {
                if (prefs == null) {
                    prefs = context.getApplicationContext()
                            .getSharedPreferences(PREFS_NAME, Context.MODE_MULTI_PROCESS);
                }
            }
        }
    }

    public static void forceReload(Context context) {
        synchronized (lock) {
            prefs = context.getApplicationContext()
                    .getSharedPreferences(PREFS_NAME, Context.MODE_MULTI_PROCESS);
        }
    }

    public static class PlatformInfo {
        public String name;
        public String type;
        public double tokenPct;
        public double mcpPct;
        public String resetTime;
        public String balance;
        public String granted;
        public PlatformInfo(String n, String t, double tk, double m, String r, String b, String g) {
            name = n != null ? n : "";
            type = t != null ? t : "glm";
            tokenPct = tk;
            mcpPct = m;
            resetTime = r != null ? r : "";
            balance = b != null ? b : "";
            granted = g != null ? g : "";
        }
    }

    public static class WidgetData {
        public int widgetShowToken, widgetShowMcp, widgetShowTime, widgetFontSize;
        public int widgetShowBalance, widgetShowGranted;
        public int platformCount, currentPlatform;
        public PlatformInfo[] platforms;
        public WidgetData() { platforms = new PlatformInfo[0]; }
    }

    public static WidgetData getData(Context context, boolean forceRefresh) {
        init(context);

        WidgetData d = new WidgetData();
        synchronized (lock) {
            d.widgetShowToken = parseIntSafe(prefs.getString("widgetShowToken", "1"));
            d.widgetShowMcp = parseIntSafe(prefs.getString("widgetShowMcp", "1"));
            d.widgetShowTime = parseIntSafe(prefs.getString("widgetShowTime", "1"));
            d.widgetShowBalance = parseIntSafe(prefs.getString("widgetShowBalance", "1"));
            d.widgetShowGranted = parseIntSafe(prefs.getString("widgetShowGranted", "1"));
            d.widgetFontSize = parseIntSafe(prefs.getString("widgetFontSize", "14"));
            d.platformCount = parseIntSafe(prefs.getString("platformCount", "0"));
            d.currentPlatform = parseIntSafe(prefs.getString("currentPlatform", "0"));

            d.platforms = new PlatformInfo[d.platformCount];
            for (int i = 0; i < d.platformCount; i++) {
                String prefix = "p" + i + "_";
                d.platforms[i] = new PlatformInfo(
                    prefs.getString(prefix + "name", ""),
                    prefs.getString(prefix + "type", "glm"),
                    parseDoubleSafe(prefs.getString(prefix + "token", "-1.0")),
                    parseDoubleSafe(prefs.getString(prefix + "mcp", "-1.0")),
                    prefs.getString(prefix + "time", ""),
                    prefs.getString(prefix + "balance", ""),
                    prefs.getString(prefix + "granted", "")
                );
            }
        }
        return d;
    }

    public static void setCurrentPlatform(Context context, int index) {
        init(context);
        synchronized (lock) {
            prefs.edit().putString("currentPlatform", String.valueOf(index)).apply();
        }
    }

    private static double parseDoubleSafe(String s) {
        if (s == null || s.isEmpty()) return -1.0;
        try { return Double.parseDouble(s); }
        catch (NumberFormatException e) { return -1.0; }
    }

    private static int parseIntSafe(String s) {
        if (s == null || s.isEmpty()) return 0;
        try { return Integer.parseInt(s); }
        catch (NumberFormatException e) { return 0; }
    }
}
