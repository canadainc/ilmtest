#include "precompiled.h"

#include "ShopManager.h"
#include "Lifeline.h"
#include "Plugin.h"
#include "Persistance.h"
#include "TextUtils.h"
#include "UserManager.h"

#define KEY_GOOD_TYPE "type"
#define KEY_PURCHASE_CODE "value"
#define KEY_PURCHASE_PRICE "price"
#define KEY_PURCHASED_LIFELINES "purchased/lifelines"
#define KEY_PURCHASED_PLUGINS "purchased/plugins"
#define MANUFACTURE_LIFELINE(lifeline, title, description, image, price) m_market[ LID_TO_QSTR(lifeline) ] = createProduct(title, description, image, price, LID_TO_QSTR(lifeline), PurchaseType::Lifeline )
#define MANUFACTURE_PLUGIN(plugin, title, description, image, price) m_market[ PLUGID_TO_QSTR(plugin) ] = createProduct(title, description, image, price, PLUGID_TO_QSTR(plugin), PurchaseType::Plugin )

namespace {

class PurchaseType
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        Lifeline,
        Plugin
    };
};

QVariantMap createProduct(QString const& name, QString const& description, QString const& image, int price, QString const& purchaseCode, PurchaseType::Type type)
{
    QVariantMap qvm;
    qvm["title"] = name;
    qvm["description"] = description;
    qvm["imageSource"] = image;
    qvm[KEY_PURCHASE_PRICE] = price;
    qvm[KEY_PURCHASE_CODE] = purchaseCode;
    qvm[KEY_GOOD_TYPE] = type;

    return qvm;
}

}

namespace ilmtest {

using namespace canadainc;

ShopManager::ShopManager(Persistance* p, UserManager* user) :
        m_persist(p), m_user(user), m_adm(this)
{
}


QStringList ShopManager::getLifelines() {
    return m_persist->getValueFor(KEY_PURCHASED_LIFELINES).toStringList();
}


QStringList ShopManager::getPlugins() {
    return m_persist->getValueFor(KEY_PURCHASED_PLUGINS).toStringList();
}


void ShopManager::lazyInit()
{
    MANUFACTURE_LIFELINE(Lifeline::AskAnExpert, tr("Ask an Expert"), tr("Always get the right answer!"), "images/list/lifelines/ic_lifeline_expert.png", 200);
    MANUFACTURE_LIFELINE(Lifeline::ExtendTime, tr("Extend Clock"), tr("Add an additional 30 seconds to the clock!"), "images/list/lifelines/ic_lifeline_add_clock.png", 50);
    MANUFACTURE_LIFELINE(Lifeline::SecondChance, tr("Second Chance"), tr("Get two guesses to the correct answer"), "images/list/lifelines/ic_lifeline_second.png", 150);
    MANUFACTURE_LIFELINE(Lifeline::TakeOne, tr("Take One"), tr("Remove a wrong answer!"), "images/list/lifelines/ic_lifeline_take_one.png", 100);

    MANUFACTURE_PLUGIN(Plugin::ExposeAnswer, tr("Expose Answer"), tr("Display the correct answer if you get it wrong!"), "images/list/ic_expose.png", 15);

    QStringList purchases = getLifelines();
    QMap<QString,QVariant> map = m_market;

    foreach (QString const& purchase, purchases) {
        map.remove(purchase);
    }

    m_adm.clear();
    m_adm.append( map.values() );

    qmlRegisterUncreatableType<Plugin>("com.canadainc.data", 1, 0, "Plugin", "Can't instantiate");
}


QVariant ShopManager::getModel() {
    return QVariant::fromValue(&m_adm);
}


void ShopManager::refundLifeline(int lifeLineId)
{
    QString lifeline = LID_TO_QSTR(lifeLineId);
    QStringList purchased = getLifelines();
    purchased.removeAll(lifeline);
    m_persist->saveValueFor(KEY_PURCHASED_LIFELINES, purchased, false);

    m_adm.append( m_market.value(lifeline) );
}


void ShopManager::refundPlugin(int pluginId)
{
    QString plugin = PLUGID_TO_QSTR(pluginId);
    QStringList purchased = getPlugins();
    purchased.removeAll(plugin);
    m_persist->saveValueFor(KEY_PURCHASED_PLUGINS, purchased, false);

    m_adm.append( m_market.value(plugin) );
}


void ShopManager::purchase(QString const& purchaseCode)
{
    for (int i = m_adm.size()-1; i >= 0; i--)
    {
        QVariantMap current = m_adm.value(i).toMap();

        if ( current.value(KEY_PURCHASE_CODE).toString() == purchaseCode )
        {
            int price = current.value(KEY_PURCHASE_PRICE).toInt();
            m_adm.removeAt(i);
            m_user->setPoints( m_user->points()-price );

            int type = current.value(KEY_GOOD_TYPE).toInt();

            if (type == PurchaseType::Lifeline)
            {
                QStringList purchased = getLifelines();
                purchased << purchaseCode;
                m_persist->saveValueFor(KEY_PURCHASED_LIFELINES, purchased, false);
            } else if (type == PurchaseType::Plugin) {
                QStringList purchased = getPlugins();
                purchased << purchaseCode;
                m_persist->saveValueFor(KEY_PURCHASED_PLUGINS, purchased, false);
            }

            break;
        }
    }
}


bool ShopManager::isExposePurchased() {
    return getPlugins().contains( PLUGID_TO_QSTR(Plugin::ExposeAnswer) );
}


ShopManager::~ShopManager()
{
    m_adm.setParent(NULL);
}

} /* namespace ilmtest */
