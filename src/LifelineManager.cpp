#include "LifelineManager.h"
#include "CommonConstants.h"

#define KEY_CHOICE_DISABLED "disabled"

namespace ilmtest {

LifelineManager::LifelineManager()
{
}


void LifelineManager::useFiftyFifty(bb::cascades::ArrayDataModel* adm)
{
    int choicesToRemove = adm->size()/2;
    QList<int> wrongChoices;

    for (int i = adm->size()-1; i >= 0; i--)
    {
        if ( !adm->value(i).toMap().contains(KEY_FLAG_CORRECT) ) {
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


LifelineManager::~LifelineManager()
{
}

} /* namespace canadainc */
