#ifndef SHOPMANAGER_H_
#define SHOPMANAGER_H_

#include <bb/cascades/ArrayDataModel>

namespace canadainc {
    class Persistance;
}

namespace ilmtest {

using namespace bb::cascades;
using namespace canadainc;

class UserManager;

class ShopManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ getModel FINAL)
    Q_PROPERTY(bool isExposePurchased READ isExposePurchased FINAL)

    Persistance* m_persist;
    UserManager* m_user;
    ArrayDataModel m_adm;
    QMap<QString,QVariant> m_market;

    int getAvailableProductIndex(QString const& purchaseCode);

public:
    ShopManager(Persistance* p, UserManager* user);
    virtual ~ShopManager();

    QStringList getPlugins();
    QStringList getLifelines();
    void refundLifeline(int lifeLineId);
    Q_INVOKABLE void refundPlugin(int pluginId);
    QVariant getModel();
    Q_INVOKABLE void purchase(QString const& purchaseCode);
    Q_INVOKABLE void purchasePlugin(int pluginId);
    Q_INVOKABLE QVariantMap getAvailablePlugin(int pluginId);
    void lazyInit();

    bool isExposePurchased();
};

} /* namespace ilmtest */

#endif /* SHOPMANAGER_H_ */
