#include "precompiled.h"

#include "UserManager.h"
#include "AnalyticHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Game.h"
#include "Lifeline.h"
#include "Logger.h"
#include "Persistance.h"
#include "QueryId.h"
#include "TextUtils.h"

#define KEY_MAX_LEVEL "highest_level"
#define KEY_NUM_PLAYED "played"
#define KEY_POINTS "points"
#define KEY_SETTING_USER_PROFILE "userProfile"
#define KEY_USER_NAME "name"
#define KEY_USER_KUNYA "kunya"
#define KEY_USER_FEMALE "female"
#define SET_KEY_VALUE_ID if (id) keyValues["id"] = id;
#define SET_AND_RETURN SET_KEY_VALUE_ID; return keyValues
#define TABLE_NAME "user_profiles"
#define INIT_ARGS QVariantList args; args << ID_TO_QSTR( m_game->currentFate().questionType ); args << ID_TO_QSTR( m_game->currentFate().formatType ); args << m_game->currentFate().questionId

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
        id(0), female(false), points(0), maxLevel(0), played(0)
{
}

UserManager::UserManager(Persistance* persist, DatabaseHelper* db, Game* game) :
        m_persist(persist), m_db(db), m_game(game)
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
            m_profile.played = profile.value(KEY_NUM_PLAYED).toInt();

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


int UserManager::numPlayed() const {
    return m_profile.played;
}


void UserManager::setNumPlayed(int played) {
    m_profile.played = played;
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
    keyValues[KEY_NUM_PLAYED] = m_profile.played;

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
    m_db->attachIfNecessary(ANALYTIC_DB_NAME, true);

    m_db->startTransaction(this, QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS user_profiles (id INTEGER PRIMARY KEY, name TEXT NOT NULL, kunya TEXT, female INTEGER, points INTEGER DEFAULT 0, highest_level INTEGER DEFAULT 0, played INTEGER DEFAULT 0, UNIQUE(name,kunya,female) ON CONFLICT IGNORE)"), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS %1.chosen_answers (id INTEGER PRIMARY KEY, presented INTEGER NOT NULL DEFAULT 0, chosen INTEGER NOT NULL DEFAULT 0)").arg(ANALYTIC_DB_NAME), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS %1.numeric_answers (id INTEGER PRIMARY KEY, question_type TEXT NOT NULL, question_id INTEGER, entry INTEGER NOT NULL)").arg(ANALYTIC_DB_NAME), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS %1.question_answered (id INTEGER PRIMARY KEY, question_type TEXT NOT NULL, question_format TEXT NOT NULL, question_id INTEGER, elapsed INTEGER, passed INTEGER)").arg(ANALYTIC_DB_NAME), QueryId::Setup);
    m_db->executeInternal( QString("CREATE TABLE IF NOT EXISTS %1.lifelines (id INTEGER PRIMARY KEY, question_type TEXT NOT NULL, question_format TEXT NOT NULL, question_id INTEGER, lifeline INTEGER NOT NULL)").arg(ANALYTIC_DB_NAME), QueryId::Setup);
    m_db->endTransaction(this, QueryId::Setup);

    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( commitChanges() ) );
    m_persist->registerForSetting(this, KEY_SETTING_USER_PROFILE, true);
}


void UserManager::recordStats(bb::cascades::ArrayDataModel* adm, QVariantList const& selected, bb::cascades::TextField* tf, int elapsed, bool passed)
{
    m_db->startTransaction(this, QueryId::RecordStats);

    if ( tf->isVisible() )
    {
        QVariantList args;
        args << ID_TO_QSTR( m_game->currentFate().questionType );
        args << m_game->currentFate().questionId;
        args << tf->text().trimmed().toInt();

        m_db->executeInternal( QString("INSERT INTO %1.numeric_answers (question_type,question_id,entry) VALUES (?,?,?)").arg(ANALYTIC_DB_NAME), QueryId::FetchAllProfiles, args );
    } else if ( m_game->multipleChoice() && !m_game->booleanQuestion() ) {
        QSet<int> selectedIndices;
        foreach (QVariant const& q, selected) {
            selectedIndices << q.toList().first().toInt();
        }

        const QString summer = "COALESCE((SELECT %2 FROM chosen_answers WHERE id=?)+%1,%1)";

        for (int i = adm->size()-1; i >= 0; i--)
        {
            QVariantMap qvm = adm->value(i).toMap();

            if ( qvm.contains(KEY_ANSWER_ID) )
            {
                QVariantList args;
                args << qvm.value(KEY_ANSWER_ID);
                args << qvm.value(KEY_ANSWER_ID);
                args << qvm.value(KEY_ANSWER_ID);

                m_db->executeInternal( QString("INSERT OR REPLACE INTO %3.chosen_answers (id,presented,chosen) VALUES (?,%1,%2)").arg( summer.arg(1).arg("presented") ).arg( summer.arg( selectedIndices.contains(i) ? 1 : 0 ).arg("chosen") ).arg(ANALYTIC_DB_NAME), QueryId::FetchAllProfiles, args );
            }
        }
    }

    INIT_ARGS;
    args << elapsed;
    args << passed;

    m_db->executeInternal( QString("INSERT INTO %1.question_answered (question_type,question_format,question_id,elapsed,passed) VALUES (?,?,?,?,?)").arg(ANALYTIC_DB_NAME), QueryId::RecordStats, args );
    m_db->endTransaction(this, QueryId::RecordStats);
}


void UserManager::recordLifeLine(int lifeKey)
{
    INIT_ARGS;
    args << LID_TO_QSTR(lifeKey);

    m_db->executeInternal( QString("INSERT INTO %1.lifelines (question_type,question_format,question_id,lifeline) VALUES (?,?,?,?)").arg(ANALYTIC_DB_NAME), QueryId::RecordStats, args );
}


UserManager::~UserManager()
{
}

} /* namespace canadainc */
