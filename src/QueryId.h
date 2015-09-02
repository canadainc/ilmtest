#ifndef QUERYID_H_
#define QUERYID_H_

#include <qobjectdefs.h>

namespace ilmtest {

class QueryId
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        BoolSurahLocation,
        NumericMaxVerseCount,
        NumericMinVerseCount,
        NumericTotalAyatCount,
        NumericTotalSurahCount,
        NumericVerseCount,
        OrderedRevelationSurahs,
        OrderedSurahs,
        OrderedSurahsByLength,
        OrderedSurahVerses,
        StandardSajdaSurah,
        StandardVersesForSurah,
        NumericBirthYearForIndividual,
        NumericDeathYearForIndividual,
        OrderedPeopleByBirth,
        OrderedPeopleByDeath,
        StandardSahabah,
        StandardTabiee,
        StandardTabiTabiee,
        TempArgument1,
        TempList
    };
};

}

#endif /* QUERYID_H_ */
