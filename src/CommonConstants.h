#ifndef COMMONCONSTANTS_H_
#define COMMONCONSTANTS_H_

#include <QUrl>

#define KEY_FLAG_CORRECT "correct"
#define KEY_SORT_ORDER "sort_order"
#define KEY_ARG_1 "arg1"
#define TOTAL_COUNT_VALUE "total_count"

namespace admin {

struct CommonConstants
{
    static QUrl generateHostUrl(QString const& path=QString());
};

}

#endif /* COMMONCONSTANTS_H_ */
