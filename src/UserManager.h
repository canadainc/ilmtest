#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <QObject>

namespace canadainc {
    class DatabaseHelper;
    class Persistance;
}

namespace bb {
    namespace cascades {
        class ArrayDataModel;
        class TextField;
    }
}

namespace ilmtest {

using namespace canadainc;

struct UserProfile
{
    qint64 id;
    QString name;
    QString kunya;
    bool female;
    int points;
    int maxLevel;
    int played;

    UserProfile();
};

class Game;

class UserManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY profileChanged)
    Q_PROPERTY(QString kunya READ kunya NOTIFY profileChanged)
    Q_PROPERTY(qint64 id READ id NOTIFY profileChanged)
    Q_PROPERTY(bool female READ female NOTIFY profileChanged)
    Q_PROPERTY(int points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(int highestLevel READ highestLevel WRITE setHighestLevel NOTIFY pointsChanged)
    Q_PROPERTY(int numPlayed READ numPlayed WRITE setNumPlayed NOTIFY pointsChanged)
    Q_PROPERTY(bool profileSet READ profileSet NOTIFY profileChanged)

    Persistance* m_persist;
    DatabaseHelper* m_db;
    UserProfile m_profile;
    Game* m_game;

private slots:
    void onDataLoaded(QVariant id, QVariant data);
    void onSettingChanged(QVariant newValue, QVariant key);

signals:
    void profileChanged();
    void pointsChanged();

public:
    UserManager(Persistance* persist, DatabaseHelper* db, Game* game);
    virtual ~UserManager();

    QString name() const;
    QString kunya() const;
    bool female() const;
    qint64 id() const;
    int points() const;
    void setPoints(int points);
    int highestLevel() const;
    void setHighestLevel(int highestLevel);
    int numPlayed() const;
    void setNumPlayed(int played);
    bool profileSet() const;

    Q_SLOT void commitChanges();
    void lazyInit();

    Q_INVOKABLE QVariantMap createProfile(QString const& name, QString const& kunya, bool female);
    Q_INVOKABLE QVariantMap editProfile(QObject* caller, qint64 id, QString const& name, QString const& kunya, bool female);
    Q_INVOKABLE void changeProfile(qint64 id);
    Q_INVOKABLE void fetchAllProfiles(QObject* caller);
    Q_INVOKABLE void fetchProfile(QObject* caller, qint64 userId);
    Q_INVOKABLE void recordStats(bb::cascades::ArrayDataModel* adm, QVariantList const& selected, bb::cascades::TextField* tf, int elapsed, bool passed);
    Q_INVOKABLE void recordLifeLine(int lifeKey);
};

} /* namespace canadainc */

#endif /* UserManager_H_ */
