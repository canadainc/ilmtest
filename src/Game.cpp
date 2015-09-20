#include "precompiled.h"

#include "Game.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "Offloader.h"
#include "QueryId.h"
#include "TextUtils.h"

#define CUSTOM_QUESTION_PREFIX "Custom"
#define EMIT_ERROR(x) emit error( QString("%1 QuestionId: %2; QuestionType: %3; FormatType: %4; TruthType: %5; Debug: %6_%7:%8").arg(x).arg(m_destiny.questionId).arg( ID_TO_QSTR(id) ).arg( ID_TO_QSTR(m_destiny.formatType).arg( ID_TO_QSTR(m_destiny.truthType) ).arg(__FILE__).arg(__FUNCTION__).arg(__LINE__) ) )
#define EXTRACT_NUMERIC_ANSWER(x,key) x.first().toMap().value( !key.isNull() ? key : TOTAL_COUNT_VALUE ).toInt();
#define KEY_BOOLEAN "boolean"
#define KEY_NUMERIC "numeric"
#define KEY_ORDERED "ordered"
#define KEY_STANDARD "standard"
#define ID_TO_QSTR(t) QString( QueryId::staticMetaObject.enumerator(0).valueToKey( (QueryId::Type)t ) )
#define IS_NUMERIC_QUESTION(x) x.size() == 1 && QRegExp("\\d+$").exactMatch( x.first().toMap().value(KEY_CHOICE_VALUE).toString() )

