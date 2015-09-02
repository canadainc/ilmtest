#include "precompiled.h"

#include "IlmHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "QueryId.h"

#define NUMERIC_FIELD_QUERY(field) QString("SELECT %1 AS %4,%3 AS %2 FROM individuals i WHERE %3 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 1").arg( NAME_FIELD("i") ).arg(TOTAL_COUNT_VALUE).arg(field).arg(KEY_ARG_1)
#define ORDERED_FIELD_QUERY(field) QString("SELECT %1 AS %2,%4 AS %3 FROM individuals i WHERE %4 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 4").arg( NAME_FIELD("i") ).arg(KEY_CHOICE_VALUE).arg(KEY_SORT_ORDER).arg(field)
#define NAME_FIELD(var) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' )").arg(var).arg( QChar(8217) ).arg( QChar(8216) )

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
