#include "precompiled.h"

#include "QuranHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "QueryId.h"
#include "TextUtils.h"

#define DB_ARABIC "quran_arabic"
#define DB_ENGLISH "quran_english"

namespace quran {

using namespace canadainc;
using namespace ilmtest;

QuranHelper::QuranHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void QuranHelper::lazyInit()
{
    QStringList languages = QStringList() << DB_ARABIC << DB_ENGLISH;

    foreach (QString const& language, languages) {
        m_sql->attachIfNecessary(language);
    }
}


void QuranHelper::fetchRandomSurahs(QObject* caller)
{
    m_sql->executeQuery(caller, "SELECT surahs.id AS sort_order,name || ' (' || transliteration || ')' AS value FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 4", QueryId::FetchRandomSurahs);
}


void QuranHelper::fetchRandomVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, "SELECT name,transliteration,verse_count FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 1", QueryId::FetchRandomVerseCount);
}


QuranHelper::~QuranHelper()
{
}

} /* namespace oct10 */
