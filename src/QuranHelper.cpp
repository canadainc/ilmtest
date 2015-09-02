#include "precompiled.h"

#include "QuranHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "QueryId.h"
#include "TextUtils.h"

#define DB_ARABIC "quran_arabic"
#define DB_ENGLISH "quran_english"
#define FETCH_TABLE_COUNT(table) QString("SELECT COUNT() AS %1 FROM %2").arg(TOTAL_COUNT_VALUE).arg(table)
#define MERGE_COLUMNS(col1, col2, alias) QString("%1 || ' (' || %2 || ')' AS %3").arg(col1).arg(col2).arg(alias)
#define MERGE_SURAH_NAME MERGE_COLUMNS("name", "transliteration", KEY_ARG_1)
#define MERGE_SURAH_VALUE MERGE_COLUMNS("name", "transliteration", KEY_CHOICE_VALUE)
#define SURAH_AS_VALUE QString("name AS %1").arg(KEY_CHOICE_VALUE)
#define AYAT_AS_VALUE QString("content AS %1").arg(KEY_CHOICE_VALUE)
#define TRANSLATION_AS_DESCRIPTION "translation AS description"
#define RANDOM_SURAH TextUtils::randInt(1,114)
#define RESULT_SET_MIN 2
#define RESULT_SET_MAX 8
#define RESULT_SET_LIMIT TextUtils::randInt(RESULT_SET_MIN, RESULT_SET_MAX)

namespace ilmtest {

using namespace canadainc;

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


void QuranHelper::fetchSurahHeader(QObject* caller, int chapterNumber)
{
    LOGGER(chapterNumber);
    m_sql->executeQuery( caller, QString("SELECT %2 FROM surahs s INNER JOIN chapters c ON s.id=c.id WHERE s.id=%1").arg(chapterNumber).arg(MERGE_SURAH_NAME), QueryId::TempArgument1 );
}


void QuranHelper::boolSurahLocation(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    m_sql->executeQuery(caller, QString("SELECT %1,type FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id=%2").arg(MERGE_SURAH_NAME).arg(surahId), QueryId::BoolSurahLocation);
}


void QuranHelper::numericMaxVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT MAX(verse_count) AS %1 FROM surahs").arg(TOTAL_COUNT_VALUE), QueryId::NumericMaxVerseCount);
}

void QuranHelper::numericMinVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT MIN(verse_count) AS %1 FROM surahs").arg(TOTAL_COUNT_VALUE), QueryId::NumericMinVerseCount);
}


void QuranHelper::numericTotalAyatCount(QObject* caller) {
    m_sql->executeQuery(caller, FETCH_TABLE_COUNT("ayahs"), QueryId::NumericTotalAyatCount);
}


void QuranHelper::numericTotalSurahCount(QObject* caller) {
    m_sql->executeQuery(caller, FETCH_TABLE_COUNT("surahs"), QueryId::NumericTotalSurahCount);
}


void QuranHelper::numericVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT %1,verse_count AS total_count FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 1").arg(MERGE_SURAH_NAME), QueryId::NumericVerseCount);
}


void QuranHelper::orderedSurahs(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.id AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahs);
}


void QuranHelper::orderedRevelationSurahs(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.revelation_order AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedRevelationSurahs);
}


void QuranHelper::orderedSurahsByLength(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.verse_count AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahsByLength);
}


void QuranHelper::orderedSurahVerses(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    fetchSurahHeader(caller, surahId);
    m_sql->executeQuery(caller, QString("SELECT %1,%2,verse_id AS %3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%4 AND length(content) < 50 GROUP BY %3 ORDER BY RANDOM() LIMIT %5").arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(KEY_SORT_ORDER).arg(surahId).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahVerses);
}


void QuranHelper::standardSajdaSurah(QObject* caller)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(0,4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM sajdas INNER JOIN surahs ON sajdas.surah_id=surahs.id INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %2").arg(MERGE_SURAH_VALUE).arg(correctAnswerLimits), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id NOT IN (SELECT surah_id FROM sajdas) ORDER BY RANDOM() LIMIT %2").arg(MERGE_SURAH_VALUE).arg(incorrectAnswerLimits), QueryId::StandardSajdaSurah);
}


void QuranHelper::standardVersesForSurah(QObject* caller)
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
    m_sql->executeQuery(caller, QString("SELECT %2,%3,1 AS correct FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 AND length(content) < 50 ORDER BY RANDOM() LIMIT %4").arg(chapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(2), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %2,%3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 AND length(content) < 50 ORDER BY RANDOM() LIMIT %4").arg(wrongChapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(2), QueryId::StandardVersesForSurah);
}


QuranHelper::~QuranHelper()
{
}

} /* namespace ilmtest */
