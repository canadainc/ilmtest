#include "LifelineManager.h"
#include "CommonConstants.h"
#include "Logger.h"
#include "TextUtils.h"

#define KEY_CHOICE_DISABLED "disabled"

namespace ilmtest {

LifelineManager::LifelineManager()
{
}


void LifelineManager::useFiftyFifty(bb::cascades::ArrayDataModel* adm, bool sorted)
{
    LOGGER(sorted);

    int n = adm->size()/2;

    if (sorted) {
        solveSorted(adm, n);
    } else {
        eliminateIncorrect(adm, n);
    }
}


void LifelineManager::useTakeOne(bb::cascades::ArrayDataModel* adm, bool sorted)
{
    LOGGER(sorted);

    if (sorted) {
        solveSorted(adm, 1);
    } else {
        eliminateIncorrect(adm, 1);
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
