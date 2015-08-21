#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "DatabaseHelper.h"
#include "InvokeHelper.h"
#include "NetworkProcessor.h"
#include "Offloader.h"
#include "Persistance.h"
#include "QuranHelper.h"
#include "TextUtils.h"

#include <bb/system/CardDoneMessage>

namespace ilmtest {

using namespace canadainc;

class ApplicationUI : public QObject
{
    Q_OBJECT

    DatabaseHelper m_sql;
    Persistance m_persistance;
    NetworkProcessor m_network;
    quran::QuranHelper m_quran;
    InvokeHelper m_invoke;
    Offloader m_offloader;

    void init(QString const& qml);

signals:
    void childCardFinished(QString const& message, QString const& cookie);
    void initialize();
    void lazyInitComplete();
    void transferProgress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);

private slots:
    void childCardDone(bb::system::CardDoneMessage const& message=bb::system::CardDoneMessage());
    void invoked(bb::system::InvokeRequest const& request);
    void lazyInit();
    void onRequestComplete(QVariant const& cookie, QByteArray const& data, bool error);

public:
    ApplicationUI(bb::system::InvokeManager* im);
    virtual ~ApplicationUI() {}
};

}

#endif /* ApplicationUI_HPP_ */
