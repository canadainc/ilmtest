#ifndef ILMHELPER_H_
#define ILMHELPER_H_

#include <QObject>

#include "QueryId.h"

#define QUESTION_BANK(lang) QString("quran_tafsir_%1").arg(lang)

namespace canadainc {
    class DatabaseHelper;
}

namespace ilmtest {

using namespace canadainc;

class IlmHelper : public QObject
{
    Q_OBJECT

    DatabaseHelper* m_sql;

    void lookupByCompanionField(QObject* caller, int fieldValue, QueryId::Type t);

public:
    IlmHelper(DatabaseHelper* sql);
    virtual ~IlmHelper();

    Q_INVOKABLE void numericBirthYearForIndividual(QObject* caller);
    Q_INVOKABLE void numericDeathYearForIndividual(QObject* caller);
    Q_INVOKABLE void orderedPeopleByBirth(QObject* caller);
    Q_INVOKABLE void orderedPeopleByDeath(QObject* caller);
    Q_INVOKABLE void standardSahabah(QObject* caller);
    Q_INVOKABLE void standardTabiee(QObject* caller);
    Q_INVOKABLE void standardTabiTabiee(QObject* caller);

    Q_INVOKABLE void customQuestion(QObject* caller);
    Q_INVOKABLE void getChoicesForCustomQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void getOrderedChoicesForCustomQuestion(QObject* caller, int questionId);
    Q_INVOKABLE void getCorrectCountForCustomQuestion(QObject* caller, int questionId);

    void lazyInit();
};

} /* namespace ilm */

#endif /* ILMHELPER_H_ */
