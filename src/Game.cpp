#include "precompiled.h"

#include "Game.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "Offloader.h"
#include "QueryId.h"
#include "TextUtils.h"

#define KEY_BOOLEAN "boolean"
#define KEY_NUMERIC "numeric"
#define KEY_ORDERED "ordered"
#define KEY_STANDARD "standard"
#define ID_TO_QSTR(t) QString( QueryId::staticMetaObject.enumerator(0).valueToKey( (QueryId::Type)t ) )

namespace {

class NumericQuestion
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        MultipleChoice,
        TextInput
    };
};

}

namespace ilmtest {

using namespace canadainc;

Game::Game(DatabaseHelper* db) : m_quran(db), m_ilm(db)
{
}


void Game::lazyInit()
{
    m_quran.lazyInit();
    m_ilm.lazyInit();
}


void Game::onDataLoaded(QVariant idV, QVariant dataV)
{
    int id = idV.toInt();

    m_currentQuestion.clear();
    m_currentQuestion["id"] = idV;

    QString t = ID_TO_QSTR(id);
    QVariantList data = dataV.toList();
    LOGGER(t);

    bool questionChanged = true;
    int n = data.size();

    if ( n == 1 && data.first().toMap().contains(KEY_ARG_1) ) {
        m_arg1 = data.first().toMap().value(KEY_ARG_1).toString();
    }

    if ( t.startsWith("Numeric") && n > 0 )
    {
        QVariantMap qvm = data.first().toMap();

        int answer = qvm.value(TOTAL_COUNT_VALUE).toInt();
        NumericQuestion::Type type = (NumericQuestion::Type)TextUtils::randInt(NumericQuestion::MultipleChoice, NumericQuestion::TextInput);

        if (type == NumericQuestion::MultipleChoice) {
            data = Offloader::generateChoices(answer);
            m_currentQuestion[KEY_STANDARD] = true;
        } else {
            m_currentQuestion[KEY_NUMERIC] = true;
            m_currentQuestion[KEY_ANSWER] = answer;
        }
    } else if ( t.startsWith("Ordered") ) {
        m_currentQuestion[KEY_ORDERED] = true;
    } else if ( t.startsWith("Standard") ) {
        m_currentQuestion[KEY_STANDARD] = true;
        data = Offloader::mergeAndShuffle(data, m_tempList);
    } else if ( t.startsWith("Bool") ) {
        m_currentQuestion[KEY_BOOLEAN] = true;
    } else {
        questionChanged = false;
    }

    if ( id == QueryId::TempList ) {
        m_tempList = data;
    }

    if (questionChanged) {
        m_currentQuestion["choices"] = data;
        LOGGER(m_currentQuestion);
        emit currentQuestionChanged();
    }
}


void Game::nextQuestion(int t)
{
    m_currentQuestion.clear();
    m_tempList.clear();
    m_arg1.clear();

    QString f = ID_TO_QSTR(t);
    f = f.at(0).toLower() + f.mid(1);
    QByteArray qba = f.toUtf8();
    const char* func = qba.constData();

    QObject* target = &m_quran;

    if (t > QueryId::StandardVersesForSurah) {
        target = &m_ilm;
    }

    QMetaObject::invokeMethod( target, func, Qt::QueuedConnection, Q_ARG(QObject*, this) );
}


QVariantMap Game::currentQuestion() const {
    return m_currentQuestion;
}


QString Game::arg1() const {
    return m_arg1;
}


bool Game::multipleChoice() const {
    return m_currentQuestion.contains(KEY_STANDARD) || booleanQuestion() || m_currentQuestion.contains(KEY_ORDERED);
}


bool Game::numeric() const {
    return m_currentQuestion.contains(KEY_NUMERIC);
}


bool Game::booleanQuestion() const {
    return m_currentQuestion.contains(KEY_BOOLEAN);
}


QString Game::formatQuestion(QString const& input)
{
    if ( input.contains("%1") ) {
        return input.arg(m_arg1);
    } else {
        return input;
    }
}


Game::~Game()
{
}

} /* namespace ilmtest */
