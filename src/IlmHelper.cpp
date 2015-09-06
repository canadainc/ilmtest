#include "precompiled.h"

#include "IlmHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "TextUtils.h"

#define NAME_FIELD(var,fieldName) QString("replace( replace( replace( replace( coalesce(%1.displayName, TRIM((coalesce(%1.prefix,'') || ' ' || coalesce(%1.kunya,'') || ' ' || %1.name))),\"'\",''), '%2', ''), '%3', ''), '  ', ' ' ) AS %4").arg(var).arg( QChar(8217) ).arg( QChar(8216) ).arg(fieldName)
#define NUMERIC_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 1").arg( NAME_FIELD("i", KEY_ARG_1) ).arg(TOTAL_COUNT_VALUE).arg(field)
#define ORDERED_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 AND hidden ISNULL ORDER BY RANDOM() LIMIT 4").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(KEY_SORT_ORDER).arg(field)
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

IlmHelper::IlmHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void IlmHelper::boolSurahLocation(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    m_sql->executeQuery(caller, QString("SELECT %1,type FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id=%2").arg(MERGE_SURAH_NAME).arg(surahId), QueryId::BoolSurahLocation);
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


void IlmHelper::customStandardQuestion(QObject* caller) {
    fetchCustomColumn(caller, "standard_body", QueryId::CustomStandardQuestion);
}

void IlmHelper::customAfterQuestion(QObject* caller) {
    fetchCustomColumn(caller, "after_body", QueryId::CustomAfterQuestion);
}

void IlmHelper::customBeforeQuestion(QObject* caller) {
    fetchCustomColumn(caller, "before_body", QueryId::CustomBeforeQuestion);
}

void IlmHelper::customBoolCountQuestion(QObject* caller) {
    fetchCustomColumn(caller, "bool_count_body", QueryId::CustomBoolCountQuestion);
}

void IlmHelper::customBoolStandardQuestion(QObject* caller) {
    fetchCustomColumn(caller, "bool_standard_body", QueryId::CustomBoolStandardQuestion);
}

void IlmHelper::customCountQuestion(QObject* caller) {
    fetchCustomColumn(caller, "count_body", QueryId::CustomCountQuestion);
}

void IlmHelper::customOrderedQuestion(QObject* caller) {
    fetchCustomColumn(caller, "ordered_body", QueryId::CustomOrderedQuestion);
}

void IlmHelper::customPromptCountQuestion(QObject* caller) {
    fetchCustomColumn(caller, "prompt_count_body", QueryId::CustomPromptCountQuestion);
}

void IlmHelper::customPromptStandardQuestion(QObject* caller) {
    fetchCustomColumn(caller, "prompt_standard_body", QueryId::CustomPromptStandardQuestion);
}

void IlmHelper::fetchCustomColumn(QObject* caller, QString const& column, QueryId::Type t) {
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT q.id,q.%1 AS %2,sp1.heading AS heading1,sp2.heading AS heading2,q.source_id FROM questions q LEFT JOIN questions p ON q.source_id=p.id LEFT JOIN suite_pages sp1 ON q.suite_page_id=sp1.id LEFT JOIN suite_pages sp2 ON p.suite_page_id=sp2.id WHERE q.%1 NOT NULL) ORDER BY RANDOM() LIMIT 1").arg(column).arg(KEY_ARG_1), t);
}

void IlmHelper::answersForCustomBoolCountQuestion(QObject* caller, int questionId) {
    answersForCustomCountQuestion(caller, questionId, QueryId::AnswersForCustomBoolCountQuestion);
}

void IlmHelper::answersForCustomPromptCountQuestion(QObject* caller, int questionId) {
    answersForCustomCountQuestion(caller, questionId, QueryId::AnswersForCustomPromptCountQuestion);
}

void IlmHelper::fetchRightOrWrong(QObject* caller, int questionId, QueryId::Type t, bool correctOnly) {
    m_sql->executeQuery(caller, QString("SELECT choices.id AS %2,value_text AS %3,correct AS %4,%5 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 AND correct %6 UNION SELECT choices.id,choices.value_text,NULL,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1 AND correct %6) ORDER BY source_id").arg(questionId).arg(KEY_ID_FIELD).arg(KEY_CHOICE_VALUE).arg(KEY_FLAG_CORRECT).arg(KEY_SOURCE_ID).arg(correctOnly ? "NOT NULL" : "ISNULL"), t);
}

void IlmHelper::answersForCustomBoolStandardQuestion(QObject* caller, int questionId, bool correct) {
    fetchRightOrWrong(caller, questionId, QueryId::AnswersForCustomBoolStandardQuestion, correct);
}

void IlmHelper::answersForCustomPromptStandardQuestion(QObject* caller, int questionId, bool correct) {
    fetchRightOrWrong(caller, questionId, QueryId::AnswersForCustomPromptStandardQuestion, correct);
}

void IlmHelper::answersForCustomStandardQuestion(QObject* caller, int questionId) {
    m_sql->executeQuery(caller, QString("SELECT choices.id AS %2,value_text AS %3,correct AS %4,%5 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 UNION SELECT choices.id,choices.value_text,NULL,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1) ORDER BY source_id").arg(questionId).arg(KEY_ID_FIELD).arg(KEY_CHOICE_VALUE).arg(KEY_FLAG_CORRECT).arg(KEY_SOURCE_ID), QueryId::AnswersForCustomStandardQuestion);
}

