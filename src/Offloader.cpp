#include "precompiled.h"

#include "Offloader.h"
#include "CommonConstants.h"
#include "Logger.h"
#include "TextUtils.h"

#define KEY_CHOICE_VALUE "value"

namespace {

bool compareInts(QVariant const& v1, QVariant const& v2) {
    return v1.toMap().value(KEY_CHOICE_VALUE).toInt() < v2.toMap().value(KEY_CHOICE_VALUE).toInt();
}

QVariantList setChoices(bb::cascades::AbstractTextControl* atc, QString const& label, QString const& trueString=QObject::tr("Yes"), QString const& falseString=QObject::tr("No"), bool yesCorrect=true)
{
    QVariantList result;

    QVariantMap qvm;
    qvm[KEY_CHOICE_VALUE] = trueString;

    if (yesCorrect) {
        qvm[KEY_FLAG_CORRECT] = 1;
    }

    result << qvm;

    qvm.clear();
    qvm[KEY_CHOICE_VALUE] = falseString;

    if (!yesCorrect) {
        qvm[KEY_FLAG_CORRECT] = 1;
    }

    result << qvm;

    atc->setText(label);

    return result;
}

}

namespace ilmtest {

Offloader::Offloader()
{
}


QVariantList Offloader::generateChoices(int correctAnswer)
{
    QVariantList qvl;
    QVariantMap current;

    current[KEY_CHOICE_VALUE] = correctAnswer-1;
    qvl << current;

    current[KEY_CHOICE_VALUE] = correctAnswer+1;
    qvl << current;

    current[KEY_CHOICE_VALUE] = correctAnswer-2;
    qvl << current;

    current[KEY_CHOICE_VALUE] = correctAnswer;
    current[KEY_FLAG_CORRECT] = 1;
    qvl << current;

    qSort( qvl.begin(), qvl.end(), compareInts );

    return qvl;
}


QVariantList Offloader::mergeAndShuffle(QVariantList i1, QVariantList const& i2)
{
    i1.append(i2);

    std::random_shuffle( i1.begin(), i1.end() );
    return i1;
}


QVariantList Offloader::generateBooleanChoices(bb::cascades::AbstractTextControl* atc, QString const& trueString, QString const& falseString, QString const& truePrompt, QString const& falsePrompt, QString const& choiceText, QString const& correct, QString const& incorrect)
{
    QVariantList result;
    int questionType = canadainc::TextUtils::randInt(1,5);

    switch (questionType)
    {
        case 1:
            result = setChoices(atc, trueString, tr("True"), tr("False") );
            break;
        case 2:
            result = setChoices(atc, falseString, tr("True"), tr("False"), false );
            break;
        case 3:
            result = setChoices(atc, truePrompt);
            break;
        case 4:
            result = setChoices(atc, falsePrompt, tr("Yes"), tr("No"), false);
            break;
        case 5:
            result = setChoices(atc, choiceText, correct, incorrect);
            result = mergeAndShuffle( result, QVariantList() );
            break;
        default:
            break;
    }

    return result;
}


bool Offloader::verifyMultipleChoice(bb::cascades::ArrayDataModel* adm, QVariantList const& selected)
{
    QSet<int> correctIndices;
    for (int i = adm->size()-1; i >= 0; i--)
    {
        QVariantMap qvm = adm->value(i).toMap();

        if ( qvm.contains(KEY_FLAG_CORRECT) ) {
            correctIndices << i;
        }
    }

    QSet<int> selectedIndices;
    foreach (QVariant const& q, selected) {
        selectedIndices << q.toList().first().toInt();
    }

    return correctIndices == selectedIndices;
}


bool Offloader::verifyOrdered(bb::cascades::ArrayDataModel* adm)
{
    bool sorted = true;

    for (int i = 0; i < adm->size()-1; i++)
    {
        if ( adm->value(i).toMap().value(KEY_SORT_ORDER).toInt() >= adm->value(i+1).toMap().value(KEY_SORT_ORDER).toInt() ) {
            sorted = false;
            break;
        }
    }

    return sorted;
}


Offloader::~Offloader()
{
}

} /* namespace quran */
