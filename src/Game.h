#ifndef GAME_H_
#define GAME_H_

#include <QObject>
#include "QueryId.h"

namespace ilmtest {

class Game : public QObject
{
    Q_OBJECT

    QObject* m_quran;
    QObject* m_ilm;

public:
    Game(QObject* quran, QObject* ilm);
    virtual ~Game();

    void lazyInit();
    Q_SLOT void nextQuestion(QObject* caller, int q);
};

} /* namespace ilmtest */

#endif /* GAME_H_ */
