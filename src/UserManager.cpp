#include "UserManager.h"
#include "DatabaseHelper.h"
#include "Persistance.h"
#include "QueryId.h"
#include "ReportGenerator.h"

#define KEY_SETTING_USER_PROFILE "userProfile"
#define KEY_USER_NAME "name"
#define KEY_USER_KUNYA "kunya"
#define KEY_USER_FEMALE "female"

namespace ilmtest {

using namespace canadainc;

UserProfile::UserProfile() :
        female(false), points(0)
{
}

UserManager::UserManager(Persistance* persist, DatabaseHelper* db) :
        m_persist(persist), m_db(db)
{
    persist->registerForSetting(this, KEY_SETTING_USER_PROFILE);
}


void UserManager::onDataLoaded(QVariant id, QVariant data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
}


void UserManager::onSettingChanged(QVariant newValue, QVariant key)
{
    if ( key.toString() == KEY_SETTING_USER_PROFILE )
    {
        QVariantMap profile = newValue.toMap();
        m_profile.name = profile.value(KEY_USER_NAME).toString();
        m_profile.kunya = profile.value(KEY_USER_KUNYA).toString();
        m_profile.female = profile.value(KEY_USER_FEMALE).toInt() == 1;

        emit profileChanged();
    }
}


QString UserManager::name() const {
    return m_profile.name;
}


QString UserManager::kunya() const {
    return m_profile.kunya;
}


bool UserManager::female() const {
    return m_profile.female;
}


bool UserManager::profileSet() const {
    return m_persist->contains(KEY_SETTING_USER_PROFILE);
}


int UserManager::points() const {
    return m_profile.points;
}


void UserManager::setPoints(int points)
{
    if (points != m_profile.points)
    {
        m_profile.points = points;
        emit pointsChanged();
    }
}


void UserManager::saveProfile(QString const& name, QString const& kunya, bool female)
{
    QVariantMap qvm;
    qvm[KEY_USER_FEMALE] = female ? 1 : 0;
    qvm[KEY_USER_KUNYA] = kunya;
    qvm[KEY_USER_NAME] = name;

    m_persist->saveValueFor(KEY_SETTING_USER_PROFILE, qvm);
}


void UserManager::lazyInit()
{
    setPoints( m_persist->getValueFor("points").toInt() );

    m_db->startTransaction(this, QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS scores (id INTEGER PRIMARY KEY, name TEXT NOT NULL, kunya TEXT, female INTEGER, points INTEGER, level INTEGER, UNIQUE(name,kunya,female) ON CONFLICT IGNORE)"), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS chosen_answers (id INTEGER PRIMARY KEY, presented INTEGER NOT NULL DEFAULT 0, chosen INTEGER NOT NULL DEFAULT 0)"), QueryId::Setup);
    m_db->endTransaction(this, QueryId::Setup);
}


UserManager::~UserManager()
{
}

} /* namespace canadainc */
