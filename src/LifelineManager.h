#ifndef LIFELINEMANAGER_H_
#define LIFELINEMANAGER_H_

#include <QObject>

namespace bb {
    namespace cascades {
        class ArrayDataModel;
        class TextField;
    }
}

namespace ilmtest {

class Game;

class LifelineManager : public QObject
{
    Q_OBJECT

    Game* m_game;

    void solveSorted(bb::cascades::ArrayDataModel* adm, int count);
    void eliminateIncorrect(bb::cascades::ArrayDataModel* adm, int count);

public:
    LifelineManager(Game* game);
    virtual ~LifelineManager();

    Q_INVOKABLE void useAskExpert(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);
    Q_INVOKABLE void useFiftyFifty(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);
    Q_INVOKABLE void useTakeOne(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);
};

} /* namespace canadainc */

#endif /* LIFELINEMANAGER_H_ */
