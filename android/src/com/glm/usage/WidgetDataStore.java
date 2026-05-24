package com.glm.usage;

import android.content.Context;
import android.content.SharedPreferences;

public class WidgetDataStore {
    private static final String PREFS_NAME = "glm_usage_prefs";
    private static SharedPreferences prefs;

    public static void init(Context context) {
        prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
    }

    public static void forceReload(Context context) {
        prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
    }

    public static class PlatformInfo {
        public String name;
        public double tokenPct;
        public double mcpPct;
        public String resetTime;
        public PlatformInfo(String n, double t, double m, String r) {
            name = n != null ? n : ""; tokenPct = t; mcpPct = m; resetTime = r != null ? r : "";
        }
    }

    public static class WidgetData {
        public int widgetShowToken, widgetShowMcp, widgetShowTime, widgetFontSize;
        public int platformCount, currentPlatform;
        public PlatformInfo[] platforms;

        public WidgetData() { platforms = new PlatformInfo[0]; }
    }

    public static WidgetData getData(Context context, boolean forceRefresh) {
        if (prefs == null) init(context);
        forceReload(context);

        WidgetData d = new WidgetData();
        d.widgetShowToken = parseInt(prefs.getString("widgetShowToken", "1"));
        d.widgetShowMcp = parseInt(prefs.getString("widgetShowMcp", "1"));
        d.widgetShowTime = parseInt(prefs.getString("widgetShowTime", "1"));
        d.widgetFontSize = parseInt(prefs.getString("widgetFontSize", "14"));
        d.platformCount = parseInt(prefs.getString("platformCount", "0"));
        d.currentPlatform = parseInt(prefs.getString("currentPlatform", "0"));

        d.platforms = new PlatformInfo[d.platformCount];
        for (int i = 0; i < d.platformCount; i++) {
            String prefix = "p" + i + "_";
            d.platforms[i] = new PlatformInfo(
                prefs.getString(prefix + "name", ""),
                parseDouble(prefs.getString(prefix + "token", "-1.0")),
                parseDouble(prefs.getString(prefix + "mcp", "-1.0")),
                prefs.getString(prefix + "time", "")
            );
        }
        return d;
    }

    public static void setCurrentPlatform(Context context, int index) {
        if (prefs == null) init(context);
        prefs.edit().putString("currentPlatform", String.valueOf(index)).apply();
    }

    private static double parseDouble(String s) {
        try { return Double.parseDouble(s); } catch (NumberFormatException e) { return -1.0; }
    }
    private static int parseInt(String s) {
        try { return Integer.parseInt(s); } catch (NumberFormatException e) { return 1; }
    }
}