namespace ilmtest {

using namespace canadainc;

Game::Game(DatabaseHelper* db) :
        m_ilm(db), m_level(0)
{
}


void Game::lazyInit()
{
    m_ilm.lazyInit();
    m_ilm.fetchDictionary(this);
}


void Game::setReference(QVariantMap const& first, QString const& idKey, QString const& authorKey, QString const& titleKey, QString const& headingKey, QString const& targetId, QString const& uriPrefix)
{
    m_reference[KEY_ID_FIELD] = first.value(idKey);
    m_reference["author"] = first.value(authorKey);
    m_reference["title"] = first.value(titleKey);
    m_reference["target"] = targetId;
    m_reference["uri"] = uriPrefix;

    if ( !first.value(headingKey).toString().isEmpty() && ( first.value(headingKey).toString() != first.value(titleKey).toString() ) ) {
        m_reference["pageTitle"] = first.value(headingKey);
    }
}


void Game::onDataLoaded(QVariant idV, QVariant dataV)
{
    int id = idV.toInt();

    if (id == QueryId::FetchDictionary)
    {
        QVariantList data = dataV.toList();
        QMap< QueryId::Type, QSet<qint64> > typeToQuestions;
        QMap< qint64, QList<QueryId::Type> > questionToTypes;

        foreach (QVariant const& q, data)
        {
            QVariantMap qvm = q.toMap();
            QueryId::Type t = (QueryId::Type)qvm.value(FIELD_COLUMN_TYPE).toInt();
            qint64 questionId = qvm.value(KEY_ID_FIELD).toLongLong();

            QSet<qint64> questions = typeToQuestions.value(t);
            questions << questionId;
            typeToQuestions[t] = questions;

            QList<QueryId::Type> types = questionToTypes.value(questionId);
            types << t;
            questionToTypes[questionId] = types;
        }

        while ( !questionToTypes.isEmpty() )
        {
            QMap<qint64, QueryId::Type> questions = Offloader::generateQuestions(typeToQuestions, questionToTypes);

            foreach ( qint64 questionId, questions.keys() )
            {
                Destiny d;
                d.questionId = questionId;
                d.questionType = questions.value(questionId);
                m_questionPath.enqueue(d);
            }
        }

        std::random_shuffle( m_questionPath.begin(), m_questionPath.end() );
    } else if (id != QueryId::Unknown) { // unknown ones are within transactions and should get ignored
        m_currentQuestion.clear();
        QString t = ID_TO_QSTR(id);
        QVariantList data = dataV.toList();
        LOGGER( t << ID_TO_QSTR(m_destiny.truthType) << ID_TO_QSTR(m_destiny.formatType) );

        int n = data.size();

        if (n > 0)
        {
            QVariantMap first = data.first().toMap();

            if (n == 1)
            {
                LOGGER(first << first.contains("spi1"));

                if ( first.contains(KEY_ARG_1) ) {
                    m_arg1 = first.value(KEY_ARG_1).toString();
                }

                if ( first.contains("spi1") )
                {
                    if ( first.value("ref_page_id").toLongLong() > 0 ) {
                        setReference(first, "spi2", "author2", "title2", "heading2");
                    } else {
                        setReference(first, "spi1", "author1", "title1", "heading1");
                    }
                }
            }
        }

        if ( t.startsWith("Numeric") && n > 0 ) {
            data = generateNumeric(data);
        } else if ( t.startsWith("Ordered") && n > 0 ) {
            m_currentQuestion[KEY_ORDERED] = true;
        } else if ( t.startsWith("Standard") ) {
            m_currentQuestion[KEY_STANDARD] = true;
            data = Offloader::mergeAndShuffle(data, QVariantList());
        } else if ( t.startsWith("Bool") ) {
            m_currentQuestion[KEY_BOOLEAN] = true;
        } else if ( t.startsWith(CUSTOM_QUESTION_PREFIX) ) {
            if (n > 0) {
                m_currentQuestion = data.first().toMap();
                processCustom( (QueryId::Type)id );
            } else {
                LOGGER("NoResults!");
                emit currentQuestionChanged();
            }
        } else if ( t.startsWith("After") ) {
            data = processOrdered(data, false);
        } else if ( t.startsWith("Before") ) {
            data = processOrdered(data, true);
        } else if ( t.startsWith("AnswersForCustom") ) {
            data = processAnswersForCustomQuestion( (QueryId::Type)id, data );
        }

        if ( multipleChoice() || numeric() )
        {
            m_currentQuestion["id"] = idV;
            m_currentQuestion["choices"] = data;

            if ( !m_reference.isEmpty() ) {
                m_currentQuestion["reference"] = m_reference;
            }

            LOGGER(m_currentQuestion);
            emit currentQuestionChanged();
        }
    }
}


QVariantList Game::generateNumericBoolean(int id, QVariantList data, QString const& key)
{
    LOGGER(data.size() << data);
    bool boolType = id == QueryId::AnswersForCustomBoolCountQuestion || id == QueryId::AnswersForCustomBoolStandardQuestion;
    bool truth = m_destiny.truthType == QueryId::GenerateTruth;
    int answer = data.first().toMap().value( !key.isNull() ? key : TOTAL_COUNT_VALUE ).toInt();
    data = Offloader::setChoices( boolType ? tr("True") : tr("Yes"), boolType ? tr("False") : tr("No"), truth );
    LOGGER(data);
    m_arg1 = m_arg1.arg( truth ? answer : TextUtils::randInt(answer+1, answer+15) );
    m_currentQuestion[KEY_BOOLEAN] = true;
    m_currentQuestion[KEY_STANDARD] = true;

    return data;
}


QVariantList Game::processAnswersForCustomQuestion(QueryId::Type id, QVariantList data)
{
    if (id == QueryId::AnswersForCustomBoolStandardQuestion || id == QueryId::AnswersForCustomPromptStandardQuestion)
    {
        if ( IS_NUMERIC_QUESTION(data) ) { // admin only put in the correct numeric answer, so we need to generate the answer based on whether destiny is correct or not
            data = generateNumericBoolean(id, data, KEY_CHOICE_VALUE);
        } else {
            data = Offloader::transformToStandard(data, false);
            QVariantMap qvm = Offloader::fetchRandomElement(data, m_destiny.truthType == QueryId::GenerateTruth);

            if ( !qvm.isEmpty() )
            {
                m_arg1 = m_arg1.arg( qvm.value(KEY_CHOICE_VALUE).toString() );
                data = Offloader::setChoices( id == QueryId::AnswersForCustomBoolStandardQuestion ? tr("True") : tr("Yes"), id == QueryId::AnswersForCustomBoolStandardQuestion ? tr("False") : tr("No"), m_destiny.truthType == QueryId::GenerateTruth );
                m_currentQuestion[KEY_BOOLEAN] = true;
                m_currentQuestion[KEY_STANDARD] = true;
            } else {
                EMIT_ERROR("ChoiceLess");
            }
        }
    } else if (id == QueryId::AnswersForCustomBoolCountQuestion || id == QueryId::AnswersForCustomPromptCountQuestion) {
        data = generateNumericBoolean(id, data);
    } else if (id == QueryId::AnswersForCustomOrderedQuestion) {
        data = Offloader::transformToStandard(data);
        m_currentQuestion[KEY_ORDERED] = true;
    } else if (id == QueryId::AnswersForCustomCountQuestion) {
        data = generateNumeric(data);
    } else if (id == QueryId::AnswersForCustomStandardQuestion || id == QueryId::AnswersForCustomStandardNegation) {
        if ( IS_NUMERIC_QUESTION(data) )
        {
            if (id == QueryId::AnswersForCustomStandardNegation) {
                EMIT_ERROR("NumericForStandardNegation");
            } else {
                data = generateNumeric(data, KEY_CHOICE_VALUE);
            }
        } else {
            data = Offloader::transformToStandard(data, true, id == QueryId::AnswersForCustomStandardNegation);
            m_currentQuestion[KEY_STANDARD] = true;
        }
    } else if (id == QueryId::AnswersForCustomAfterQuestion) {
        data = processOrdered(data, false);
    } else if (id == QueryId::AnswersForCustomBeforeQuestion) {
        data = processOrdered(data, true);
    }

    m_currentQuestion["question"] = m_arg1;

    return data;
}


QVariantList Game::processOrdered(QVariantList data, bool before)
{
    data = Offloader::processOrdered(data, m_arg1, before, m_destiny.questionType == QueryId::CustomAfterQuestion || m_destiny.questionType == QueryId::CustomBeforeQuestion);
    m_currentQuestion[KEY_STANDARD] = true;

    return data;
}


void Game::processCustom(QueryId::Type t)
{
    int questionId = REAL_ID(m_currentQuestion);
    m_destiny.questionId = questionId;

    if (t == QueryId::CustomBoolCountQuestion) {
        m_ilm.answersForCustomBoolCountQuestion(this, questionId);
    } else if (t == QueryId::CustomBoolStandardQuestion) {
        m_ilm.answersForCustomBoolStandardQuestion(this, questionId);
    } else if (t == QueryId::CustomCountQuestion) {
        m_ilm.answersForCustomCountQuestion(this, questionId);
    } else if (t == QueryId::CustomOrderedQuestion) {
        m_ilm.answersForCustomOrderedQuestion(this, questionId);
    } else if (t == QueryId::CustomPromptCountQuestion) {
        m_ilm.answersForCustomPromptCountQuestion(this, questionId);
    } else if (t == QueryId::CustomPromptStandardQuestion) {
        m_ilm.answersForCustomPromptStandardQuestion(this, questionId);
    } else if (t == QueryId::CustomStandardQuestion || t == QueryId::CustomStandardNegation) {
        m_ilm.answersForCustomStandardQuestion(this, questionId, t == QueryId::CustomStandardQuestion ? QueryId::AnswersForCustomStandardQuestion : QueryId::AnswersForCustomStandardNegation);
    } else if (t == QueryId::CustomAfterQuestion) {
        m_ilm.answersForCustomAfterQuestion(this, questionId);
    } else if (t == QueryId::CustomBeforeQuestion) {
        m_ilm.answersForCustomBeforeQuestion(this, questionId);
    }

    m_ilm.markVisited(questionId);
}


QVariantList Game::generateNumeric(QVariantList data, QString const& key)
{
    int answer = EXTRACT_NUMERIC_ANSWER(data, key);

    if (m_destiny.formatType == QueryId::MultipleChoice) {
        data = Offloader::generateChoices(answer);
        m_currentQuestion[KEY_STANDARD] = true;
    } else if (m_destiny.formatType == QueryId::TextInput) {
        m_currentQuestion[KEY_ANSWER] = answer;
        m_currentQuestion[KEY_NUMERIC] = true;
    }

    return data;
}


void Game::nextQuestion(int q, int requestedFormat, int requestedBool)
{
    ++m_level;
    emit levelChanged();

    m_destiny = Destiny(requestedFormat, q, requestedBool);
    m_arg1.clear();
    m_reference.clear();

    QString f = ID_TO_QSTR(q);

    if ( f.startsWith(CUSTOM_QUESTION_PREFIX) ) {
        Destiny d = m_questionPath.dequeue();
        m_destiny.questionType = d.questionType;
        m_destiny.questionId = d.questionId;

        m_ilm.fetchCustomColumn(this, m_destiny.questionType, m_destiny.questionId);
    } else {
        f = f.at(0).toLower() + f.mid(1);
        QByteArray qba = f.toUtf8();
        QMetaObject::invokeMethod( &m_ilm, qba.constData(), Qt::QueuedConnection, Q_ARG(QObject*, this) );
    }
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


void Game::reset() {
    m_ilm.resetVisited();
}


QString Game::queryToString(int q) {
    return ID_TO_QSTR(q);
}


void Game::reloadQuestions() {
    m_ilm.reloadQuestionBank();
}


int Game::level() const {
    return m_level;
}


Game::~Game()
{
}

} /* namespace ilmtest */
