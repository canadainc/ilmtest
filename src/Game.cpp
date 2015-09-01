#include "precompiled.h"

#include "Game.h"
#include "Logger.h"
#include "QueryId.h"

namespace ilmtest {

Game::Game(QObject* quranHelper) : m_quran(quranHelper)
{
}


void Game::lazyInit()
{
}


void Game::nextQuestion(QObject* caller, int t)
{
    QString f = QString( QueryId::staticMetaObject.enumerator(0).valueToKey( (QueryId::Type)t ) );
    f = f.at(0).toLower() + f.mid(1);
    QByteArray qba = f.toUtf8();
    const char* func = qba.constData();

    QMetaObject::invokeMethod( m_quran, func, Qt::QueuedConnection, Q_ARG(QObject*, caller) );
}


Game::~Game()
{
}

} /* namespace ilmtest */
