#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <QObject>
#include <QMap>
#include <QString>

#include <bb/multimedia/MediaState>

namespace bb {
    namespace multimedia {
        class MediaPlayer;
    }
}

namespace canadainc {
    class Persistance;
}

namespace ilmtest {

using namespace bb::multimedia;
using namespace canadainc;

class SoundManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)

    QMap<QString, MediaPlayer*> m_map;
    Persistance* m_persist;
    bool m_muted;
    int m_loaded;

    void playSound(QString const& key);

private slots:
    void mediaStateChanged(bb::multimedia::MediaState::Type mediaState);
    void onSettingChanged(QVariant newValue, QVariant key);

signals:
    void loadProgress(int current, qreal total);
    void mutedChanged();

public:
    SoundManager(Persistance* p);
    virtual ~SoundManager();

    void lazyInit();
    Q_SLOT void playClock();
    Q_SLOT void playCorrect();
    Q_SLOT void playDeselect();
    Q_SLOT void playLifeLineSelect();
    Q_SLOT void playPresentChoice();
    Q_SLOT void playPresentQuestion();
    Q_SLOT void playSelectChoice();
    Q_SLOT void playUserInput();
    Q_SLOT void stopClock();

    bool muted() const;
    void setMuted(bool mute);
};

} /* namespace ilmtest */

#endif /* SOUNDMANAGER_H_ */
