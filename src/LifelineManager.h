#ifndef LIFELINEMANAGER_H_
#define LIFELINEMANAGER_H_

#include <QObject>

#include "Lifeline.h"

namespace bb {
    namespace cascades {
        class ArrayDataModel;
        class TextField;
    }
}

namespace ilmtest {

class Game;

struct LifelineData
{
    Lifeline::Type key;
    QString title;
    QString imageSource;

    LifelineData(Lifeline::Type t=Lifeline::Unknown, QString const& titleValue="", QString const& image="") : key(t), title(titleValue), imageSource(image) {}
};

class LifelineManager : public QObject
{
    Q_OBJECT

    Game* m_game;
    QMap<int, LifelineData> m_levelToLifeline;
    QMap<QString, LifelineData> m_codeToLifeline;

    void useAskExpert(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);
    void useFiftyFifty(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);
    void useTakeOne(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted=false);

private slots:
    void onCurrentLevelChanged();

signals:
    void lifeLineAvailable(QString const& title, QString const& imageSource, int key);
    void lifeLineUsed(int key);

public:
    LifelineManager(Game* game);
    virtual ~LifelineManager();

    Q_INVOKABLE bool unlock(QString const& key);
    Q_INVOKABLE void useLifeline(int key, bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted);
    Q_INVOKABLE static QString keyToString(int q);

    void lazyInit();
};

} /* namespace canadainc */

#endif /* LIFELINEMANAGER_H_ */
