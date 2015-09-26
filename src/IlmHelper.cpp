#include "precompiled.h"

#include "IlmHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"
#include "SharedConstants.h"
#include "TextUtils.h"

#define DB_ARABIC "quran_arabic"
#define DB_ENGLISH "quran_english"
#define FETCH_TABLE_COUNT(table) QString("SELECT COUNT() AS %1 FROM %2").arg(TOTAL_COUNT_VALUE).arg(table)
#define NUMERIC_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 ORDER BY RANDOM() LIMIT 1").arg( NAME_FIELD("i", KEY_ARG_1) ).arg(TOTAL_COUNT_VALUE).arg(field)
#define ORDERED_FIELD_QUERY(field) QString("SELECT %1,%3 AS %2 FROM individuals i WHERE %3 > 0 ORDER BY RANDOM() LIMIT 4").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(KEY_SORT_ORDER).arg(field)
#define MERGE_COLUMNS(col1, col2, alias) QString("%1 || ' (' || %2 || ')' AS %3").arg(col1).arg(col2).arg(alias)
#define MERGE_SURAH_NAME MERGE_COLUMNS("name", "transliteration", KEY_ARG_1)
#define MERGE_SURAH_VALUE MERGE_COLUMNS("name", "transliteration", KEY_CHOICE_VALUE)
#define SURAH_AS_VALUE QString("name AS %1").arg(KEY_CHOICE_VALUE)
#define AYAT_AS_VALUE QString("content AS %1").arg(KEY_CHOICE_VALUE)
#define TRANSLATION_AS_DESCRIPTION "translation AS description"
#define RANDOM_SURAH TextUtils::randInt(1,114)
#define USE_RANDOM_CONSTRAINT(q,t,id) if (t == id) q += QString(" ORDER BY RANDOM() LIMIT %1").arg(RESULT_SET_LIMIT);

namespace {

QPair<int,int> generateCorrectIncorrect()
{
    int correctAnswerLimits = canadainc::TextUtils::randInt(1,RESULT_SET_MAX);
    int incorrectAnswerLimits = canadainc::TextUtils::randInt(correctAnswerLimits > 1 ? 0 : 1, RESULT_SET_MAX-correctAnswerLimits);
    LOGGER(correctAnswerLimits << incorrectAnswerLimits);

    return qMakePair<int,int>(correctAnswerLimits, incorrectAnswerLimits);
}

}

namespace ilmtest {

using namespace canadainc;

IlmHelper::IlmHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void IlmHelper::afterRevealedSurah(QObject* caller) {
    orderedRevelationSurahs(caller, QueryId::AfterRevealedSurah);
}


void IlmHelper::afterSurah(QObject* caller) {
    orderedSurahs(caller, QueryId::AfterSurah);
}


void IlmHelper::afterVerse(QObject* caller) {
    orderedSurahVerses(caller, QueryId::AfterVerse);
}


void IlmHelper::beforeRevealedSurah(QObject* caller) {
    orderedRevelationSurahs(caller, QueryId::BeforeRevealedSurah);
}


void IlmHelper::beforeSurah(QObject* caller) {
    orderedSurahs(caller, QueryId::BeforeSurah);
}


void IlmHelper::beforeVerse(QObject* caller) {
    orderedSurahVerses(caller, QueryId::AfterSurah);
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


void IlmHelper::standardBook(QObject* caller) {
    Q_UNUSED(caller);
}


void IlmHelper::standardFemale(QObject* caller)
{
    QPair<int,int> limits = generateCorrectIncorrect();
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT %1,1 AS correct FROM individuals i WHERE female=1 AND death > 0 ORDER BY RANDOM() LIMIT %2) UNION SELECT * FROM (SELECT %1,0 FROM individuals i WHERE female ISNULL AND prefix ISNULL ORDER BY RANDOM() LIMIT %3)").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(limits.first).arg(limits.second), QueryId::StandardFemale);
}


void IlmHelper::standardTabiTabiee(QObject* caller) {
    lookupByField(caller, 3, QueryId::StandardTabiTabiee);
}

void IlmHelper::fetchCustomColumn(QObject* caller, QueryId::Type t, qint64 questionId)
{
    QString column = m_typeToTable.value(t);

    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT q.id,q.%1 AS %2,q.suite_page_id AS spi1,p.suite_page_id AS spi2,s1.title AS title1,s2.title AS title2,sp1.heading AS heading1,sp2.heading AS heading2,q.source_id,%4,%5 FROM questions q LEFT JOIN questions p ON q.source_id=p.id LEFT JOIN suite_pages sp1 ON q.suite_page_id=sp1.id LEFT JOIN suite_pages sp2 ON p.suite_page_id=sp2.id LEFT JOIN suites s1 ON s1.id=sp1.suite_id LEFT JOIN suites s2 ON s2.id=sp2.suite_id LEFT JOIN individuals i1 ON s1.author=i1.id LEFT JOIN individuals i2 ON s2.author=i2.id WHERE q.%1 NOT NULL AND q.id=%3) ORDER BY RANDOM() LIMIT 1").arg(column).arg(KEY_ARG_1).arg(questionId).arg( NAME_FIELD("i1", "author1") ).arg( NAME_FIELD("i2", "author2") ), t);
}

