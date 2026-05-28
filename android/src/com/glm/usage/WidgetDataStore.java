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
                            .getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
                }
            }
        }
    }

    public static void forceReload(Context context) {
        init(context);
    }

    public static class PlatformInfo {
        public String name;
        public double tokenPct;
        public double mcpPct;
        public String resetTime;
        public PlatformInfo(String n, double t, double m, String r) {
            name = n != null ? n : "";
            tokenPct = t;
            mcpPct = m;
            resetTime = r != null ? r : "";
        }
    }

    public static class WidgetData {
        public int widgetShowToken, widgetShowMcp, widgetShowTime, widgetFontSize;
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
            d.widgetFontSize = parseIntSafe(prefs.getString("widgetFontSize", "14"));
            d.platformCount = parseIntSafe(prefs.getString("platformCount", "0"));
            d.currentPlatform = parseIntSafe(prefs.getString("currentPlatform", "0"));

            d.platforms = new PlatformInfo[d.platformCount];
            for (int i = 0; i < d.platformCount; i++) {
                String prefix = "p" + i + "_";
                d.platforms[i] = new PlatformInfo(
                    prefs.getString(prefix + "name", ""),
                    parseDoubleSafe(prefs.getString(prefix + "token", "-1.0")),
                    parseDoubleSafe(prefs.getString(prefix + "mcp", "-1.0")),
                    prefs.getString(prefix + "time", "")
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
