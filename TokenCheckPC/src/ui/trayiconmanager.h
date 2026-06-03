#ifndef TRAYICONMANAGER_H
#define TRAYICONMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSet>

class FloatingBall;
class MainWindow;
class UsageQuery;
class DataManager;

class TrayIconManager : public QObject
{
    Q_OBJECT

public:
    explicit TrayIconManager(FloatingBall *ball, MainWindow *window,
                             UsageQuery *query, QObject *parent = nullptr);

    void show();
    void updateTooltip();
    void rebuildMenu();

public slots:
    void showSettingsTab();
    void checkQuotaNotifications();

signals:
    void quitRequested();
    void settingsChanged();

private:
    void onQueryNow();
    void onShowDetail();

    QSystemTrayIcon *m_tray;
    QMenu *m_menu;
    FloatingBall *m_ball;
    MainWindow *m_window;
    UsageQuery *m_query;
    DataManager *m_dm;
    QSet<QString> m_notifiedQuotas;
};

#endif