void IlmHelper::answersForCustomBoolCountQuestion(QObject* caller, int questionId) {
    answersForCustomCountQuestion(caller, questionId, QueryId::AnswersForCustomBoolCountQuestion);
}

void IlmHelper::answersForCustomPromptCountQuestion(QObject* caller, int questionId) {
    answersForCustomCountQuestion(caller, questionId, QueryId::AnswersForCustomPromptCountQuestion);
}

void IlmHelper::fetchRightOrWrong(QObject* caller, int questionId, QueryId::Type t) {
    m_sql->executeQuery(caller, QString("SELECT choices.id AS %2,value_text AS %3,correct AS %4,%5 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 UNION SELECT choices.id,choices.value_text,NULL,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1) ORDER BY source_id").arg(questionId).arg(KEY_ID_FIELD).arg(KEY_CHOICE_VALUE).arg(KEY_FLAG_CORRECT).arg(KEY_SOURCE_ID), t);
}

void IlmHelper::answersForCustomAfterQuestion(QObject* caller, int questionId) {
    fetchOrderedChoices(caller, questionId, QueryId::AnswersForCustomAfterQuestion);
}

void IlmHelper::answersForCustomBeforeQuestion(QObject* caller, int questionId) {
    fetchOrderedChoices(caller, questionId, QueryId::AnswersForCustomBeforeQuestion);
}

void IlmHelper::answersForCustomBoolStandardQuestion(QObject* caller, int questionId) {
    fetchRightOrWrong(caller, questionId, QueryId::AnswersForCustomBoolStandardQuestion);
}

void IlmHelper::answersForCustomPromptStandardQuestion(QObject* caller, int questionId) {
    fetchRightOrWrong(caller, questionId, QueryId::AnswersForCustomPromptStandardQuestion);
}

void IlmHelper::answersForCustomStandardQuestion(QObject* caller, int questionId, QueryId::Type t) {
    m_sql->executeQuery(caller, QString("SELECT answers.id AS answer_id,choices.id AS %2,value_text AS %3,correct AS %4,%5 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%1 UNION SELECT 0,choices.id,choices.value_text,NULL,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%1) ORDER BY source_id").arg(questionId).arg(KEY_ID_FIELD).arg(KEY_CHOICE_VALUE).arg(KEY_FLAG_CORRECT).arg(KEY_SOURCE_ID), t);
}

