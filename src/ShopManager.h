#ifndef SHOPMANAGER_H_
#define SHOPMANAGER_H_

#include <QObject>

namespace canadainc {
    class Persistance;
}

namespace ilmtest {

using namespace canadainc;

class ShopManager : public QObject
{
    Q_OBJECT

    Persistance* m_persist;

public:
    ShopManager(Persistance* p);
    virtual ~ShopManager();

    Q_INVOKABLE QVariantList getPurchases();
};

} /* namespace ilmtest */

#endif /* SHOPMANAGER_H_ */
