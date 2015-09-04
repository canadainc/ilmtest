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
    BooleanQuestion::Type questionType = (BooleanQuestion::Type)TextUtils::randInt(BooleanQuestion::TrueIsCorrect, BooleanQuestion::ShowChoices);

    switch (questionType)
    {
        case BooleanQuestion::TrueIsCorrect:
            result = setChoices(atc, KEY_BOOLEAN_RAND_TEXT(trueStrings), tr("True"), tr("False") );
            break;
        case BooleanQuestion::FalseIsCorrect:
            result = setChoices(atc, KEY_BOOLEAN_RAND_TEXT(falseStrings), tr("True"), tr("False"), false );
            break;
        case BooleanQuestion::YesIsCorrect:
            result = setChoices(atc, KEY_BOOLEAN_RAND_TEXT(truePrompts));
            break;
        case BooleanQuestion::NoIsCorrect:
            result = setChoices(atc, KEY_BOOLEAN_RAND_TEXT(falsePrompts), tr("Yes"), tr("No"), false);
            break;
        case BooleanQuestion::ShowChoices:
            result = setChoices(atc, KEY_BOOLEAN_RAND_TEXT(choiceTexts), KEY_BOOLEAN_RAND_TEXT(corrects), KEY_BOOLEAN_RAND_TEXT(incorrects));
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

        if ( qvm.value(KEY_FLAG_CORRECT).toInt() == 1 ) {
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


QVariantList Offloader::transformToStandard(QVariantList data)
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
            qvm["sort_order"] = first.value("sort_order");
            qvm["correct"] = first.value("correct");
        }

        qvm.remove("source_id");

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

    if ( data.size() > 2 )
    {
        int x = TextUtils::randInt( 0, data.size()-2 );

        for (int i = 0; i < x; i++) {
            data.removeLast();
        }
    }

    return data;
}


QString Offloader::getRandomQuestionColumn(QVariantMap const& qvm)
{
    QStringList interested = QStringList() << "standard_body" << "ordered_body" << "count_body"/* << "before_body" << "after_body"*/;
    QStringList availableKeys;

    foreach (QString const& key, interested)
    {
        QString currentBody = qvm.value(key).toString();

        if ( !currentBody.isEmpty() ) {
            availableKeys << key;
        }
    }

    return availableKeys[ TextUtils::randInt( 0, availableKeys.size()-1 ) ];
}


Offloader::~Offloader()
{
}

} /* namespace quran */
