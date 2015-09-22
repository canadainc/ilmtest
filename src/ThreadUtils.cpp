#include "precompiled.h"

#include "ThreadUtils.h"
#include "CommonConstants.h"
#include "IOUtils.h"
#include "JlCompress.h"
#include "Logger.h"

namespace ilmtest {

using namespace bb::cascades;
using namespace canadainc;

void ThreadUtils::compressFiles(Report& r, QString const& zipPath, const char* password) {
    JlCompress::compressFiles(zipPath, r.attachments, password);
}


bool ThreadUtils::replaceDatabase(QString const& src)
{
    QFileInfo qfi(src);
    QString dest = QString("%1/%2").arg( QDir::homePath() ).arg( qfi.fileName() );

    if ( QFile::exists(dest) ) {
        LOGGER("Removing" << dest);
        LOGGER( QFile::remove(dest) );
    }

    LOGGER(src << dest);

    return QFile::copy(src, dest);
}

} /* namespace quran */
