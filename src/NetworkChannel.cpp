#include "precompiled.h"

#include "NetworkChannel.h"
#include "AppLogFetcher.h"
#include "CommonConstants.h"
#include "IOUtils.h"
#include "JlCompress.h"
#include "Logger.h"
#include "Persistance.h"
#include "ReportGenerator.h"

#define ARCHIVE_PASSWORD "ILM_X___23333Txtx099"
#define COOKIE_FETCH_HEAD "fetch_head"
#define COOKIE_DLOAD_DB "dload_db"
#define KEY_ARCHIVE_NAME "archive_name"
#define KEY_DB_VERSION "dbVersion"
#define KEY_SERVER_DB_VERSION "version"
#define KEY_ERROR "error"
#define KEY_MD5 "md5"

namespace {

QVariantMap processDownload(QVariantMap const& cookie, QByteArray const& data)
{
    QVariantMap q = cookie;
    QString filePath = q.value(KEY_ARCHIVE_NAME).toString();
    QString target = QString("%1/%2").arg( QDir::tempPath() ).arg(filePath);
    QString expectedMd5 = q.value(KEY_MD5).toString();

    bool valid = canadainc::IOUtils::writeIfValidMd5(target, expectedMd5, data);

    if (valid)
    {
        QStringList files = JlCompress::extractDir( target, QDir::homePath(), ARCHIVE_PASSWORD );

        if ( !files.isEmpty() ) {
            return q;
        }
    }

    q[KEY_ERROR] = QObject::tr("Corrupted archive downloaded. Please re-try the download again.");

    return q;
}

}

namespace ilmtest {

using namespace canadainc;

NetworkChannel::NetworkChannel(Persistance* persist) : m_persist(persist)
{
}


void NetworkChannel::lazyInit()
{
    connect( &m_network, SIGNAL( requestComplete(QVariant const&, QByteArray const&, bool) ), this, SLOT( requestComplete(QVariant const&, QByteArray const&, bool) ) );
    connect( &m_network, SIGNAL( downloadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( transferProgress(QVariant const&, qint64, qint64) ) );
    connect( &m_network, SIGNAL( uploadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( transferProgress(QVariant const&, qint64, qint64) ) );
    connect( &m_extractor, SIGNAL( finished() ), this, SLOT( onExtracted() ) );
    connect( AppLogFetcher::getInstance(), SIGNAL( userIdFetched(QString const&) ), this, SLOT( onUserIdFetched(QString const&) ) );
}


void NetworkChannel::onUserIdFetched(QString const& userId)
{
    Q_UNUSED(userId);
    checkForUpdates();
}


void NetworkChannel::checkForUpdates()
{
    if ( m_persist->containsFlag(KEY_USER_ID) )
    {
        QUrl url = CommonConstants::generateHostUrl("fetch_head.php");
        url.addQueryItem( "user_id", m_persist->getFlag(KEY_USER_ID).toString() );
        url.addQueryItem( "language", "english" );

        QVariantMap qvm;
        qvm[COOKIE_FETCH_HEAD] = true;
        m_network.doGet(url, qvm);
    }
}


void NetworkChannel::downloadLatestDatabase()
{
    QString archiveName = m_response.value("path").toString();
    archiveName = archiveName.split("/").last();
    QUrl url = CommonConstants::generateHostUrl(archiveName);

    QVariantMap qvm;
    qvm[COOKIE_DLOAD_DB] = true;
    qvm[KEY_ARCHIVE_NAME] = archiveName;
    qvm[KEY_MD5] = m_response.value(KEY_MD5);
    qvm[KEY_DB_VERSION] = m_response.value(KEY_SERVER_DB_VERSION);
    m_network.doGet(url, qvm);
}


void NetworkChannel::onExtracted()
{
    QVariantMap result = m_extractor.result();

    if ( !result.contains(KEY_ERROR) )
    {
        qint64 dbVersion = result.value(KEY_DB_VERSION).toLongLong();
        m_persist->setFlag(KEY_DB_VERSION, dbVersion);

        m_persist->showToast( tr("Database updated!"), "images/toast/db_up_to_date.png" );

        emit questionBankUpdated();
    } else {
        m_persist->showToast( result.value(KEY_ERROR).toString(), "images/toast/ca_error.png" );
    }
}


void NetworkChannel::requestComplete(QVariant const& cookie, QByteArray const& data, bool error)
{
    QVariantMap qvm = cookie.toMap();

    if (!error)
    {
        if ( qvm.contains(COOKIE_FETCH_HEAD) )
        {
            QVariantMap actualResult = bb::data::JsonDataAccess().loadFromBuffer(data).toMap();
            error = actualResult.value("result").toString() != HTTP_RESPONSE_OK;

            if (!error)
            {
                m_response = actualResult;

                qint64 serverVersion = m_response.value(KEY_SERVER_DB_VERSION).toLongLong();
                qint64 myVersion = m_persist->getFlag(KEY_DB_VERSION).toLongLong();

                if (serverVersion > myVersion) {
                    emit dbUpdateAvailable( m_response.value("size").toLongLong(), serverVersion );
                } else {
                    m_persist->showToast( tr("You have the latest version of the database."), "images/toast/db_up_to_date.png" );
                }
            } else {
                LOGGER(error << qvm << QString(data));
                m_persist->showToast( tr("%1. Please try again.").arg( actualResult.value("debug").toString() ), "images/toast/ca_error.png" );
            }

            emit updateCheckComplete();
        } else if ( qvm.contains(COOKIE_DLOAD_DB) ) {
            QFuture<QVariantMap> future = QtConcurrent::run(&processDownload, cookie.toMap(), data);
            m_extractor.setFuture(future);
        }
    } else {
        LOGGER(error << QString(data));
        m_persist->showToast( tr("Request could not be completed due to a network error. Please try again."), "images/toast/ca_error.png" );
    }
}


NetworkChannel::~NetworkChannel()
{
}

} /* namespace ilmtest */
