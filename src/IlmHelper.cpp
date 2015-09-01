#include "precompiled.h"

#include "IlmHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "QueryId.h"

#define NAME_FIELD(var) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' )").arg(var).arg( QChar(8217) ).arg( QChar(8216) )

namespace ilmtest {

using namespace canadainc;

IlmHelper::IlmHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void IlmHelper::getBirthYearForIndividual(QObject* caller) {
    m_sql->executeQuery( caller, QString("SELECT %1 AS name,birth AS %2 FROM individuals i WHERE birth > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 1").arg( NAME_FIELD("i") ).arg(TOTAL_COUNT_VALUE), QueryId::GetBirthYearForIndividual );
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
