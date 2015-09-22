#ifndef THREADUTILS_H_
#define THREADUTILS_H_

#include <QVariant>

#include "Report.h"

namespace ilmtest {

struct ThreadUtils
{
    static void compressFiles(canadainc::Report& r, QString const& zipPath, const char* password);
    static bool replaceDatabase(QString const& src);
};

} /* namespace quran */

#endif /* THREADUTILS_H_ */