void IlmHelper::fetchOrderedChoices(QObject* caller, int questionId, QueryId::Type t) {
    m_sql->executeQuery(caller, QString("SELECT choices.id AS %1,value_text AS %2,sort_order AS %3,source_id AS %4 FROM answers INNER JOIN choices ON answers.choice_id=choices.id WHERE question_id=%5 AND sort_order NOT NULL UNION SELECT choices.id,choices.value_text,NULL,source_id FROM choices WHERE source_id IN (SELECT choice_id FROM answers WHERE question_id=%5 AND sort_order NOT NULL) ORDER BY %4").arg(KEY_ID_FIELD).arg(KEY_CHOICE_VALUE).arg(KEY_SORT_ORDER).arg(KEY_SOURCE_ID).arg(questionId), t);
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


void IlmHelper::orderedSurahs(QObject* caller, QueryId::Type t)
{
    QString query = QString("SELECT surahs.id AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id").arg( MERGE_COLUMNS("name", "transliteration", KEY_CHOICE_VALUE) ).arg(KEY_SORT_ORDER);
    USE_RANDOM_CONSTRAINT(query, t, QueryId::OrderedSurahs);

    m_sql->executeQuery(caller, query, t);
}


void IlmHelper::orderedRevelationSurahs(QObject* caller, QueryId::Type t)
{
    QString query = QString("SELECT surahs.revelation_order AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id").arg( MERGE_COLUMNS("name", "transliteration", KEY_CHOICE_VALUE) ).arg(KEY_SORT_ORDER);
    USE_RANDOM_CONSTRAINT(query, t, QueryId::OrderedRevelationSurahs);

    m_sql->executeQuery(caller, query, t);
}


void IlmHelper::orderedSurahsByLength(QObject* caller) {
    m_sql->executeQuery(caller, QString("SELECT surahs.verse_count AS %2,%1 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %4").arg( MERGE_COLUMNS("name", "transliteration", KEY_CHOICE_VALUE) ).arg(KEY_SORT_ORDER).arg(RESULT_SET_LIMIT), QueryId::OrderedSurahsByLength);
}


void IlmHelper::orderedSurahVerses(QObject* caller, QueryId::Type t)
{
    int surahId = RANDOM_SURAH;
    fetchSurahHeader(caller, surahId);

    QString query = QString("SELECT %1,%2,verse_id AS %3 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%4 GROUP BY %3").arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(KEY_SORT_ORDER).arg(surahId);
    USE_RANDOM_CONSTRAINT(query, t, QueryId::OrderedSurahVerses);

    m_sql->executeQuery(caller, query, t);
}


void IlmHelper::standardSajdaSurah(QObject* caller)
{
    QPair<int,int> limits = generateCorrectIncorrect();
    m_sql->executeQuery( caller, QString("SELECT * FROM (SELECT %1,1 AS correct FROM sajdas INNER JOIN surahs ON sajdas.surah_id=surahs.id INNER JOIN chapters ON surahs.id=chapters.id ORDER BY RANDOM() LIMIT %2) UNION SELECT * FROM (SELECT %1,0 FROM surahs INNER JOIN chapters ON surahs.id=chapters.id WHERE surahs.id NOT IN (SELECT surah_id FROM sajdas) ORDER BY RANDOM() LIMIT %3)").arg(MERGE_SURAH_VALUE).arg(limits.first).arg(limits.second), QueryId::StandardSajdaSurah );
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
    QPair<int,int> limits = generateCorrectIncorrect();
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT %1,%2,1 AS correct FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%3 ORDER BY RANDOM() LIMIT %5) UNION SELECT * FROM (SELECT %1,%2,0 FROM ayahs INNER JOIN verses ON ayahs.surah_id=verses.chapter_id WHERE surah_id=%4 ORDER BY RANDOM() LIMIT %6)").arg(AYAT_AS_VALUE).arg(TRANSLATION_AS_DESCRIPTION).arg(chapter).arg(wrongChapter).arg(limits.first).arg(limits.second), QueryId::StandardVersesForSurah);
}


void IlmHelper::standardTeacher(QObject* caller) {
    lookupByRelation(caller, "teachers", "individual", "teacher", QueryId::StandardTeacher);
}


void IlmHelper::standardStudent(QObject* caller) {
    lookupByRelation(caller, "teachers", "teacher", "individual", QueryId::StandardStudent);
}


