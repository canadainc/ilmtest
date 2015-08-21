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
        FetchRandomSurahs,
        FetchRandomVerseCount
    };
};

}

#endif /* QUERYID_H_ */
