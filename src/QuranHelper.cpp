#include "precompiled.h"

#include "QuranHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "QueryId.h"
#include "TextUtils.h"

#define DB_ARABIC "quran_arabic"
#define DB_ENGLISH "quran_english"
#define MERGE_COLUMNS(col1, col2, alias) QString("%1 || ' (' || %2 || ')' AS %3").arg(col1).arg(col2).arg(alias)
#define MERGE_SURAH_NAME MERGE_COLUMNS("name", "transliteration", "surah_name")
#define SURAH_AS_VALUE "name AS value"
#define AYAT_AS_VALUE "content AS value"
#define TRANSLATION_AS_DESCRIPTION "translation AS description"
#define RANDOM_SURAH TextUtils::randInt(1,114)

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


void QuranHelper::fetchRandomSurahs(QObject* caller, bool revelation)
{
    LOGGER(revelation);
    m_sql->executeQuery(caller, QString("SELECT surahs.%3 AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(revelation ? "revelation_order" : "id"), revelation ? QueryId::FetchSurahsByRevealed : QueryId::FetchRandomSurahs);
}


void QuranHelper::fetchVersesForRandomSurah(QObject* caller)
{
    int chapter = RANDOM_SURAH;
    int wrongChapter = RANDOM_SURAH;

    if (wrongChapter == chapter)
    {
        if (chapter == 1) {
            wrongChapter = TextUtils::randInt(2,114);
        } else if (chapter == 114) {
            wrongChapter = TextUtils::randInt(1,113);
        } else { // 5, and wrongChapter = 5
            wrongChapter = TextUtils::randInt(chapter+1,114);
        }
    }

    fetchSurahHeader(caller, chapter);
    m_sql->executeQuery(caller, QString("SELECT %2,%3,1 AS correct FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 AND length(content) < 50 ORDER BY RANDOM() LIMIT 2").arg(chapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION), QueryId::PendingQuery);
    m_sql->executeQuery(caller, QString("SELECT %2,%3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 AND length(content) < 50 ORDER BY RANDOM() LIMIT 2").arg(wrongChapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION), QueryId::FetchVersesForRandomSurah);
}


void QuranHelper::fetchRandomSajdaSurah(QObject* caller)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(0,4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM sajdas INNER JOIN surahs ON sajdas.surah_id=surahs.id INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %2").arg(SURAH_AS_VALUE).arg(correctAnswerLimits), QueryId::PendingQuery);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id NOT IN (SELECT surah_id FROM sajdas) ORDER BY RANDOM() LIMIT %2").arg(SURAH_AS_VALUE).arg(incorrectAnswerLimits), QueryId::FetchRandomSajdaSurah);
}


void QuranHelper::fetchRandomVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT %1,verse_count FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 1").arg(MERGE_SURAH_NAME), QueryId::FetchRandomVerseCount);
}


void QuranHelper::fetchSurahHeader(QObject* caller, int chapterNumber)
{
    LOGGER(chapterNumber);
    m_sql->executeQuery( caller, QString("SELECT %2 FROM surahs s INNER JOIN chapters c ON s.id=c.id WHERE s.id=%1").arg(chapterNumber).arg(MERGE_SURAH_NAME), QueryId::FetchSurahHeader );
}


void QuranHelper::fetchRandomSurahLocation(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    m_sql->executeQuery(caller, QString("SELECT %1,type FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id=%2").arg(MERGE_SURAH_NAME).arg(surahId), QueryId::FetchRandomSurahLocation);
}


void QuranHelper::fetchSurahRandomVerses(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    fetchSurahHeader(caller, surahId);
    m_sql->executeQuery(caller, QString("SELECT %1,%2,verse_id AS %3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%4 AND length(content) < 50 GROUP BY %3 ORDER BY RANDOM() LIMIT 4").arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(KEY_SORT_ORDER).arg(surahId), QueryId::FetchSurahRandomVerses);
}


QuranHelper::~QuranHelper()
{
}

} /* namespace ilmtest */
