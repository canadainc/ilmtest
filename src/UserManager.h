#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <QObject>

namespace canadainc {
    class Persistance;
}

namespace ilmtest {

using namespace canadainc;

class UserManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY profileChanged)
    Q_PROPERTY(QString kunya READ kunya NOTIFY profileChanged)
    Q_PROPERTY(bool female READ female NOTIFY profileChanged)
    Persistance* m_persist;

private slots:
    void onSettingChanged(QVariant newValue, QVariant key);

signals:
    void profileChanged();

public:
    UserManager(Persistance* persist);
    virtual ~UserManager();

    QString name();
    QString kunya();
    bool female();
};

} /* namespace canadainc */

#endif /* UserManager_H_ */
