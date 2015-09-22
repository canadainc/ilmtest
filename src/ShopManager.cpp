#include "precompiled.h"

#include "ShopManager.h"
#include "Lifeline.h"
#include "Persistance.h"
#include "TextUtils.h"
#include "UserManager.h"

#define KEY_PURCHASE_CODE "value"
#define KEY_PURCHASE_PRICE "price"
#define KEY_PURCHASED_LIFELINES "purchased/lifelines"
#define MANUFACTURE_LIFELINE(lifeline, title, description, image, price) m_market[ LID_TO_QSTR(lifeline) ] = createProduct(title, description, image, price, LID_TO_QSTR(lifeline), PurchaseType::Lifeline )

namespace {

class PurchaseType
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        Lifeline
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
    qvm["type"] = type;

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


void ShopManager::lazyInit()
{
    MANUFACTURE_LIFELINE(Lifeline::AskAnExpert, tr("Ask an Expert"), tr("Always get the right answer!"), "images/list/lifelines/ic_lifeline_expert.png", 200);
    MANUFACTURE_LIFELINE(Lifeline::SecondChance, tr("Second Chance"), tr("Get two guesses to the correct answer"), "images/list/lifelines/ic_lifeline_second.png", 3000);
    MANUFACTURE_LIFELINE(Lifeline::TakeOne, tr("Take One"), tr("Remove a wrong answer!"), "images/list/lifelines/ic_lifeline_take_one.png", 100);

    QStringList purchases = getLifelines();
    QMap<QString,QVariant> map = m_market;

    foreach (QString const& purchase, purchases) {
        map.remove(purchase);
    }

    m_adm.clear();
    m_adm.append( map.values() );
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

            QStringList purchased = getLifelines();
            purchased << purchaseCode;
            m_persist->saveValueFor(KEY_PURCHASED_LIFELINES, purchased, false);

            break;
        }
    }
}


ShopManager::~ShopManager()
{
    m_adm.setParent(NULL);
}

} /* namespace ilmtest */
