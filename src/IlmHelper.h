#ifndef ILMHELPER_H_
#define ILMHELPER_H_

#include <QObject>

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

public:
    IlmHelper(DatabaseHelper* sql);
    virtual ~IlmHelper();

    Q_INVOKABLE void numericBirthYearForIndividual(QObject* caller);
    Q_INVOKABLE void numericDeathYearForIndividual(QObject* caller);
    Q_INVOKABLE void orderedPeopleByBirth(QObject* caller);
    Q_INVOKABLE void orderedPeopleByDeath(QObject* caller);
    Q_INVOKABLE void standardSahabah(QObject* caller);

    void lazyInit();
};

} /* namespace ilm */

#endif /* ILMHELPER_H_ */
