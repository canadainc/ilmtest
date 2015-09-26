#include "UserManager.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "Persistance.h"
#include "QueryId.h"

#define KEY_MAX_LEVEL "highest_level"
#define KEY_POINTS "points"
#define KEY_SETTING_USER_PROFILE "userProfile"
#define KEY_USER_NAME "name"
#define KEY_USER_KUNYA "kunya"
#define KEY_USER_FEMALE "female"
#define SET_KEY_VALUE_ID if (id) keyValues["id"] = id;
#define SET_AND_RETURN SET_KEY_VALUE_ID; return keyValues
#define TABLE_NAME "user_profiles"

namespace {

QVariantMap getProfileTokens(QString const& name, QString const& kunya, bool female)
{
    QVariantMap keyValues;
    keyValues[KEY_USER_NAME] = name;
    keyValues[KEY_USER_KUNYA] = kunya;
    keyValues[KEY_USER_FEMALE] = female;

    return keyValues;
}

}

namespace ilmtest {

using namespace canadainc;

UserProfile::UserProfile() :
        id(0), female(false), points(0), maxLevel(0)
{
}

UserManager::UserManager(Persistance* persist, DatabaseHelper* db) :
        m_persist(persist), m_db(db)
{
}


void UserManager::onDataLoaded(QVariant id, QVariant data)
{
    if ( id.toInt() == QueryId::FetchProfile )
    {
        QVariantList profiles = data.toList();

        if ( !profiles.isEmpty() )
        {
            QVariantMap profile = profiles.first().toMap();
            m_profile.id = profile.value(KEY_ID_FIELD).toLongLong();
            m_profile.name = profile.value(KEY_USER_NAME).toString();
            m_profile.kunya = profile.value(KEY_USER_KUNYA).toString();
            m_profile.female = profile.value(KEY_USER_FEMALE).toInt() == 1;
            m_profile.points = profile.value(KEY_POINTS).toInt();
            m_profile.maxLevel = profile.value(KEY_MAX_LEVEL).toInt();

            emit profileChanged();
            emit pointsChanged();
        }
    }
}


void UserManager::onSettingChanged(QVariant newValue, QVariant key)
{
    if ( key.toString() == KEY_SETTING_USER_PROFILE )
    {
        if ( profileSet() )
        {
            qint64 userId = newValue.toLongLong();
            fetchProfile(this, userId);
        }
    }
}


void UserManager::fetchProfile(QObject* caller, qint64 userId)
{
    LOGGER(userId);
    m_db->executeQuery( caller, "SELECT * FROM user_profiles WHERE id=?", QueryId::FetchProfile, QVariantList() << userId );
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
    return m_persist->containsFlag(KEY_SETTING_USER_PROFILE);
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


int UserManager::highestLevel() const {
    return m_profile.maxLevel;
}


void UserManager::setHighestLevel(int highestLevel)
{
    if (highestLevel > m_profile.maxLevel) {
        m_profile.maxLevel = highestLevel;
    }
}


QVariantMap UserManager::createProfile(QString const& name, QString const& kunya, bool female)
{
    LOGGER(name << kunya << female);
    QVariantMap keyValues = getProfileTokens(name, kunya, female);

    qint64 id = m_db->executeInsert(TABLE_NAME, keyValues);
    SET_AND_RETURN;
}


void UserManager::changeProfile(qint64 id)
{
    commitChanges();
    m_persist->setFlag(KEY_SETTING_USER_PROFILE, id);
}


void UserManager::commitChanges()
{
    QVariantMap keyValues;
    keyValues[KEY_POINTS] = m_profile.points;
    keyValues[KEY_MAX_LEVEL] = m_profile.maxLevel;

    m_db->executeUpdate(this, TABLE_NAME, keyValues, QueryId::EditProfile, m_profile.id);
}


QVariantMap UserManager::editProfile(QObject* caller, qint64 id, QString const& name, QString const& kunya, bool female)
{
    LOGGER(id << name << kunya << female);

    QVariantMap keyValues = getProfileTokens(name, kunya, female);
    m_db->executeUpdate(caller, TABLE_NAME, keyValues, QueryId::EditProfile, id);

    SET_AND_RETURN;
}


void UserManager::fetchAllProfiles(QObject* caller) {
    m_db->executeQuery( caller, "SELECT * FROM user_profiles", QueryId::FetchAllProfiles);
}


qint64 UserManager::id() const {
    return m_profile.id;
}


void UserManager::lazyInit()
{
    m_db->startTransaction(this, QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS user_profiles (id INTEGER PRIMARY KEY, name TEXT NOT NULL, kunya TEXT, female INTEGER, points INTEGER DEFAULT 0, highest_level INTEGER DEFAULT 0, played INTEGER DEFAULT 0, UNIQUE(name,kunya,female) ON CONFLICT IGNORE)"), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS chosen_answers (id INTEGER PRIMARY KEY, presented INTEGER NOT NULL DEFAULT 0, chosen INTEGER NOT NULL DEFAULT 0)"), QueryId::Setup);
    m_db->endTransaction(this, QueryId::Setup);

    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commitChanges() ) );
    m_persist->registerForSetting(this, KEY_SETTING_USER_PROFILE, true);
}


UserManager::~UserManager()
{
}

} /* namespace canadainc */
