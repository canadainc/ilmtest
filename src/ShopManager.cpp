#include "precompiled.h"

#include "ShopManager.h"
#include "Persistance.h"

namespace ilmtest {

using namespace canadainc;

ShopManager::ShopManager(Persistance* p) : m_persist(p)
{
}


QVariantList ShopManager::getPurchases() {
    return m_persist->getValueFor("purchases").toList();
}


ShopManager::~ShopManager()
{
}

} /* namespace ilmtest */
