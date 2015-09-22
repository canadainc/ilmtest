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

    Persistance* m_persist;
    UserManager* m_user;
    ArrayDataModel m_adm;
    QMap<QString,QVariant> m_market;

public:
    ShopManager(Persistance* p, UserManager* user);
    virtual ~ShopManager();

    QStringList getLifelines();
    void refundLifeline(int lifeLineId);
    QVariant getModel();
    Q_INVOKABLE void purchase(QString const& purchaseCode);
    void lazyInit();
};

} /* namespace ilmtest */

#endif /* SHOPMANAGER_H_ */
