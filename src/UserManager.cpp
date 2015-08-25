#include "UserManager.h"
#include "Persistance.h"

#define KEY_SETTING_NAME "userFullName"
#define KEY_SETTING_KUNYA "userKunya"
#define KEY_SETTING_FEMALE "userFullName"

namespace ilmtest {

using namespace canadainc;

UserManager::UserManager()
{
}


void UserManager::onSettingChanged(QVariant newValue, QVariant key)
{
    emit profileChanged();
}


QString UserManager::name() {
    return m_persist->getValueFor(KEY_SETTING_NAME).toString();
}


QString UserManager::kunya() {
    return m_persist->getValueFor(KEY_SETTING_KUNYA).toString();
}


bool UserManager::female() {
    return m_persist->getValueFor(KEY_SETTING_FEMALE).toInt() == 1;
}


UserManager::~UserManager()
{
}

} /* namespace canadainc */
