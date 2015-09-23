#include "precompiled.h"

#include "LifelineManager.h"
#include "CommonConstants.h"
#include "Game.h"
#include "Logger.h"
#include "ShopManager.h"
#include "TextUtils.h"

#define KEY_CHOICE_DISABLED "disabled"
#define NUMERIC_ANSWER m_game->currentQuestion().value(KEY_ANSWER).toInt()

namespace {

void bubbleSort(bb::cascades::ArrayDataModel* adm)
{
    int n = adm->size();

    for (int i = 0; i < n; i++)
    {
        for(int j = 0; j < n-1; j++)
        {
            if ( adm->value(j).toMap().value(KEY_SORT_ORDER).toInt() > adm->value(j+1).toMap().value(KEY_SORT_ORDER).toInt() ) {
                adm->swap(j, j+1);
            }
        }
    }
}

void eliminateIncorrect(bb::cascades::ArrayDataModel* adm, int choicesToRemove)
{
    QList<int> wrongChoices;

    for (int i = adm->size()-1; i >= 0; i--)
    {
        QVariantMap current = adm->value(i).toMap();

        if ( ( current.value(KEY_FLAG_CORRECT) != 1 ) && !current.contains(KEY_CHOICE_DISABLED) ) {
            wrongChoices << i;
        }
    }

    std::random_shuffle( wrongChoices.begin(), wrongChoices.end() );

    for (int i = 0; i < choicesToRemove && !wrongChoices.isEmpty(); i++)
    {
        int index = wrongChoices.takeFirst();
        QVariantMap qvm = adm->value(index).toMap();
        qvm[KEY_CHOICE_DISABLED] = 1;
        adm->replace(index, qvm);
    }
}


void solveSorted(bb::cascades::ArrayDataModel* adm, int n)
{
    int start = canadainc::TextUtils::randInt(0,n-1);

    for (int i = start; i < n; i += 2)
    {
        if ( adm->value(i).toMap().value(KEY_SORT_ORDER).toInt() > adm->value(i+1).toMap().value(KEY_SORT_ORDER).toInt() ) {
            adm->swap(i, i+1);
        }
    }
}

class BelowAbove
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Below,
        Above
    };
};

}

