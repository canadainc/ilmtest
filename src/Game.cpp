#include "precompiled.h"

#include "Game.h"
#include "Logger.h"
#include "QueryId.h"

namespace ilmtest {

Game::Game(QObject* quranHelper, QObject* ilm) : m_quran(quranHelper), m_ilm(ilm)
{
}


void Game::lazyInit()
{
}


void Game::nextQuestion(QObject* caller, int t)
{
    QString f = QString( QueryId::staticMetaObject.enumerator(0).valueToKey( (QueryId::Type)t ) );
    f = f.at(0).toLower() + f.mid(1);
    LOGGER(f);
    QByteArray qba = f.toUtf8();
    const char* func = qba.constData();

    QObject* target = m_quran;

    if (t > QueryId::FetchVersesForRandomSurah) {
        target = m_ilm;
    }

    QMetaObject::invokeMethod( target, func, Qt::QueuedConnection, Q_ARG(QObject*, caller) );
}


Game::~Game()
{
}

} /* namespace ilmtest */
