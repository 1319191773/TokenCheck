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
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import org.json.JSONArray;
import org.json.JSONObject;

public class GlmUsageWidget extends AppWidgetProvider {

    public static final String WIDGET_REFRESH = "com.glm.usage.WIDGET_REFRESH";
    public static final String WIDGET_SWITCH = "com.glm.usage.WIDGET_SWITCH";
    private static final int[] PLATFORM_IDS = {R.id.platformName0, R.id.platformName1, R.id.platformName2};
    private static final int MAX_RETRY = 3;
    private final ExecutorService executor = Executors.newSingleThreadExecutor();

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
                RemoteViews v = new RemoteViews(context.getPackageName(), R.layout.glm_widget);
                v.setTextViewText(R.id.tokenPctText, "...");
                v.setTextViewText(R.id.mcpPctText, "...");
                v.setTextViewText(R.id.resetTimeText, "...");
                mgr.partiallyUpdateAppWidget(id, v);
            }
            executor.execute(() -> doApiQuery(context));
        } else if (WIDGET_SWITCH.equals(action)) {
            int idx = intent.getIntExtra("platformIndex", 0);
            WidgetDataStore.setCurrentPlatform(context, idx);
            WidgetDataStore.forceReload(context);
            onUpdate(context, mgr, ids);
        }
    }

    @Override
    public void onEnabled(Context c) {
        WidgetDataStore.init(c);
        scheduleAutoRefresh(c);
    }

    @Override
    public void onDisabled(Context c) {
        cancelAutoRefresh(c);
        executor.shutdownNow();
    }

    private void doApiQuery(Context context) {
        SharedPreferences prefs = context.getSharedPreferences("glm_usage_prefs", Context.MODE_MULTI_PROCESS);
        WidgetDataStore.forceReload(context);
        WidgetDataStore.WidgetData data = WidgetDataStore.getData(context, false);
        int cp = data.currentPlatform;

        String configsJson = prefs.getString("platformConfigs", "[]");
        JSONArray allConfigs = null;
        try { allConfigs = new JSONArray(configsJson); } catch (Exception e) { allConfigs = null; }

        if (allConfigs == null || allConfigs.length() == 0) {
            restoreLastData(context);
            return;
        }

        int targetIdx = -1;
        if (cp >= 0 && cp < data.platforms.length && !data.platforms[cp].name.isEmpty()) {
            String name = data.platforms[cp].name;
            for (int i = 0; i < allConfigs.length(); i++) {
                try {
                    if (name.equals(allConfigs.getJSONObject(i).optString("name", ""))) {
                        targetIdx = i;
                        break;
                    }
                } catch (Exception e) { /* skip */ }
            }
        }
        if (targetIdx < 0) {
            for (int i = 0; i < allConfigs.length(); i++) {
                try {
                    String token = allConfigs.getJSONObject(i).optString("authToken", "");
                    if (!token.isEmpty()) { targetIdx = i; break; }
                } catch (Exception e) { /* skip */ }
            }
        }
        if (targetIdx < 0) {
            restoreLastData(context);
            return;
        }

        String base = "https://open.bigmodel.cn";
        String authToken = "";
        String apiPrefix = "/api/monitor/usage";
        try {
            JSONObject cfg = allConfigs.getJSONObject(targetIdx);
            base = cfg.optString("baseUrl", base);
            authToken = cfg.optString("authToken", "");
            apiPrefix = cfg.optString("apiPrefix", apiPrefix);
        } catch (Exception e) { /* skip */ }

        if (authToken.isEmpty()) {
            restoreLastData(context);
            return;
        }

        boolean isDeepSeek = base.toLowerCase().contains("deepseek");
        String tokenVal = "-1.0";
        String mcpVal = "-1.0";
        String timeVal = "";
        String balanceVal = "";
        String grantedVal = "";

        if (isDeepSeek) {
            BalanceResult br = fetchBalance(base, authToken);
            if (br.success) {
                tokenVal = br.primaryBalance;
                balanceVal = br.primaryBalance;
                grantedVal = br.grantedBalance;
            } else {
                restoreLastData(context);
                return;
            }
        } else {
            QuotaResult r = fetchQuotaWithRetry(base, authToken, apiPrefix);
            if (r.tokenPct < 0 && r.mcpPct < 0) {
                restoreLastData(context);
                return;
            }
            tokenVal = String.valueOf(r.tokenPct);
            mcpVal = String.valueOf(r.mcpPct);
            timeVal = r.tokenReset;
        }

        int writeIdx = (targetIdx < allConfigs.length()) ? targetIdx : 0;
        String prefix = "p" + writeIdx + "_";
        SharedPreferences.Editor editor = prefs.edit();
        try { editor.putString(prefix + "name", allConfigs.getJSONObject(writeIdx).optString("name", "")); } catch (Exception e) { /* skip */ }
        editor.putString(prefix + "type", isDeepSeek ? "deepseek" : "glm");
        editor.putString(prefix + "token", tokenVal);
        editor.putString(prefix + "mcp", mcpVal);
        editor.putString(prefix + "time", timeVal);
        editor.putString(prefix + "balance", balanceVal);
        editor.putString(prefix + "granted", grantedVal);
        editor.putString("platformCount", String.valueOf(allConfigs.length()));
        for (int p = 0; p < allConfigs.length(); p++) {
            try {
                String pName = allConfigs.getJSONObject(p).optString("name", "");
                if (pName.isEmpty()) continue;
                String pPrefix = "p" + p + "_";
                String existingName = prefs.getString(pPrefix + "name", "");
                if (existingName.isEmpty()) {
                    editor.putString(pPrefix + "name", pName);
                }
            } catch (Exception e) { /* skip */ }
        }
        editor.apply();

        new Handler(Looper.getMainLooper()).post(() -> {
            AppWidgetManager mgr = AppWidgetManager.getInstance(context);
            WidgetDataStore.forceReload(context);
            int[] ids = mgr.getAppWidgetIds(new ComponentName(context, GlmUsageWidget.class));
            for (int id : ids) updateAppWidget(context, mgr, id);
        });
    }

    private void restoreLastData(Context context) {
        new Handler(Looper.getMainLooper()).post(() -> {
            AppWidgetManager mgr = AppWidgetManager.getInstance(context);
            WidgetDataStore.forceReload(context);
            int[] ids = mgr.getAppWidgetIds(new ComponentName(context, GlmUsageWidget.class));
            for (int id : ids) updateAppWidget(context, mgr, id);
        });
    }

    static class QuotaResult {
        double tokenPct = -1, mcpPct = -1;
        String tokenReset = "";
    }

    private QuotaResult fetchQuotaWithRetry(String base, String token, String prefix) {
        QuotaResult r = new QuotaResult();
        if (token == null || token.isEmpty()) return r;

        for (int attempt = 0; attempt < MAX_RETRY; attempt++) {
            if (attempt > 0) {
                try { Thread.sleep(1000 * attempt); } catch (InterruptedException ie) { break; }
            }
            QuotaResult attemptResult = fetchQuota(base, token, prefix);
            if (attemptResult.tokenPct >= 0 || attemptResult.mcpPct >= 0) {
                return attemptResult;
            }
        }
        return r;
    }

    private QuotaResult fetchQuota(String base, String token, String prefix) {
        QuotaResult r = new QuotaResult();
        HttpsURLConnection conn = null;
        try {
            URL url = new URL(base + prefix + "/quota/limit");
            conn = (HttpsURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.setRequestProperty("Authorization", token);
            conn.setRequestProperty("Accept-Language", "en-US,en");
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setConnectTimeout(10000);
            conn.setReadTimeout(10000);

            int responseCode = conn.getResponseCode();
            if (responseCode != 200) return r;

            BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) sb.append(line);
            br.close();

            JSONObject root = new JSONObject(sb.toString());
            if (!root.has("data")) return r;

            JSONArray limits = root.getJSONObject("data").getJSONArray("limits");
            SimpleDateFormat fmt = new SimpleDateFormat("HH:mm", Locale.getDefault());

            for (int i = 0; i < limits.length(); i++) {
                JSONObject o = limits.getJSONObject(i);
                String type = o.optString("type", "").toLowerCase();
                double pct = o.optDouble("percentage", -1);
                long resetMs = o.optLong("nextResetTime", 0);
                String resetStr = resetMs > 0 ? fmt.format(new Date(resetMs)) : "";

                if (type.contains("token")) {
                    r.tokenPct = pct;
                    if (!resetStr.isEmpty()) r.tokenReset = resetStr;
                } else if (type.contains("mcp") || type.contains("time")) {
                    r.mcpPct = pct;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (conn != null) conn.disconnect();
        }
        return r;
    }

    static class BalanceResult {
        boolean success = false;
        String primaryBalance = "";
        String grantedBalance = "";
    }

    private BalanceResult fetchBalance(String base, String apiKey) {
        BalanceResult r = new BalanceResult();
        HttpsURLConnection conn = null;
        try {
            URL url = new URL(base + "/user/balance");
            conn = (HttpsURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.setRequestProperty("Authorization", "Bearer " + apiKey);
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setConnectTimeout(10000);
            conn.setReadTimeout(10000);

            int responseCode = conn.getResponseCode();
            if (responseCode != 200) return r;

            BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) sb.append(line);
            br.close();

            JSONObject root = new JSONObject(sb.toString());
            if (!root.has("balance_infos")) return r;

            JSONArray infos = root.getJSONArray("balance_infos");
            for (int i = 0; i < infos.length(); i++) {
                JSONObject info = infos.getJSONObject(i);
                String currency = info.optString("currency", "");
                double total = info.optDouble("total_balance", 0);
                double granted = info.optDouble("granted_balance", 0);
                if (total > 0 && r.primaryBalance.isEmpty()) {
                    r.primaryBalance = currency + " " + String.format("%.2f", total);
                    r.grantedBalance = String.format("%.2f", granted);
                }
            }
            if (r.primaryBalance.isEmpty() && infos.length() > 0) {
                JSONObject first = infos.getJSONObject(0);
                String currency = first.optString("currency", "USD");
                double total = first.optDouble("total_balance", 0);
                double granted = first.optDouble("granted_balance", 0);
                r.primaryBalance = currency + " " + String.format("%.2f", total);
                r.grantedBalance = String.format("%.2f", granted);
            }
            r.success = true;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (conn != null) conn.disconnect();
        }
        return r;
    }

    static void updateAppWidget(Context context, AppWidgetManager mgr, int id) {
        WidgetDataStore.WidgetData data = WidgetDataStore.getData(context, false);
        RemoteViews views = new RemoteViews(context.getPackageName(), R.layout.glm_widget);

        int cp = data.currentPlatform;
        int total = data.platforms.length;
        if (cp < 0 || cp >= total) cp = 0;

        WidgetDataStore.PlatformInfo pi = (total > 0)
                ? data.platforms[cp]
                : new WidgetDataStore.PlatformInfo("", "glm", -1, -1, "", "", "");

        boolean isDeepSeek = "deepseek".equals(pi.type);

        if (isDeepSeek) {
            boolean showBal = data.widgetShowBalance != 0;
            boolean showGrant = data.widgetShowGranted != 0;
            views.setViewVisibility(R.id.tokenArea, showBal ? android.view.View.VISIBLE : android.view.View.GONE);
            views.setViewVisibility(R.id.mcpArea, showGrant ? android.view.View.VISIBLE : android.view.View.GONE);
            views.setViewVisibility(R.id.timeArea, android.view.View.GONE);
            if (showBal) {
                views.setTextViewText(R.id.tokenPctText, pi.balance.isEmpty() ? "--" : pi.balance);
                views.setTextViewText(R.id.tokenLabelText, context.getString(R.string.widget_label_balance));
            }
            if (showGrant) {
                views.setTextViewText(R.id.mcpPctText, pi.granted.isEmpty() ? "--" : pi.granted);
                views.setTextViewText(R.id.mcpLabelText, context.getString(R.string.widget_label_granted));
            }
        } else {
            boolean st = data.widgetShowToken != 0;
            boolean sm = data.widgetShowMcp != 0;
            boolean stm = data.widgetShowTime != 0;
            views.setViewVisibility(R.id.tokenArea, st ? android.view.View.VISIBLE : android.view.View.GONE);
            views.setViewVisibility(R.id.mcpArea, sm ? android.view.View.VISIBLE : android.view.View.GONE);
            views.setViewVisibility(R.id.timeArea, stm ? android.view.View.VISIBLE : android.view.View.GONE);
            if (st) {
                views.setTextViewText(R.id.tokenPctText, formatPercent(pi.tokenPct));
                views.setTextViewText(R.id.tokenLabelText, "Token");
            }
            if (sm) {
                views.setTextViewText(R.id.mcpPctText, formatPercent(pi.mcpPct));
                views.setTextViewText(R.id.mcpLabelText, "MCP");
            }
            if (stm) {
                views.setTextViewText(R.id.resetTimeText, pi.resetTime.isEmpty() ? "--:--" : pi.resetTime);
                views.setTextViewText(R.id.timeLabelText, context.getString(R.string.widget_label_reset));
            }
        }

        if (total == 2) {
            int other = (cp == 0) ? 1 : 0;
            views.setViewVisibility(R.id.btnPrev, cp == 1 ? android.view.View.VISIBLE : android.view.View.GONE);
            views.setViewVisibility(R.id.btnNext, cp == 0 ? android.view.View.VISIBLE : android.view.View.GONE);

            if (cp == 1) {
                views.setViewVisibility(R.id.platformName0, android.view.View.VISIBLE);
                views.setTextViewText(R.id.platformName0, data.platforms[other].name);
                views.setTextColor(R.id.platformName0, 0xFF777777);
                Intent swL = new Intent(context, GlmUsageWidget.class);
                swL.setAction(WIDGET_SWITCH);
                swL.putExtra("platformIndex", other);
                views.setOnClickPendingIntent(R.id.platformName0, PendingIntent.getBroadcast(
                        context, 200, swL, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));
            } else {
                views.setViewVisibility(R.id.platformName0, android.view.View.GONE);
            }

            views.setViewVisibility(R.id.platformName1, android.view.View.VISIBLE);
            views.setTextViewText(R.id.platformName1, data.platforms[cp].name);
            views.setTextColor(R.id.platformName1, 0xFFFFFFFF);

            if (cp == 0) {
                views.setViewVisibility(R.id.platformName2, android.view.View.VISIBLE);
                views.setTextViewText(R.id.platformName2, data.platforms[other].name);
                views.setTextColor(R.id.platformName2, 0xFF777777);
                Intent swR = new Intent(context, GlmUsageWidget.class);
                swR.setAction(WIDGET_SWITCH);
                swR.putExtra("platformIndex", other);
                views.setOnClickPendingIntent(R.id.platformName2, PendingIntent.getBroadcast(
                        context, 202, swR, PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));
            } else {
                views.setViewVisibility(R.id.platformName2, android.view.View.GONE);
            }

            Intent twoPrev = new Intent(context, GlmUsageWidget.class);
            twoPrev.setAction(WIDGET_SWITCH);
            twoPrev.putExtra("platformIndex", other);
            views.setOnClickPendingIntent(R.id.btnPrev, PendingIntent.getBroadcast(context, 101, twoPrev,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

            Intent twoNext = new Intent(context, GlmUsageWidget.class);
            twoNext.setAction(WIDGET_SWITCH);
            twoNext.putExtra("platformIndex", other);
            views.setOnClickPendingIntent(R.id.btnNext, PendingIntent.getBroadcast(context, 102, twoNext,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

        } else if (total > 2) {
            views.setViewVisibility(R.id.btnPrev, android.view.View.VISIBLE);
            views.setViewVisibility(R.id.btnNext, android.view.View.VISIBLE);

            int[] indices = new int[]{
                cp - 1 < 0 ? total - 1 : cp - 1,
                cp,
                cp + 1 >= total ? 0 : cp + 1
            };

            for (int i = 0; i < 3; i++) {
                int idx = indices[i];
                boolean show = (total > 0 && (i == 1 || total >= 2));

                if (show && idx < total) {
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

            Intent prevIntent = new Intent(context, GlmUsageWidget.class);
            prevIntent.setAction(WIDGET_SWITCH);
            prevIntent.putExtra("platformIndex", cp > 0 ? cp - 1 : total - 1);
            views.setOnClickPendingIntent(R.id.btnPrev, PendingIntent.getBroadcast(context, 101, prevIntent,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

            Intent nextIntent = new Intent(context, GlmUsageWidget.class);
            nextIntent.setAction(WIDGET_SWITCH);
            nextIntent.putExtra("platformIndex", cp < total - 1 ? cp + 1 : 0);
            views.setOnClickPendingIntent(R.id.btnNext, PendingIntent.getBroadcast(context, 102, nextIntent,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

        } else {
            views.setViewVisibility(R.id.btnPrev, android.view.View.GONE);
            views.setViewVisibility(R.id.btnNext, android.view.View.GONE);
            views.setViewVisibility(R.id.platformName0, android.view.View.GONE);
            views.setViewVisibility(R.id.platformName2, android.view.View.GONE);
            if (total == 1) {
                views.setViewVisibility(R.id.platformName1, android.view.View.VISIBLE);
                views.setTextViewText(R.id.platformName1, data.platforms[0].name);
                views.setTextColor(R.id.platformName1, 0xFFFFFFFF);
            } else {
                views.setViewVisibility(R.id.platformName1, android.view.View.GONE);
            }
        }

        Intent ref = new Intent(context, GlmUsageWidget.class);
        ref.setAction(WIDGET_REFRESH);
        views.setOnClickPendingIntent(R.id.refreshBtn, PendingIntent.getBroadcast(context, 1, ref,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));

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

    private static String formatPercent(double p) {
        return p < 0 ? "--%" : String.format("%.1f%%", p);
    }

    private void scheduleAutoRefresh(Context c) {
        AlarmManager am = (AlarmManager) c.getSystemService(Context.ALARM_SERVICE);
        if (am == null) return;
        int intervalMin = 15;
        SharedPreferences prefs = c.getSharedPreferences("glm_usage_prefs", Context.MODE_MULTI_PROCESS);
        try { intervalMin = Integer.parseInt(prefs.getString("widgetRefreshInterval", "15")); }
        catch (Exception e) { /* use default */ }
        if (intervalMin < 1) intervalMin = 15;
        Intent i = new Intent(c, GlmUsageWidget.class);
        i.setAction(WIDGET_REFRESH);
        PendingIntent pi = PendingIntent.getBroadcast(c, 0, i,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
        am.cancel(pi);
        am.setInexactRepeating(AlarmManager.ELAPSED_REALTIME,
                SystemClock.elapsedRealtime() + 60000, intervalMin * 60 * 1000, pi);
    }

    private void cancelAutoRefresh(Context c) {
        AlarmManager am = (AlarmManager) c.getSystemService(Context.ALARM_SERVICE);
        if (am == null) return;
        Intent i = new Intent(c, GlmUsageWidget.class);
        i.setAction(WIDGET_REFRESH);
        PendingIntent pi = PendingIntent.getBroadcast(c, 0, i,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
        am.cancel(pi);
    }
}
