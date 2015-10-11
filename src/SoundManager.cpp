#include "precompiled.h"

#include "SoundManager.h"
#include "AppLogFetcher.h"
#include "Logger.h"
#include "Persistance.h"

#define FILE_AUDIENCE_RESULTS "asset:///audio/sfx01.mp3"
#define FILE_CHOICE_PRESENT "asset:///audio/choice03.mp3"
#define FILE_CLOCK "asset:///audio/clock.mp3"
#define FILE_CORRECT "asset:///audio/correct.mp3"
#define FILE_DESELECT_CHOICE "asset:///audio/choice02.mp3"
#define FILE_INCORRECT "asset:///audio/incorrect.mp3"
#define FILE_LIFELINE_SELECT "asset:///audio/sfx02.mp3"
#define FILE_LIFELINE_SUSPENSE "asset:///audio/life_suspense.mp3"
#define FILE_MAIN_LOOP "asset:///audio/mainLoop.mp3"
#define FILE_QUESTION_PRESENT "asset:///audio/question.mp3"
#define FILE_SELECT_CHOICE "asset:///audio/choice01.mp3"
#define FILE_USER_INPUT "asset:///audio/inputted.mp3"
#define KEY_MUTE_SOUND "muteSfx"

namespace ilmtest {

using namespace bb::multimedia;

SoundManager::SoundManager(Persistance* p) :
        m_persist(p), m_muted(false), m_loaded(0)
{
}


void SoundManager::onSettingChanged(QVariant newValue, QVariant key)
{
    if ( key.toString() == KEY_MUTE_SOUND )
    {
        m_muted = newValue.toInt() == 1;

        if ( !m_muted && m_map.isEmpty() )
        {
            QStringList sfx = QStringList() << FILE_LIFELINE_SUSPENSE << FILE_AUDIENCE_RESULTS << FILE_LIFELINE_SELECT << FILE_MAIN_LOOP;
            QStringList keys = QStringList() << FILE_DESELECT_CHOICE << FILE_CLOCK << FILE_QUESTION_PRESENT << FILE_CHOICE_PRESENT << FILE_CORRECT << FILE_INCORRECT << FILE_SELECT_CHOICE << FILE_USER_INPUT;

            foreach (QString const& key, keys)
            {
                MediaPlayer* mp = new MediaPlayer(this);
                connect( mp, SIGNAL( mediaStateChanged(bb::multimedia::MediaState::Type) ), this, SLOT( mediaStateChanged(bb::multimedia::MediaState::Type) ) );
                mp->setSourceUrl( QUrl(key) );

                if (key == FILE_CLOCK || key == FILE_MAIN_LOOP || key == FILE_LIFELINE_SUSPENSE) {
                    mp->setRepeatMode(RepeatMode::Track);
                }

                m_map.insert(key, mp);
            }

            foreach ( QString const& key, m_map.keys() )
            {
                MediaPlayer* mp = m_map.value(key);
                mp->prepare();
            }
        }

        emit mutedChanged();
    }
}


void SoundManager::mediaStateChanged(bb::multimedia::MediaState::Type mediaState)
{
    if (mediaState == MediaState::Prepared) {
        emit loadProgress( ++m_loaded, m_map.size() );
    }
}


void SoundManager::lazyInit() {
    m_persist->registerForSetting(this, KEY_MUTE_SOUND);
}


void SoundManager::playAudienceResults() {
    playSound(FILE_AUDIENCE_RESULTS);
}


void SoundManager::setMuted(bool mute)
{
    bool changed = m_persist->saveValueFor(KEY_MUTE_SOUND, mute ? 1 : 0);

    if (changed) {
        AppLogFetcher::getInstance()->record( "MuteSounds", mute ? "1" : "0" );
    }
}


void SoundManager::playSound(QString const& key)
{
    if (!m_muted)
    {
        MediaPlayer* mp = m_map.value(key);

        if (mp)
        {
            if ( mp->mediaState() != MediaState::Started ) {
                mp->play();
            } else {
                mp->seekTime(0);
            }
        }
    }
}


void SoundManager::playClock() {
    playSound(FILE_CLOCK);
}


void SoundManager::playCorrect() {
    playSound(FILE_CORRECT);
}


void SoundManager::playDeselect() {
    playSound(FILE_DESELECT_CHOICE);
}


void SoundManager::playIncorrect() {
    playSound(FILE_INCORRECT);
}


void SoundManager::playLifeLineSelect() {
    playSound(FILE_LIFELINE_SELECT);
}


void SoundManager::playLifeSuspense() {
    playSound(FILE_LIFELINE_SUSPENSE);
}


void SoundManager::playMainLoop() {
    playSound(FILE_MAIN_LOOP);
}


void SoundManager::playPresentChoice() {
    playSound(FILE_CHOICE_PRESENT);
}


void SoundManager::playPresentQuestion() {
    playSound(FILE_QUESTION_PRESENT);
}


void SoundManager::playSelectChoice() {
    playSound(FILE_SELECT_CHOICE);
}


void SoundManager::playUserInput() {
    playSound(FILE_USER_INPUT);
}


void SoundManager::stopClock()
{
    if ( m_map.contains(FILE_CLOCK) ) {
        m_map.value(FILE_CLOCK)->stop();
    }
}


void SoundManager::stopMainLoop()
{
    if ( m_map.contains(FILE_MAIN_LOOP) ) {
        m_map.value(FILE_MAIN_LOOP)->stop();
    }
}


void SoundManager::stopLifeSuspense()
{
    if ( m_map.contains(FILE_LIFELINE_SUSPENSE) ) {
        m_map.value(FILE_LIFELINE_SUSPENSE)->stop();
    }
}


void SoundManager::tempMute()
{
    if (!m_muted)
    {
        m_muted = true;
        emit mutedChanged();
    }
}


bool SoundManager::muted() const {
    return m_muted;
}


SoundManager::~SoundManager()
{
}

} /* namespace ilmtest */
