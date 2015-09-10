#include "precompiled.h"

#include "InvokeHelper.h"
#include "AppLogFetcher.h"
#include "CardUtils.h"
#include "Logger.h"
#include "Persistance.h"
#include "QueryId.h"
#include "ThreadUtils.h"

namespace ilmtest {

using namespace bb::cascades;
using namespace bb::system;
using namespace canadainc;

InvokeHelper::InvokeHelper(InvokeManager* invokeManager) :
        m_root(NULL), m_invokeManager(invokeManager)
{
}


void InvokeHelper::init(QString const& qmlDoc, QMap<QString, QObject*> const& context, QObject* parent)
{
    qmlRegisterUncreatableType<QueryId>("com.canadainc.data", 1, 0, "QueryId", "Can't instantiate");

    QmlDocument* qml = QmlDocument::create("asset:///GlobalProperties.qml").parent(this);
    QObject* global = qml->createRootObject<QObject>();
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("global", global);

    m_root = CardUtils::initAppropriate(qmlDoc, context, parent);
}


QString InvokeHelper::invoked(bb::system::InvokeRequest const& request)
{
    LOGGER( request.action() << request.target() << request.mimeType() << request.metadata() << request.uri().toString() << QString( request.data() ) );

    QString target = request.target();

    QMap<QString,QString> targetToQML;
    targetToQML[TARGET_SHARE] = "PlaceholderPage.qml";

    QString qml = targetToQML.value(target);

    if ( qml.isNull() ) {
        qml = "PlaceholderPage.qml";
    }

    m_request = request;
    m_request.setTarget(target);

    return qml;
}


void InvokeHelper::process()
{
    QString target = m_request.target();

    if ( !target.isEmpty() )
    {
        if (target == TARGET_SHARE)
        {
            QFutureWatcher<bool>* qfw = new QFutureWatcher<bool>(this);
            connect( qfw, SIGNAL( finished() ), this, SLOT( onDatabasePorted() ) );
            QFuture<bool> future = QtConcurrent::run( &ThreadUtils::replaceDatabase, m_request.uri().toLocalFile() );
            qfw->setFuture(future);
        }
    }
}


void InvokeHelper::onDatabasePorted()
{
    m_root->setProperty("isBusy", false);

    QFutureWatcher<bool>* qfw = static_cast< QFutureWatcher<bool>* >( sender() );
    bool copied = qfw->result();

    LOGGER(copied);

    qfw->deleteLater();

    Persistance::showBlockingDialog( tr("IlmTest"), copied ? tr("Database ported successfully!") : tr("Database could not be copied!"), tr("OK"), "" );
    m_invokeManager->sendCardDone( CardDoneMessage() );
}


InvokeHelper::~InvokeHelper()
{
}

} /* namespace admin */
