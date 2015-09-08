#include "precompiled.h"

#include "Offloader.h"
#include "CommonConstants.h"
#include "Logger.h"
#include "TextUtils.h"

#define KEY_BOOLEAN_RAND_TEXT(x) x[ TextUtils::randInt( 0, x.size()-1 ) ].toString().arg(arg)

namespace {

class BooleanQuestion
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        TrueIsCorrect,
        FalseIsCorrect,
        YesIsCorrect,
        NoIsCorrect,
        ShowChoices
    };
};

bool compareInts(QVariant const& v1, QVariant const& v2) {
    return v1.toMap().value(KEY_CHOICE_VALUE).toInt() < v2.toMap().value(KEY_CHOICE_VALUE).toInt();
}

}

namespace ilmtest {

using namespace canadainc;

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


QVariantList Offloader::generateBooleanChoices(bb::cascades::AbstractTextControl* atc, QVariantList const& trueStrings, QVariantList const& falseStrings, QVariantList const& truePrompts, QVariantList const& falsePrompts, QVariantList const& choiceTexts, QVariantList const& corrects, QVariantList const& incorrects, QString const& arg)
{
    QVariantList result;
    QString label;
    BooleanQuestion::Type questionType = (BooleanQuestion::Type)TextUtils::randInt(BooleanQuestion::TrueIsCorrect, BooleanQuestion::ShowChoices);

    switch (questionType)
    {
        case BooleanQuestion::TrueIsCorrect:
            label = KEY_BOOLEAN_RAND_TEXT(trueStrings);
            result = setChoices( tr("True"), tr("False") );
            break;
        case BooleanQuestion::FalseIsCorrect:
            label = KEY_BOOLEAN_RAND_TEXT(falseStrings);
            result = setChoices( tr("True"), tr("False"), false );
            break;
        case BooleanQuestion::YesIsCorrect:
            label = KEY_BOOLEAN_RAND_TEXT(truePrompts);
            result = setChoices();
            break;
        case BooleanQuestion::NoIsCorrect:
            label = KEY_BOOLEAN_RAND_TEXT(falsePrompts);
            result = setChoices( tr("Yes"), tr("No"), false );
            break;
        case BooleanQuestion::ShowChoices:
            label = KEY_BOOLEAN_RAND_TEXT(choiceTexts);
            result = setChoices( KEY_BOOLEAN_RAND_TEXT(corrects), KEY_BOOLEAN_RAND_TEXT(incorrects) );
            result = mergeAndShuffle( result, QVariantList() );
            break;
        default:
            break;
    }

    if ( !label.isNull() ) {
        atc->setText(label);
    }

    return result;
}


bool Offloader::verifyMultipleChoice(bb::cascades::ArrayDataModel* adm, QVariantList const& selected)
{
    QSet<int> correctIndices;
    for (int i = adm->size()-1; i >= 0; i--)
    {
        QVariantMap qvm = adm->value(i).toMap();

        if ( qvm.value(KEY_FLAG_CORRECT).toInt() == 1 ) {
            correctIndices << i;
            LOGGER(qvm);
        }
    }

    QSet<int> selectedIndices;
    foreach (QVariant const& q, selected) {
        selectedIndices << q.toList().first().toInt();
    }

    LOGGER(correctIndices << selectedIndices);

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


QVariantMap Offloader::fetchRandomElement(QVariantList data, bool correctOnly)
{
    QList<QVariantMap> result;

    foreach (QVariant const& q, data)
    {
        QVariantMap qvm = q.toMap();
        bool isCorrect = qvm.value(KEY_FLAG_CORRECT) == 1;

        if (correctOnly == isCorrect) {
            result << qvm;
        }
    }

    std::random_shuffle( result.begin(), result.end() );

    return !data.isEmpty() ? result.first() : QVariantMap();
}


QVariantList Offloader::transformToStandard(QVariantList data, bool trim)
{
    QMap<int,QVariantList> map;

    foreach (QVariant const& q, data)
    {
        QVariantMap qvm = q.toMap();
        int realID = REAL_ID(qvm);
        QVariantList list = map[realID];

        if ( !list.isEmpty() )
        {
            QVariantMap first = list.first().toMap();
            qvm[KEY_FLAG_CORRECT] = first.value(KEY_FLAG_CORRECT);

            if ( first.contains(KEY_SORT_ORDER) ) {
                qvm[KEY_SORT_ORDER] = first.value(KEY_SORT_ORDER);
            }
        }

        qvm.remove(KEY_SOURCE_ID);

        list << qvm;
        map[realID] = list;
    }

    QList<int> ids = map.keys();
    data.clear();

    foreach (int id, ids) {
        QVariantList x = map[id];
        data << x[ TextUtils::randInt( 0, x.size()-1 ) ];
    }

    std::random_shuffle( data.begin(), data.end() );

    if ( data.size() > 2 && trim )
    {
        int x = TextUtils::randInt( 0, data.size()-2 );

        for (int i = 0; i < x; i++) {
            data.removeLast();
        }
    }

    return data;
}


QVariantList Offloader::setChoices(QString const& trueString, QString const& falseString, bool yesCorrect)
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

    return result;
}


Offloader::~Offloader()
{
}

} /* namespace quran */
