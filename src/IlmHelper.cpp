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
    lookupByCompanionField(caller, 1, QueryId::StandardSahabah);
}


void IlmHelper::standardTabiee(QObject* caller) {
    lookupByCompanionField(caller, 2, QueryId::StandardTabiee);
}


void IlmHelper::standardTabiTabiee(QObject* caller) {
    lookupByCompanionField(caller, 3, QueryId::StandardTabiTabiee);
}


void IlmHelper::lookupByCompanionField(QObject* caller, int fieldValue, QueryId::Type t)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(0,4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM individuals i WHERE is_companion=%3 ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(correctAnswerLimits).arg(fieldValue), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM individuals i WHERE is_companion <> %3 AND hidden ISNULL AND prefix ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(incorrectAnswerLimits).arg(fieldValue), t);
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
