#ifndef COMMONCONSTANTS_H_
#define COMMONCONSTANTS_H_

#include <QUrl>

namespace admin {

struct CommonConstants
{
    static QUrl generateHostUrl(QString const& path=QString());
};

}

#endif /* COMMONCONSTANTS_H_ */
