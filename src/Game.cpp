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
#define KEY_QUESTION_BODY "question"
#define ID_TO_QSTR(t) QString( QueryId::staticMetaObject.enumerator(0).valueToKey( (QueryId::Type)t ) )

namespace ilmtest {

using namespace canadainc;

Game::Game(DatabaseHelper* db) :
        m_ilm(db)
{
}


void Game::lazyInit()
{
    m_ilm.lazyInit();
}


void Game::onDataLoaded(QVariant idV, QVariant dataV)
{
    int id = idV.toInt();

    m_currentQuestion.clear();
    QString t = ID_TO_QSTR(id);
    QVariantList data = dataV.toList();
    LOGGER(t);

    int n = data.size();

    if (n > 0)
    {
        if ( n == 1 && data.first().toMap().contains(KEY_ARG_1) ) {
            m_arg1 = data.first().toMap().value(KEY_ARG_1).toString();
        }

        if ( t.startsWith("Numeric") ) {
            data = generateNumeric(data);
        } else if ( t.startsWith("Ordered") ) {
            m_currentQuestion[KEY_ORDERED] = true;
        } else if ( t.startsWith("Standard") ) {
            m_currentQuestion[KEY_STANDARD] = true;
            data = Offloader::mergeAndShuffle(data, m_tempList);
        } else if ( t.startsWith("Bool") ) {
            m_currentQuestion[KEY_BOOLEAN] = true;
        } else if ( t.startsWith("Custom") ) {
            m_currentQuestion = data.first().toMap();
            processCustom( (QueryId::Type)id );
        } else if ( t.startsWith("AnswersForCustom") ) {
            data = processAnswersForCustomQuestion( (QueryId::Type)id, data );
        }
    }

    if (id == QueryId::TempList) {
        m_tempList = data;
    }

    if ( multipleChoice() || numeric() )
    {
        m_currentQuestion["id"] = idV;
        m_currentQuestion["choices"] = data;
        LOGGER(m_currentQuestion);
        emit currentQuestionChanged();
    }
}


QVariantList Game::processAnswersForCustomQuestion(QueryId::Type id, QVariantList data)
{
    if (id == QueryId::AnswersForCustomBoolStandardQuestion || id == QueryId::AnswersForCustomPromptStandardQuestion) {
        m_arg1 = m_arg1.arg( data.first().toMap().value(KEY_CHOICE_VALUE).toString() );
        data = Offloader::transformToStandard(data, false);
        data = Offloader::setChoices( id == QueryId::AnswersForCustomBoolStandardQuestion ? tr("True") : tr("Yes"), id == QueryId::AnswersForCustomBoolStandardQuestion ? tr("False") : tr("No"), data.first().toMap().value(KEY_FLAG_CORRECT).toInt() == 1 );
        m_currentQuestion[KEY_BOOLEAN] = true;
        m_currentQuestion[KEY_STANDARD] = true;
    } else if (id == QueryId::AnswersForCustomBoolCountQuestion || id == QueryId::AnswersForCustomPromptCountQuestion) {
        bool truth = m_destiny.truthType == QueryId::GenerateTruth;
        int answer = data.first().toMap().value(TOTAL_COUNT_VALUE).toInt();
        data = Offloader::setChoices( id == QueryId::AnswersForCustomBoolCountQuestion ? tr("True") : tr("Yes"), id == QueryId::AnswersForCustomBoolCountQuestion ? tr("False") : tr("No"), truth );
        m_arg1 = m_arg1.arg( truth ? answer : TextUtils::randInt(answer+1, answer+15) );
        m_currentQuestion[KEY_BOOLEAN] = true;
        m_currentQuestion[KEY_STANDARD] = true;
    } else if (id == QueryId::AnswersForCustomOrderedQuestion) {
        data = Offloader::transformToStandard(data);
        m_currentQuestion[KEY_ORDERED] = true;
    } else if (id == QueryId::AnswersForCustomCountQuestion) {
        data = generateNumeric(data);
    } else if (id == QueryId::AnswersForCustomStandardQuestion) {
        if ( data.size() == 1 && QRegExp("\\d+$").exactMatch( data.first().toMap().value(KEY_CHOICE_VALUE).toString() ) ) {
            data = generateNumeric(data, KEY_CHOICE_VALUE);
        } else {
            data = Offloader::transformToStandard(data);
            m_currentQuestion[KEY_STANDARD] = true;
        }
    }

    m_currentQuestion["question"] = m_arg1;

    return data;
}


void Game::processCustom(QueryId::Type t)
{
    int questionId = REAL_ID(m_currentQuestion);

    if (t == QueryId::CustomBoolCountQuestion) {
        m_ilm.answersForCustomBoolCountQuestion(this, questionId);
    } else if (t == QueryId::CustomBoolStandardQuestion) {
        m_ilm.answersForCustomBoolStandardQuestion(this, questionId, m_destiny.truthType == QueryId::GenerateTruth);
    } else if (t == QueryId::CustomCountQuestion) {
        m_ilm.answersForCustomCountQuestion(this, questionId);
    } else if (t == QueryId::CustomOrderedQuestion) {
        m_ilm.answersForCustomOrderedQuestion(this, questionId);
    } else if (t == QueryId::CustomPromptCountQuestion) {
        m_ilm.answersForCustomPromptCountQuestion(this, questionId);
    } else if (t == QueryId::CustomPromptStandardQuestion) {
        m_ilm.answersForCustomPromptStandardQuestion(this, questionId, m_destiny.truthType == QueryId::GenerateTruth);
    } else if (t == QueryId::CustomStandardQuestion) {
        m_ilm.answersForCustomStandardQuestion(this, questionId);
    }
}


QVariantList Game::generateNumeric(QVariantList data, QString const& key)
{
    QVariantMap qvm = data.first().toMap();
    m_currentQuestion[KEY_NUMERIC] = true;

    int answer = qvm.value( !key.isNull() ? key : TOTAL_COUNT_VALUE ).toInt();

    if (m_destiny.formatType == QueryId::MultipleChoice) {
        data = Offloader::generateChoices(answer);
        m_currentQuestion[KEY_STANDARD] = true;
    } else if (m_destiny.formatType == QueryId::TextInput) {
        m_currentQuestion[KEY_ANSWER] = answer;
    }

    return data;
}


void Game::nextQuestion(int q, int requestedFormat, int requestedBool)
{
    m_destiny = Destiny(requestedFormat, q, requestedBool);
    m_tempList.clear();
    m_arg1.clear();

    QString f = ID_TO_QSTR(q);
    f = f.at(0).toLower() + f.mid(1);
    QByteArray qba = f.toUtf8();

    QMetaObject::invokeMethod( &m_ilm, qba.constData(), Qt::QueuedConnection, Q_ARG(QObject*, this) );
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