void IlmHelper::answersForCustomOrderedQuestion(QObject* caller, int questionId)
{
    int limit = TextUtils::randInt(2,8);
    m_sql->executeQuery(caller, QString("SELECT choices.id AS %5,value_text AS %3,sort_order AS %4,%6 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 AND correct=1 UNION SELECT choices.id,choices.value_text AS %3,NULL AS %5,%6 FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1 AND correct=1) ORDER BY source_id LIMIT %2").arg(questionId).arg(limit).arg(KEY_CHOICE_VALUE).arg(KEY_SORT_ORDER).arg(KEY_ID_FIELD).arg(KEY_SOURCE_ID), QueryId::AnswersForCustomOrderedQuestion);
}

void IlmHelper::answersForCustomCountQuestion(QObject* caller, int questionId, QueryId::Type t) {
    m_sql->executeQuery(caller, QString("SELECT COUNT() AS %2 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 AND correct=1").arg(questionId).arg(TOTAL_COUNT_VALUE), t);
}

void IlmHelper::fetchSurahHeader(QObject* caller, int chapterNumber)
{
    LOGGER(chapterNumber);
    m_sql->executeQuery( caller, QString("SELECT %2 FROM surahs s INNER JOIN chapters c ON s.id=c.id WHERE s.id=%1").arg(chapterNumber).arg(MERGE_SURAH_NAME), QueryId::TempArgument1 );
}

void IlmHelper::numericMaxVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT MAX(verse_count) AS %1 FROM surahs").arg(TOTAL_COUNT_VALUE), QueryId::NumericMaxVerseCount);
}

void IlmHelper::numericMinVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT MIN(verse_count) AS %1 FROM surahs").arg(TOTAL_COUNT_VALUE), QueryId::NumericMinVerseCount);
}


void IlmHelper::numericTotalAyatCount(QObject* caller) {
    m_sql->executeQuery(caller, FETCH_TABLE_COUNT("ayahs"), QueryId::NumericTotalAyatCount);
}


void IlmHelper::numericTotalSurahCount(QObject* caller) {
    m_sql->executeQuery(caller, FETCH_TABLE_COUNT("surahs"), QueryId::NumericTotalSurahCount);
}


void IlmHelper::numericVerseCount(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT %1,verse_count AS total_count FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT 1").arg(MERGE_SURAH_NAME), QueryId::NumericVerseCount);
}


void IlmHelper::orderedSurahs(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.id AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahs);
}


void IlmHelper::orderedRevelationSurahs(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.revelation_order AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedRevelationSurahs);
}


void IlmHelper::orderedSurahsByLength(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.verse_count AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", "value") ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahsByLength);
}


void IlmHelper::orderedSurahVerses(QObject* caller)
{
    int surahId = RANDOM_SURAH;
    fetchSurahHeader(caller, surahId);
    m_sql->executeQuery(caller, QString("SELECT %1,%2,verse_id AS %3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%4 GROUP BY %3 ORDER BY RANDOM() LIMIT %5").arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(KEY_SORT_ORDER).arg(surahId).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahVerses);
}


void IlmHelper::standardSajdaSurah(QObject* caller)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(correctAnswerLimits > 1 ? 0 : 1, 4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM sajdas INNER JOIN surahs ON sajdas.surah_id=surahs.id INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %2").arg(MERGE_SURAH_VALUE).arg(correctAnswerLimits), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id NOT IN (SELECT surah_id FROM sajdas) ORDER BY RANDOM() LIMIT %2").arg(MERGE_SURAH_VALUE).arg(incorrectAnswerLimits), QueryId::StandardSajdaSurah);
}


void IlmHelper::standardVersesForSurah(QObject* caller)
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
    m_sql->executeQuery(caller, QString("SELECT %2,%3,1 AS correct FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 ORDER BY RANDOM() LIMIT %4").arg(chapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(2), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %2,%3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%1 ORDER BY RANDOM() LIMIT %4").arg(wrongChapter).arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(2), QueryId::StandardVersesForSurah);
}


void IlmHelper::lazyInit()
{
    QStringList languages = QStringList() << QUESTION_BANK("english") << DB_ARABIC << DB_ENGLISH;

    foreach (QString const& language, languages) {
        m_sql->attachIfNecessary(language);
    }
}


void IlmHelper::lookupByField(QObject* caller, int fieldValue, QueryId::Type t, QString const& field)
{
    int correctAnswerLimits = TextUtils::randInt(1,4);
    int incorrectAnswerLimits = TextUtils::randInt(correctAnswerLimits > 1 ? 0 : 1, 4-correctAnswerLimits);

    m_sql->executeQuery(caller, QString("SELECT %1,1 AS correct FROM individuals i WHERE %4=%3 AND hidden ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(correctAnswerLimits).arg(fieldValue).arg(field), QueryId::TempList);
    m_sql->executeQuery(caller, QString("SELECT %1 FROM individuals i WHERE %4 <> %3 AND hidden ISNULL AND prefix ISNULL ORDER BY RANDOM() LIMIT %2").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(incorrectAnswerLimits).arg(fieldValue).arg(field), t);
}

IlmHelper::~IlmHelper()
{
}

} /* namespace ilm */
