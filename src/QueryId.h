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
        FetchMaxVerseCount,
        FetchMinVerseCount,
        FetchRandomRevelationSurahs,
        FetchRandomSajdaSurah,
        FetchRandomSurahLocation,
        FetchRandomSurahs,
        FetchRandomSurahsByLength,
        FetchRandomVerseCount,
        FetchSurahRandomVerses,
        FetchTotalAyatCount,
        FetchTotalSurahCount,
        FetchVersesForRandomSurah,
        PendingQuery,
        FetchSurahHeader
    };
};

}

#endif /* QUERYID_H_ */
