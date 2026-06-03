#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QDateTime>
#include "usagequery.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    static DataManager &instance();

    void setUsageData(const UsageData &data);
    void setAllData(const QMap<QString, UsageData> &allData);
    UsageData data(const QString &name) const;
    QMap<QString, UsageData> allData() const;
    QStringList accountNames() const;

    QString currentAccount() const;
    void setCurrentAccount(const QString &name);

    void clearAll();
    QDateTime lastUpdateTime() const;

signals:
    void dataUpdated(const QString &accountName);
    void allDataUpdated();
    void accountChanged(const QString &name);

private:
    explicit DataManager(QObject *parent = nullptr);

    QMap<QString, UsageData> m_data;
    QStringList m_accountOrder;
    QString m_currentAccount;
    QDateTime m_lastUpdate;
};

#endif