namespace ilmtest {

using namespace bb::cascades;
using namespace canadainc;

LifelineManager::LifelineManager(Game* game, ShopManager* shop) : m_game(game), m_shop(shop)
{
}


void LifelineManager::lazyInit()
{
    connect( m_game, SIGNAL( levelChanged() ), this, SLOT( onCurrentLevelChanged() ) );

    m_levelToLifeline.insertMulti( 1, LifelineData( Lifeline::FiftyFifty, tr("Fifty Fifty"), "images/list/lifelines/ic_lifeline_50.png" ) );
    m_levelToLifeline.insertMulti( 1, LifelineData( Lifeline::PopularOpinion, tr("Popular Opinion"), "images/list/lifelines/ic_lifeline_audience.png" ) );
    m_levelToLifeline.insertMulti( 1, LifelineData( Lifeline::PhoneFriend, tr("Phone a Friend"), "images/list/lifelines/ic_lifeline_friend.png" ) );
    m_levelToLifeline.insertMulti( 5, LifelineData( Lifeline::FreezeTime, tr("Freeze Clock"), "images/list/lifelines/ic_lifeline_clock.png" ) );
    m_levelToLifeline.insertMulti( 10, LifelineData( Lifeline::ChangeQuestion, tr("Change the Question"), "images/list/lifelines/ic_lifeline_change.png" ) );

    //m_levelToLifeline.insertMulti( 1, LifelineData( Lifeline::AskAnExpert, tr("Ask an Expert"), "images/list/lifelines/ic_lifeline_expert.png" ) );
    //m_levelToLifeline.insertMulti( 1, LifelineData( Lifeline::TakeOne, tr("Take One"), "images/list/lifelines/ic_lifeline_take_one.png" ) );

    m_codeToLifeline[ LID_TO_QSTR(Lifeline::AskAnExpert) ] = LifelineData( Lifeline::AskAnExpert, tr("Ask an Expert"), "images/list/lifelines/ic_lifeline_expert.png" );
    m_codeToLifeline[ LID_TO_QSTR(Lifeline::SecondChance) ] = LifelineData( Lifeline::SecondChance, tr("Second Chance"), "images/list/lifelines/ic_lifeline_second.png" );
    m_codeToLifeline[ LID_TO_QSTR(Lifeline::TakeOne) ] = LifelineData( Lifeline::TakeOne, tr("Take One"), "images/list/lifelines/ic_lifelines_take_one.png" );
}


void LifelineManager::onCurrentLevelChanged()
{
    int level = m_game->level();

    if (level == 1)
    {
        QStringList lifelines = m_shop->getLifelines();

        foreach (QString const& purchase, lifelines)
        {
            LifelineData ld = m_codeToLifeline.value(purchase);
            emit lifeLineAvailable(ld.title, ld.imageSource, ld.key);
        }
    }

    if ( m_levelToLifeline.contains(level) )
    {
        QList<LifelineData> lifelines = m_levelToLifeline.values(level);

        foreach (LifelineData ld, lifelines) {
            emit lifeLineAvailable(ld.title, ld.imageSource, ld.key);
        }
    }
}


QString LifelineManager::keyToString(int q) {
    return LID_TO_QSTR(q);
}


void LifelineManager::useLifeline(int key, bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(key);

    if ( m_codeToLifeline.contains( LID_TO_QSTR(key) ) ) {
        m_shop->refundLifeline(key);
    }

    switch (key)
    {
        case Lifeline::AskAnExpert:
            useAskExpert(adm, tf, sorted);
            emit lifeLineUsed(key, QVariant());
            break;
        case Lifeline::FiftyFifty:
            useFiftyFifty(adm, tf, sorted);
            emit lifeLineUsed(key, QVariant());
            break;
        case Lifeline::TakeOne:
            useTakeOne(adm, tf, sorted);
            emit lifeLineUsed(key, QVariant());
            break;
        case Lifeline::PopularOpinion:
            usePopularOpinion(adm, tf, sorted);
            break;
        default:
            emit lifeLineUsed(key, QVariant());
            break;
    }
}


void LifelineManager::useAskExpert(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(sorted);

    if ( m_game->numeric() ) {
        tf->setHintText( QString::number(NUMERIC_ANSWER) );
    } else if ( m_game->multipleChoice() ) {
        int n = adm->size();

        if (sorted) {
            bubbleSort(adm);
        } else {
            eliminateIncorrect(adm, n);
        }
    }
}


void LifelineManager::useFiftyFifty(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(sorted);

    if ( m_game->numeric() )
    {
        int answer = NUMERIC_ANSWER;
        int half = answer/2;

        BelowAbove::Type path = (BelowAbove::Type)TextUtils::randInt(BelowAbove::Below, BelowAbove::Above);

        if (path == BelowAbove::Below) {
            half = TextUtils::randInt(answer-half, answer-1);
        } else if (path == BelowAbove::Above) {
            half = TextUtils::randInt(answer+1, answer+half);
        }

        QStringList possible = QStringList() << QString::number(answer) << QString::number(half);
        possible.sort();

        tf->setHintText( tr("Either %1 ").arg( possible.join(" or ") ) );
    } else if ( m_game->multipleChoice() ) {
        int n = adm->size()/2;

        if (sorted) {
            solveSorted(adm, n);
        } else {
            eliminateIncorrect(adm, n);
        }
    }
}


void LifelineManager::useFriend(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(sorted);


}


void LifelineManager::usePopularOpinion(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(sorted);

    QVariantList data;

    if ( m_game->numeric() )
    {
        int answer = NUMERIC_ANSWER;
        int total = adm->size();

        for (int i = answer+1; i < answer+total; i++)
        {
            QVariantMap qvm;
            qvm[KEY_CHOICE_VALUE] = i;
            data << qvm;
        }

        for (int i = qMax(0, answer-total); i < answer; i++)
        {
            QVariantMap qvm;
            qvm[KEY_CHOICE_VALUE] = i;
            data << qvm;
        }

        std::random_shuffle( data.begin(), data.end() );

        while ( data.size() > total-1 ) {
            data.takeLast();
        }

        int correctPercentage = TextUtils::randInt(40,100);
        int remaining = 100-correctPercentage;

        QVariantMap qvm;
        qvm[KEY_CHOICE_VALUE] = answer;
        qvm["ratio"] = correctPercentage;

        for (int i = 0; i < data.size(); i++)
        {
            int currentRatio = TextUtils::randInt(0, remaining);
            remaining -= qMax(0, currentRatio);

            QVariantMap q = data[i].toMap();
            q["ratio"] = currentRatio;
            data << q;
        }

        data << qvm;

        std::random_shuffle( data.begin(), data.end() );

    } else if ( m_game->multipleChoice() ) {
        int correctPercentage = TextUtils::randInt(40,100);
        int remaining = 100-correctPercentage;

        for (int i = 0; i < adm->size(); i++)
        {
            QVariantMap current = adm->value(i).toMap();

            int currentRatio = TextUtils::randInt(0, remaining);

            if ( !IS_CORRECT(current) ) {
                remaining -= qMax(0, currentRatio);
            }

            current["ratio"] = IS_CORRECT(current) ? correctPercentage : currentRatio;
            data << current;
        }
    }

    emit lifeLineUsed(Lifeline::PopularOpinion, data);
}


void LifelineManager::useTakeOne(bb::cascades::ArrayDataModel* adm, bb::cascades::TextField* tf, bool sorted)
{
    LOGGER(sorted);

    if ( m_game->numeric() )
    {
        int answer = NUMERIC_ANSWER;
        int half = answer/2;
        QStringList possible = QStringList() << QString::number(answer) << QString::number( TextUtils::randInt(answer-half, answer-1) ) << QString::number( TextUtils::randInt(answer+1, answer+half) );
        possible.sort();

        tf->setHintText( tr("Either %1 ").arg( possible.join(" or ") ) );
    } else if ( m_game->multipleChoice() ) {
        if (sorted) {
            solveSorted(adm, 1);
        } else {
            eliminateIncorrect(adm, 1);
        }
    }
}


LifelineManager::~LifelineManager()
{
}

} /* namespace canadainc */
