#ifndef LIFELINEMANAGER_H_
#define LIFELINEMANAGER_H_

#include <QObject>

namespace bb {
    namespace cascades {
        class ArrayDataModel;
    }
}

namespace ilmtest {

class LifelineManager : public QObject
{
    Q_OBJECT

    void solveSorted(bb::cascades::ArrayDataModel* adm, int count);
    void eliminateIncorrect(bb::cascades::ArrayDataModel* adm, int count);

public:
    LifelineManager();
    virtual ~LifelineManager();

    Q_INVOKABLE void useFiftyFifty(bb::cascades::ArrayDataModel* adm, bool sorted=false);
    Q_INVOKABLE void useTakeOne(bb::cascades::ArrayDataModel* adm, bool sorted=false);
};

} /* namespace canadainc */

#endif /* LIFELINEMANAGER_H_ */
