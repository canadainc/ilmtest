#ifndef COMMONCONSTANTS_H_
#define COMMONCONSTANTS_H_

#include <QUrl>

#define KEY_ANSWER "answer"
#define KEY_ARG_1 "arg1"
#define KEY_CHOICE_VALUE "value"
#define KEY_FLAG_CORRECT "correct"
#define KEY_ID_FIELD "id"
#define KEY_SORT_ORDER "sort_order"
#define KEY_SOURCE_ID "source_id"
#define REAL_ID(x) x[KEY_SOURCE_ID].toInt() > 0 ? x[KEY_SOURCE_ID].toInt() : x[KEY_ID_FIELD].toInt()
#define TOTAL_COUNT_VALUE "total_count"

namespace admin {

struct CommonConstants
{
    static QUrl generateHostUrl(QString const& path=QString());
};

}

#endif /* COMMONCONSTANTS_H_ */