void IlmHelper::lookupByRelation(QObject* caller, QString const& table, QString const& fieldName, QString const& joinField, QueryId::Type t)
{
    QPair<int,int> limits = generateCorrectIncorrect();

    m_sql->startTransaction(caller, QueryId::Unknown);
    m_sql->executeQuery(caller, "CREATE TEMPORARY TABLE individuals_temp (id INTEGER PRIMARY KEY)", QueryId::Unknown);
    m_sql->executeQuery(caller, QString("INSERT INTO individuals_temp(id) SELECT %1 FROM %2 ORDER BY RANDOM() LIMIT 1").arg(fieldName).arg(table), QueryId::Unknown);

    QString tempLookup = "SELECT id FROM individuals_temp LIMIT 1";
    m_sql->executeQuery(caller, QString("SELECT %1 FROM individuals i WHERE i.id=(%2)").arg( NAME_FIELD("i", KEY_ARG_1) ).arg(tempLookup), QueryId::TempArgument1);
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT %1,1 AS %2 FROM %3 INNER JOIN individuals i ON %3.%4=i.id WHERE %3.%5=(%8) ORDER BY RANDOM() LIMIT %6) UNION SELECT * FROM (SELECT %1,0 AS %2 FROM individuals i WHERE id NOT IN (SELECT %4 FROM %3 WHERE %5=(%8)) ORDER BY RANDOM() LIMIT %7)").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(KEY_FLAG_CORRECT).arg(table).arg(joinField).arg(fieldName).arg(limits.first).arg(limits.second).arg(tempLookup), t);
    m_sql->executeQuery(caller, "DROP TABLE individuals_temp", QueryId::Unknown);
    m_sql->endTransaction(caller, QueryId::Unknown);
}


void IlmHelper::lazyInit()
{
    m_typeToTable[QueryId::CustomAfterQuestion] = "after_body";
    m_typeToTable[QueryId::CustomBeforeQuestion] = "before_body";
    m_typeToTable[QueryId::CustomBoolCountQuestion] = "bool_count_body";
    m_typeToTable[QueryId::CustomBoolStandardQuestion] = "bool_standard_body";
    m_typeToTable[QueryId::CustomCountQuestion] = "count_body";
    m_typeToTable[QueryId::CustomOrderedQuestion] = "ordered_body";
    m_typeToTable[QueryId::CustomPromptCountQuestion] = "prompt_count_body";
    m_typeToTable[QueryId::CustomPromptStandardQuestion] = "prompt_standard_body";
    m_typeToTable[QueryId::CustomStandardNegation] = "standard_negation_body";
    m_typeToTable[QueryId::CustomStandardQuestion] = "standard_body";

    if ( dbExists() ) {
        reloadQuestionBank();
    }
}


bool IlmHelper::dbExists()
{
    QFile f( QString("%1/%2.db").arg( QDir::homePath() ).arg( QUESTION_BANK("english") ) );
    return f.exists() && f.size() > 0;
}


void IlmHelper::lookupByField(QObject* caller, int fieldValue, QueryId::Type t, QString const& field)
{
    QPair<int,int> limits = generateCorrectIncorrect();
    m_sql->executeQuery(caller, QString("SELECT * FROM (SELECT %1,1 AS correct FROM individuals i WHERE %2=%3 ORDER BY RANDOM() LIMIT %4) UNION SELECT * FROM (SELECT %1,0 FROM individuals i WHERE %2 <> %3 AND prefix ISNULL ORDER BY RANDOM() LIMIT %5)").arg( NAME_FIELD("i", KEY_CHOICE_VALUE) ).arg(field).arg(fieldValue).arg(limits.first).arg(limits.second), t);
}


void IlmHelper::fetchDictionary(QObject* caller)
{
    QList<QueryId::Type> customTypes = m_typeToTable.keys();
    QStringList queries;

    foreach (QueryId::Type t, customTypes) {
        queries << QString("SELECT id,%1 AS %2 FROM questions WHERE %3 NOT NULL AND source_id ISNULL").arg(t).arg(FIELD_COLUMN_TYPE).arg( m_typeToTable.value(t) );
    }

    m_sql->executeQuery(caller, QString("SELECT * FROM (%1) ORDER BY RANDOM()").arg( queries.join(" UNION ") ), QueryId::FetchDictionary);
}


void IlmHelper::reloadQuestionBank()
{
    QStringList languages = QStringList() << DB_ARABIC << DB_ENGLISH;

    foreach (QString const& language, languages)
    {
        m_sql->detach(language);
        m_sql->attachIfNecessary(language);
    }

    languages.clear();
    languages << QUESTION_BANK("english");

    foreach (QString const& language, languages)
    {
        m_sql->detach(language);
        m_sql->attachIfNecessary(language, true);
    }
}


IlmHelper::~IlmHelper()
{
}

} /* namespace ilm */
