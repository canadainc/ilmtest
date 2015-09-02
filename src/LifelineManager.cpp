#include "precompiled.h"

#include "LifelineManager.h"
#include "CommonConstants.h"
#include "Game.h"
#include "Logger.h"
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

using namespace canadainc;

LifelineManager::LifelineManager(Game* game) : m_game(game)
{
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


void LifelineManager::eliminateIncorrect(bb::cascades::ArrayDataModel* adm, int choicesToRemove)
{
    QList<int> wrongChoices;

    for (int i = adm->size()-1; i >= 0; i--)
    {
        QVariantMap current = adm->value(i).toMap();

        if ( !current.contains(KEY_FLAG_CORRECT) && !current.contains(KEY_CHOICE_DISABLED) ) {
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


void LifelineManager::solveSorted(bb::cascades::ArrayDataModel* adm, int n)
{
    int start = canadainc::TextUtils::randInt(0,n-1);

    for (int i = start; i < n; i += 2)
    {
        if ( adm->value(i).toMap().value(KEY_SORT_ORDER).toInt() > adm->value(i+1).toMap().value(KEY_SORT_ORDER).toInt() ) {
            adm->swap(i, i+1);
        }
    }
}


LifelineManager::~LifelineManager()
{
}

} /* namespace canadainc */
