#include "datamanager.h"

DataManager &DataManager::instance()
{
    static DataManager s;
    return s;
}

DataManager::DataManager(QObject *parent) : QObject(parent) {}

void DataManager::setUsageData(const UsageData &data)
{
    m_data[data.platformName] = data;
    if (!m_accountOrder.contains(data.platformName))
        m_accountOrder.append(data.platformName);
    if (m_currentAccount.isEmpty())
        m_currentAccount = data.platformName;
    m_lastUpdate = QDateTime::currentDateTime();
    emit dataUpdated(data.platformName);
}

void DataManager::setAllData(const QMap<QString, UsageData> &allData)
{
    m_data = allData;
    m_accountOrder = allData.keys();
    if (!m_accountOrder.contains(m_currentAccount)) {
        m_currentAccount = m_accountOrder.isEmpty() ? QString() : m_accountOrder.first();
    }
    m_lastUpdate = QDateTime::currentDateTime();
    emit allDataUpdated();
}

UsageData DataManager::data(const QString &name) const
{
    return m_data.value(name);
}

QMap<QString, UsageData> DataManager::allData() const
{
    return m_data;
}

QStringList DataManager::accountNames() const
{
    return m_accountOrder;
}

QString DataManager::currentAccount() const
{
    return m_currentAccount;
}

void DataManager::setCurrentAccount(const QString &name)
{
    if (m_currentAccount == name)
        return;
    m_currentAccount = name;
    emit accountChanged(name);
}

void DataManager::clearAll()
{
    m_data.clear();
    m_accountOrder.clear();
    m_currentAccount.clear();
}

QDateTime DataManager::lastUpdateTime() const
{
    return m_lastUpdate;
}
