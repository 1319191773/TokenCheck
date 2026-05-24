package com.glm.usage;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.widget.RemoteViews;

import javax.net.ssl.HttpsURLConnection;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import org.json.JSONArray;
import org.json.JSONObject;

public class GlmUsageWidget extends AppWidgetProvider {

    public static final String WIDGET_REFRESH = "com.glm.usage.WIDGET_REFRESH";
    public static final String WIDGET_SWITCH = "com.glm.usage.WIDGET_SWITCH";
    private static final int[] PLATFORM_IDS = {R.id.platformName0, R.id.platformName1, R.id.platformName2};

    @Override
    public void onUpdate(Context context, AppWidgetManager mgr, int[] ids) {
        for (int id : ids) updateAppWidget(context, mgr, id);
        scheduleAutoRefresh(context);
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        super.onReceive(context, intent);
        AppWidgetManager mgr = AppWidgetManager.getInstance(context);
        ComponentName cn = new ComponentName(context, GlmUsageWidget.class);
        int[] ids = mgr.getAppWidgetIds(cn);

        String action = intent.getAction();

        if ("com.glm.usage.WIDGET_UPDATE".equals(action)) {
            WidgetDataStore.forceReload(context);
            onUpdate(context, mgr, ids);
        } else if (WIDGET_REFRESH.equals(action)) {
            for (int id : ids) {
                RemoteViews v = new RemoteViews(context.getPackageName(), R.layout.tokencheck_widget);
                v.setTextViewText(R.id.tokenPctText, "..."); v.setTextViewText(R.id.mcpPctText, "...");
                v.setTextViewText(R.id.resetTimeText, "..."); v.setTextViewText(R.id.refreshBtn, "\u21BB");
                mgr.partiallyUpdateAppWidget(id, v);
            }
            new Thread(() -> doApiQuery(context)).start();
        } else if (WIDGET_SWITCH.equals(action)) {
            int idx = intent.getIntExtra("platformIndex", 0);
            WidgetDataStore.setCurrentPlatform(context, idx);
            WidgetDataStore.forceReload(context);
            onUpdate(context, mgr, ids);
        }
    }

    @Override public void onEnabled(Context c) { WidgetDataStore.init(c); scheduleAutoRefresh(c); }
    @Override public void onDisabled(Context c) { cancelAutoRefresh(c); }

    private void doApiQuery(Context context) {
        WidgetDataStore.WidgetData data = WidgetDataStore.getData(context, false);
        int cp = data.currentPlatform;
        if (cp < 0 || cp >= data.platforms.length) return;

        String name = data.platforms[cp].name;
        String configsJson = context.getSharedPreferences("glm_usage_prefs", Context.MODE_PRIVATE)
                .getString("platformConfigs", "[]");

        double tPct = -1, mPct = -1;
        String tokenReset = "", mcpDetail = "";

        try {
            JSONArray platforms = new JSONArray(configsJson);
            for (int i = 0; i < platforms.length(); i++) {
                JSONObject cfg = platforms.getJSONObject(i);
                if (!name.equals(cfg.optString("name", ""))) continue;

                String base = cfg.optString("baseUrl", "https://open.bigmodel.cn");
                String token = cfg.optString("authToken", "");
                String prefix = cfg.optString("apiPrefix", "/api/monitor/usage");

                QuotaResult r = fetchQuota(base, token, prefix);
                tPct = r.tokenPct; mPct = r.mcpPct;
                tokenReset = r.tokenReset; mcpDetail = r.mcpDetail;
                break;
            }
        } catch (Exception e) { e.printStackTrace(); return; }

        SharedPreferences prefs = context.getSharedPreferences("glm_usage_prefs", Context.MODE_PRIVATE);
        String prefix = "p" + cp + "_";
        prefs.edit()
                .putString(prefix + "token", String.valueOf(tPct))
                .putString(prefix + "mcp", String.valueOf(mPct))
                .putString(prefix + "time", tokenReset)
                .apply();

        new Handler(Looper.getMainLooper()).post(() -> {
            AppWidgetManager mgr = AppWidgetManager.getInstance(context);
            WidgetDataStore.forceReload(context);
            int[] ids = mgr.getAppWidgetIds(new ComponentName(context, GlmUsageWidget.class));
            for (int id : ids) updateAppWidget(context, mgr, id);
        });
    }

