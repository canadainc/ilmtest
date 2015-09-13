#ifndef ILMHELPER_H_
#define ILMHELPER_H_

#include <QObject>

#include "QueryId.h"

#define QUESTION_BANK(lang) QString("ilmtest_%1").arg(lang)

namespace canadainc {
    class DatabaseHelper;
}

namespace ilmtest {

using namespace canadainc;

class IlmHelper : public QObject
{
    Q_OBJECT

    DatabaseHelper* m_sql;

    void fetchCustomColumn(QObject* caller, QString const& column, QueryId::Type t, int questionId=0);
    void fetchOrderedChoices(QObject* caller, int questionId, QueryId::Type t);
    void fetchRightOrWrong(QObject* caller, int questionId, QueryId::Type t);
    void fetchSurahHeader(QObject* caller, int chapterNumber);
    void lookupByField(QObject* caller, int fieldValue, QueryId::Type t, QString const& field="is_companion");

public:
    IlmHelper(DatabaseHelper* sql);
    virtual ~IlmHelper();

    Q_INVOKABLE void afterRevealedSurah(QObject* caller);
    Q_INVOKABLE void afterSurah(QObject* caller);
    Q_INVOKABLE void afterVerse(QObject* caller);
    Q_INVOKABLE void beforeRevealedSurah(QObject* caller);
    Q_INVOKABLE void beforeSurah(QObject* caller);
    Q_INVOKABLE void beforeVerse(QObject* caller);
    Q_INVOKABLE void boolSurahLocation(QObject* caller);
    Q_INVOKABLE void customAfterQuestion(QObject* caller);
    Q_INVOKABLE void customBeforeQuestion(QObject* caller);
    Q_INVOKABLE void customBoolCountQuestion(QObject* caller);
    Q_INVOKABLE void customBoolStandardQuestion(QObject* caller);
    Q_INVOKABLE void customCountQuestion(QObject* caller);
    Q_INVOKABLE void customOrderedQuestion(QObject* caller);
    Q_INVOKABLE void customPromptCountQuestion(QObject* caller);
    Q_INVOKABLE void customPromptStandardQuestion(QObject* caller);
    Q_INVOKABLE void customStandardQuestion(QObject* caller);
    Q_INVOKABLE void answersForCustomAfterQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomBeforeQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomBoolCountQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomBoolStandardQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomCountQuestion(QObject* caller, int questionId, QueryId::Type t=QueryId::AnswersForCustomCountQuestion);
    Q_INVOKABLE void answersForCustomOrderedQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomPromptCountQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomPromptStandardQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void answersForCustomStandardQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void numericBirthYearForIndividual(QObject* caller);
    Q_INVOKABLE void numericDeathYearForIndividual(QObject* caller);
    Q_INVOKABLE void numericMaxVerseCount(QObject* caller);
    Q_INVOKABLE void numericMinVerseCount(QObject* caller);
    Q_INVOKABLE void numericTotalAyatCount(QObject* caller);
    Q_INVOKABLE void numericTotalSurahCount(QObject* caller);
    Q_INVOKABLE void numericVerseCount(QObject* caller);
    Q_INVOKABLE void orderedPeopleByBirth(QObject* caller);
    Q_INVOKABLE void orderedPeopleByDeath(QObject* caller);
    Q_INVOKABLE void orderedRevelationSurahs(QObject* caller, QueryId::Type t=QueryId::OrderedRevelationSurahs);
    Q_INVOKABLE void orderedSurahs(QObject* caller, QueryId::Type t=QueryId::OrderedSurahs);
    Q_INVOKABLE void orderedSurahsByLength(QObject* caller);
    Q_INVOKABLE void orderedSurahVerses(QObject* caller, QueryId::Type t=QueryId::OrderedSurahVerses);
    Q_INVOKABLE void standardFemale(QObject* caller);
    Q_INVOKABLE void standardSahabah(QObject* caller);
    Q_INVOKABLE void standardSajdaSurah(QObject* caller);
    Q_INVOKABLE void standardTabiee(QObject* caller);
    Q_INVOKABLE void standardTabiTabiee(QObject* caller);
    Q_INVOKABLE void standardVersesForSurah(QObject* caller);

    void lazyInit();
    void markVisited(qint64 questionId);
    void resetVisited();
    void reloadQuestionBank();
};

} /* namespace ilm */

#endif /* ILMHELPER_H_ */
