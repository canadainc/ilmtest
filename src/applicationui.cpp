#include "precompiled.h"

#include "applicationui.hpp"
#include "AppLogFetcher.h" // needed for constants only
#include "CommonConstants.h"
#include "IOUtils.h"
#include "JlCompress.h"
#include "Logger.h"
#include "ThreadUtils.h"

using namespace bb::cascades;

namespace ilmtest {

using namespace bb::system;
using namespace canadainc;

ApplicationUI::ApplicationUI(bb::system::InvokeManager* im) : m_sql( QString("%1/master.db").arg( QDir::homePath() ) ),
        m_persistance(im),
        m_quran(&m_sql), m_invoke(im)
{
    switch ( im->startupMode() )
    {
        case ApplicationStartupMode::LaunchApplication:
            init("main.qml");
            break;

        case ApplicationStartupMode::InvokeCard:
            connect( im, SIGNAL( cardPooled(bb::system::CardDoneMessage const&) ), QCoreApplication::instance(), SLOT( quit() ) );
            connect( im, SIGNAL( invoked(bb::system::InvokeRequest const&) ), this, SLOT( invoked(bb::system::InvokeRequest const&) ) );
            break;
        case ApplicationStartupMode::InvokeApplication:
            connect( im, SIGNAL( invoked(bb::system::InvokeRequest const&) ), this, SLOT( invoked(bb::system::InvokeRequest const&) ) );
            break;

        default:
            break;
    }

    connect( im, SIGNAL( childCardDone(bb::system::CardDoneMessage const&) ), this, SLOT( childCardDone(bb::system::CardDoneMessage const&) ) );
}


void ApplicationUI::invoked(bb::system::InvokeRequest const& request) {
    init( m_invoke.invoked(request) );
}


void ApplicationUI::init(QString const& qmlDoc)
{
    QMap<QString, QObject*> context;
    context["quran"] = &m_quran;
    context["offloader"] = &m_offloader;

    setErrorHandler(&Persistance::onErrorMessage);

    m_invoke.init(qmlDoc, context, this);

    emit initialize();
}


void ApplicationUI::childCardDone(bb::system::CardDoneMessage const& message)
{
    LOGGER( message.data() );
    emit childCardFinished( message.data(), message.reason().split("/").last() );

    if ( !message.data().isEmpty() ) {
        m_persistance.invokeManager()->sendCardDone(message);
    }
}


void ApplicationUI::onRequestComplete(QVariant const& cookie, QByteArray const& data, bool error)
{
    Q_UNUSED(data);
    LOGGER(cookie << error);
}


void ApplicationUI::lazyInit()
{
    disconnect( this, SIGNAL( initialize() ), this, SLOT( lazyInit() ) ); // in case we get invoked again

    connect( &m_network, SIGNAL( requestComplete(QVariant const&, QByteArray const&, bool) ), this, SLOT( onRequestComplete(QVariant const&, QByteArray const&, bool) ) );
    connect( &m_network, SIGNAL( downloadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( transferProgress(QVariant const&, qint64, qint64) ) );
    connect( &m_network, SIGNAL( uploadProgress(QVariant const&, qint64, qint64) ), this, SIGNAL( transferProgress(QVariant const&, qint64, qint64) ) );

    m_sql.createDatabaseIfNotExists();

    m_quran.lazyInit();

    m_invoke.process();

    AppLogFetcher::create( &m_persistance, &ThreadUtils::compressFiles, this, false );
    connect( &m_sql, SIGNAL( error(QString const&) ), &m_persistance, SLOT( onError(QString const&) ) );
    connect( &m_sql, SIGNAL( setupError(QString const&) ), &m_persistance, SLOT( onError(QString const&) ) );

    emit lazyInitComplete();
}


}