    static class QuotaResult { double tokenPct = -1, mcpPct = -1; String tokenReset = "", mcpDetail = ""; }

    private QuotaResult fetchQuota(String base, String token, String prefix) {
        QuotaResult r = new QuotaResult();
        try {
            URL url = new URL(base + prefix + "/quota/limit");
            HttpsURLConnection conn = (HttpsURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.setRequestProperty("Authorization", token);
            conn.setRequestProperty("Accept-Language", "en-US,en");
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setConnectTimeout(10000); conn.setReadTimeout(10000);

            BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            StringBuilder sb = new StringBuilder(); String line;
            while ((line = br.readLine()) != null) sb.append(line);
            br.close(); conn.disconnect();

            JSONArray limits = new JSONObject(sb.toString()).getJSONObject("data").getJSONArray("limits");
            SimpleDateFormat fmt = new SimpleDateFormat("HH:mm", Locale.getDefault());

            for (int i = 0; i < limits.length(); i++) {
                JSONObject o = limits.getJSONObject(i);
                String type = o.optString("type", "").toLowerCase();
                double pct = o.optDouble("percentage", -1);
                long resetMs = o.optLong("nextResetTime", 0);
                String resetStr = resetMs > 0 ? fmt.format(new Date(resetMs)) : "";

                if (type.contains("token")) { r.tokenPct = pct; if (!resetStr.isEmpty()) r.tokenReset = resetStr; }
                else if (type.contains("mcp") || type.contains("time")) {
                    r.mcpPct = pct;
                    JSONArray arr = o.optJSONArray("usageDetails");
                    if (arr != null && arr.length() > 0) {
                        StringBuilder det = new StringBuilder();
                        for (int j = 0; j < arr.length(); j++) {
                            JSONObject ud = arr.getJSONObject(j);
                            if (det.length() > 0) det.append(" ");
                            det.append(ud.optString("label", ud.optString("displayName", "")));
                        }
                        r.mcpDetail = det.toString();
                    }
                }
            }
        } catch (Exception e) { e.printStackTrace(); }
        return r;
    }

    static void updateAppWidget(Context context, AppWidgetManager mgr, int id) {
        WidgetDataStore.WidgetData data = WidgetDataStore.getData(context, false);
        RemoteViews views = new RemoteViews(context.getPackageName(), R.layout.tokencheck_widget);

        boolean st = data.widgetShowToken != 0, sm = data.widgetShowMcp != 0, stm = data.widgetShowTime != 0;
        views.setViewVisibility(R.id.tokenArea, st ? android.view.View.VISIBLE : android.view.View.GONE);
        views.setViewVisibility(R.id.mcpArea, sm ? android.view.View.VISIBLE : android.view.View.GONE);
        views.setViewVisibility(R.id.timeArea, stm ? android.view.View.VISIBLE : android.view.View.GONE);

        int cp = data.currentPlatform;
        int total = data.platforms.length;
        if (cp < 0 || cp >= total) cp = 0;

        WidgetDataStore.PlatformInfo pi = (total > 0)
                ? data.platforms[cp] : new WidgetDataStore.PlatformInfo("", -1, -1, "");

        if (st) views.setTextViewText(R.id.tokenPctText, formatPercent(pi.tokenPct));
        if (sm) views.setTextViewText(R.id.mcpPctText, formatPercent(pi.mcpPct));
        if (stm) views.setTextViewText(R.id.resetTimeText, pi.resetTime.isEmpty() ? "--:--" : pi.resetTime);

        // Platform carousel: show up to 3 names with current highlighted in center
        if (total > 1) {
            views.setViewVisibility(R.id.btnPrev, android.view.View.VISIBLE);
            views.setViewVisibility(R.id.btnNext, android.view.View.VISIBLE);
        } else {
            views.setViewVisibility(R.id.btnPrev, android.view.View.GONE);
            views.setViewVisibility(R.id.btnNext, android.view.View.GONE);
        }

        // Fill 3 slots around current platform
        int[] indices = new int[]{cp - 1, cp, cp + 1};
        for (int i = 0; i < 3; i++) {
            int idx = indices[i];
            if (idx < 0) idx += total;
            if (idx >= total) idx -= total;
            boolean valid = (total > 0 && ((i == 1) || total >= 3 || (i == 0 && cp == 1) || (i == 2 && cp == 0)));

            if (valid && total > 0 && idx < total) {
                views.setViewVisibility(PLATFORM_IDS[i], android.view.View.VISIBLE);
                views.setTextViewText(PLATFORM_IDS[i], data.platforms[idx].name);
                views.setTextColor(PLATFORM_IDS[i], i == 1 ? 0xFFFFFFFF : 0xFF777777);

                if (i != 1) {
                    Intent swIntent = new Intent(context, GlmUsageWidget.class);
                    swIntent.setAction(WIDGET_SWITCH);
                    swIntent.putExtra("platformIndex", idx);
                    views.setOnClickPendingIntent(PLATFORM_IDS[i], PendingIntent.getBroadcast(
                            context, 200 + i, swIntent,
                            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));
                }
            } else {
                views.setViewVisibility(PLATFORM_IDS[i], android.view.View.GONE);
            }
        }

        // Prev
        Intent prevIntent = new Intent(context, GlmUsageWidget.class);
        prevIntent.setAction(WIDGET_SWITCH);
        prevIntent.putExtra("platformIndex", cp > 0 ? cp - 1 : total - 1);
        views.setOnClickPendingIntent(R.id.btnPrev, PendingIntent.getBroadcast(context, 101, prevIntent,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

        // Next
        Intent nextIntent = new Intent(context, GlmUsageWidget.class);
        nextIntent.setAction(WIDGET_SWITCH);
        nextIntent.putExtra("platformIndex", cp < total - 1 ? cp + 1 : 0);
        views.setOnClickPendingIntent(R.id.btnNext, PendingIntent.getBroadcast(context, 102, nextIntent,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

        // R button → refresh
        Intent ref = new Intent(context, GlmUsageWidget.class); ref.setAction(WIDGET_REFRESH);
        views.setOnClickPendingIntent(R.id.refreshBtn, PendingIntent.getBroadcast(context, 1, ref,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

        // Data areas → open app
        Intent launch = context.getPackageManager().getLaunchIntentForPackage(context.getPackageName());
        if (launch != null) {
            PendingIntent launchPi = PendingIntent.getActivity(context, 0, launch,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
            views.setOnClickPendingIntent(R.id.tokenArea, launchPi);
            views.setOnClickPendingIntent(R.id.mcpArea, launchPi);
            views.setOnClickPendingIntent(R.id.timeArea, launchPi);
        }

        mgr.updateAppWidget(id, views);
    }

    private static String formatPercent(double p) { return p < 0 ? "--%" : String.format("%.1f%%", p); }

    private void scheduleAutoRefresh(Context c) {
        AlarmManager am = (AlarmManager) c.getSystemService(Context.ALARM_SERVICE);
        if (am == null) return;
        Intent i = new Intent(c, GlmUsageWidget.class);
        i.setAction(AppWidgetManager.ACTION_APPWIDGET_UPDATE);
        PendingIntent pi = PendingIntent.getBroadcast(c, 0, i, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
        am.cancel(pi);
        am.setInexactRepeating(AlarmManager.ELAPSED_REALTIME, SystemClock.elapsedRealtime() + 60000, 15 * 60 * 1000, pi);
    }
    private void cancelAutoRefresh(Context c) {
        AlarmManager am = (AlarmManager) c.getSystemService(Context.ALARM_SERVICE);
        if (am == null) return;
        Intent i = new Intent(c, GlmUsageWidget.class);
        i.setAction(AppWidgetManager.ACTION_APPWIDGET_UPDATE);
        PendingIntent pi = PendingIntent.getBroadcast(c, 0, i, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
        am.cancel(pi);
    }
}
