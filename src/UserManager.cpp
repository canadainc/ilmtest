#include "UserManager.h"
#include "Persistance.h"
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

UserManager::UserManager(Persistance* persist) : m_persist(persist) {
    persist->registerForSetting(this, KEY_SETTING_USER_PROFILE);
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


void UserManager::lazyInit() {
    setPoints( m_persist->getValueFor("points").toInt() );
}


UserManager::~UserManager()
{
}

} /* namespace canadainc */
