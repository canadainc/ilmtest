#include "precompiled.h"

#include "IlmHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "TextUtils.h"

#define NAME_FIELD(var,fieldName) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' ) AS %4").arg(var).arg( QChar(8217) ).arg( QChar(8216) ).arg(fieldName)
#define NUMERIC_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 1").arg( NAME_FIELD("i", KEY_ARG_1) ).arg(TOTAL_COUNT_VALUE).arg(field)
#define ORDERED_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 4").arg( NAME_FIELD("i", KEY_ARG_1) ).arg(KEY_SORT_ORDER).arg(field)

namespace ilmtest {

using namespace canadainc;

IlmHelper::IlmHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void IlmHelper::numericBirthYearForIndividual(QObject* caller) {
    m_sql->executeQuery( caller, NUMERIC_FIELD_QUERY("birth"), QueryId::NumericBirthYearForIndividual );
}


void IlmHelper::numericDeathYearForIndividual(QObject* caller) {
    m_sql->executeQuery( caller, NUMERIC_FIELD_QUERY("death"), QueryId::NumericDeathYearForIndividual );
}


void IlmHelper::orderedPeopleByBirth(QObject* caller) {
    m_sql->executeQuery(caller, ORDERED_FIELD_QUERY("birth"), QueryId::OrderedPeopleByBirth);
}


void IlmHelper::orderedPeopleByDeath(QObject* caller) {
    m_sql->executeQuery(caller, ORDERED_FIELD_QUERY("death"), QueryId::OrderedPeopleByDeath);
}


void IlmHelper::standardSahabah(QObject* caller) {
    lookupByField(caller, 1, QueryId::StandardSahabah);
}


void IlmHelper::standardTabiee(QObject* caller) {
    lookupByField(caller, 2, QueryId::StandardTabiee);
}


void IlmHelper::standardFemale(QObject* caller)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(correctAnswerLimits > 1 ? 0 : 1, 4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM individuals i WHERE female=1 AND hidden ISNULL AND death > 0 ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(correctAnswerLimits), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM individuals i WHERE female ISNULL AND hidden ISNULL AND prefix ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(incorrectAnswerLimits), QueryId::StandardFemale);
}


void IlmHelper::standardTabiTabiee(QObject* caller) {
    lookupByField(caller, 3, QueryId::StandardTabiTabiee);
}


void IlmHelper::lookupByField(QObject* caller, int fieldValue, QueryId::Type t, QString const& field)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(correctAnswerLimits > 1 ? 0 : 1, 4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM individuals i WHERE %4=%3 AND hidden ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(correctAnswerLimits).arg(fieldValue).arg(field), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM individuals i WHERE %4 <> %3 AND hidden ISNULL AND prefix ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(incorrectAnswerLimits).arg(fieldValue).arg(field), t);
}


void IlmHelper::customQuestion(QObject* caller)
{
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT q.id,q.standard_body,q.ordered_body,q.count_body,q.before_body,q.after_body,sp1.heading AS heading1,sp2.heading AS heading2,q.source_id FROM questions q LEFT JOIN questions p ON q.source_id=p.id LEFT JOIN suite_pages sp1 ON q.suite_page_id=sp1.id LEFT JOIN suite_pages sp2 ON p.suite_page_id=sp2.id) ORDER BY RANDOM() LIMIT 1"), QueryId::CustomQuestion);
}


void IlmHelper::getChoicesForCustomQuestion(QObject* caller, int questionId)
{
    m_sql->executeQuery(caller, QString("SELECT choices.id AS id,value_text AS value,sort_order,correct,source_id FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 UNION SELECT choices.id,choices.value_text AS value,NULL AS sort_order,NULL AS correct,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1) ORDER BY source_id").arg(questionId), QueryId::GetChoicesForCustomQuestion);
}


void IlmHelper::getOrderedChoicesForCustomQuestion(QObject* caller, int questionId)
{
    int limit = TextUtils::randInt(2,8);

    m_sql->executeQuery(caller, QString("SELECT choices.id AS id,value_text AS value,sort_order,source_id FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 AND correct=1 UNION SELECT choices.id,choices.value_text AS value,NULL AS sort_order,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1 AND correct=1) ORDER BY source_id LIMIT %2").arg(questionId).arg(limit), QueryId::GetOrderedChoicesForCustomQuestion);
}


void IlmHelper::getCorrectCountForCustomQuestion(QObject* caller, int questionId)
{
    m_sql->executeQuery(caller, QString("SELECT COUNT() AS %2 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 AND correct=1").arg(questionId).arg(TOTAL_COUNT_VALUE), QueryId::GetCorrectCountForCustomQuestion);
}


void IlmHelper::lazyInit()
{
    QStringList languages = QStringList() << QUESTION_BANK("english");

    foreach (QString const& language, languages) {
        m_sql->attachIfNecessary(language);
    }
}

IlmHelper::~IlmHelper()
{
}

} /* namespace ilm */
