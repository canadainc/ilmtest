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


QVariantMap ThreadUtils::writePluginArchive(QVariantMap const& cookie, QByteArray const& data, QString const& pathKey)
{
    QVariantMap q = cookie;
    /*QString filePath = q.value(pathKey).toString();
    QString target = QString("%1/%2.zip").arg( QDir::tempPath() ).arg(filePath);
    QString expectedMd5 = q.value(KEY_MD5).toString();

    bool valid = IOUtils::writeIfValidMd5(target, expectedMd5, data);

    if (valid)
    {
        QStringList files = JlCompress::extractDir( target, QDir::homePath(), q.value(KEY_ARCHIVE_PASSWORD).toString().toStdString().c_str() );

        if ( !files.isEmpty() ) {
            return q;
        }
    }

    q[KEY_ERROR] = true; */

    return q;
}


} /* namespace quran */
