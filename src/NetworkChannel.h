#ifndef NETWORKCHANNEL_H_
#define NETWORKCHANNEL_H_

#include "NetworkProcessor.h"

namespace canadainc {
    class Persistance;
}

namespace ilmtest {

using namespace canadainc;

class NetworkChannel : public QObject
{
    Q_OBJECT

    NetworkProcessor m_network;
    Persistance* m_persist;
    QVariantMap m_response;
    QFutureWatcher<QVariantMap> m_extractor;

private slots:
    void onExtracted();
    void requestComplete(QVariant const& cookie, QByteArray const& data, bool error);

signals:
    void dbUpdateAvailable(qint64 dbSize, qint64 dbVersion);
    void questionBankUpdated();
    void transferProgress(QVariant const& cookie, qint64 bytesSent, qint64 bytesTotal);
    void updateCheckComplete();

public:
    NetworkChannel(Persistance* persist);
    virtual ~NetworkChannel();

    Q_SLOT void checkForUpdates();
    Q_SLOT void downloadLatestDatabase();
    void lazyInit();
};

} /* namespace ilmtest */

#endif /* NETWORKCHANNEL_H_ */
