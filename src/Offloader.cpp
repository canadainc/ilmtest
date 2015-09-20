#include "precompiled.h"

#include "Offloader.h"
#include "CommonConstants.h"
#include "Logger.h"
#include "TextUtils.h"

#define IS_CORRECT(x) x.value(KEY_FLAG_CORRECT).toInt() == 1
#define KEY_BOOLEAN_RAND_TEXT(x) x[ TextUtils::randInt( 0, x.size()-1 ) ].toString().arg(arg)
#define MARK_CORRECT(x) x[KEY_FLAG_CORRECT] = 1

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

class NumericDestiny
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        GenerateLessThan,
        GenerateGreaterThan,
        GenerateMixed
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
    int n = RESULT_SET_LIMIT;
    n = 8;

    if (correctAnswer < n) // for example n = 4, and answer=3; then 1,2,3,4
    {
        for (int i = 1; i < correctAnswer; i++)
        {
            current[KEY_CHOICE_VALUE] = i;
            qvl << current;
        }

        for (int i = correctAnswer+1; i < n; i++)
        {
            current[KEY_CHOICE_VALUE] = i;
            qvl << current;
        }
    } else {
        NumericDestiny::Type random = (NumericDestiny::Type)TextUtils::randInt(NumericDestiny::GenerateLessThan, NumericDestiny::GenerateMixed);

        switch (random)
        {
            case NumericDestiny::GenerateLessThan:
                for (int i = correctAnswer-n+1; i < correctAnswer; i++) // n = 8, correctAnswer = 10; 3,4,5,6,7,8,9
                {
                    current[KEY_CHOICE_VALUE] = i;
                    qvl << current;
                }

                break;
            case NumericDestiny::GenerateGreaterThan:
                for (int i = correctAnswer+1; i < correctAnswer+n; i++) // n = 8, correctAnswer = 10; 11,12,13,14,15,16,17
                {
                    current[KEY_CHOICE_VALUE] = i;
                    qvl << current;
                }

                break;
            default:
                for (int i = correctAnswer-n/2; i < correctAnswer; i++) // n = 7, correctAnswer = 10; 7,8,9,10,11,12
                {
                    current[KEY_CHOICE_VALUE] = i;
                    qvl << current;
                }

                for (int i = correctAnswer+1; i < correctAnswer + n/2; i++) // n = 7, correctAnswer = 10; 7,8,9,10,11,12
                {
                    current[KEY_CHOICE_VALUE] = i;
                    qvl << current;
                }

                break;
        }
    }

    current[KEY_CHOICE_VALUE] = correctAnswer;
    MARK_CORRECT(current);
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

        if ( IS_CORRECT(qvm) ) {
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
        bool isCorrect = IS_CORRECT(qvm);

        if (correctOnly == isCorrect) {
            result << qvm;
        }
    }

    if ( !result.isEmpty() ) {
        std::random_shuffle( result.begin(), result.end() );
    }

    return !data.isEmpty() ? result.first() : QVariantMap();
}


QVariantList Offloader::useRandomSources(QVariantList data, bool flipped)
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

    foreach (int id, ids)
    {
        QVariantList x = map[id];
        QVariantMap qvm = x[ TextUtils::randInt( 0, x.size()-1 ) ].toMap();

        if (flipped) {
            qvm[KEY_FLAG_CORRECT] = IS_CORRECT(qvm) ? 0 : 1;
        }

        data << qvm;
    }

    return data;
}


QVariantList Offloader::transformToStandard(QVariantList data, bool trim, bool flipped)
{
    data = useRandomSources(data, flipped);

    std::random_shuffle( data.begin(), data.end() );

    if ( data.size() > 2 && trim )
    {
        int x = RESULT_SET_LIMIT;

        while ( data.size() > x ) {
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
        MARK_CORRECT(qvm);
    }

    result << qvm;

    qvm.clear();
    qvm[KEY_CHOICE_VALUE] = falseString;

    if (!yesCorrect) {
        MARK_CORRECT(qvm);
    }

    result << qvm;

    return result;
}


QVariantList Offloader::processOrdered(QVariantList data, QString& arg1, bool before, bool hasSourceId)
{
    if (hasSourceId) {
        data = useRandomSources(data);
    }

    int targetIndex = TextUtils::randInt( before ? 1 : 0, before ? data.size()-1 : data.size()-2 );
    int correctIndex = before ? targetIndex-1 : targetIndex+1;

    QVariantMap qvm = data.takeAt(correctIndex).toMap();
    MARK_CORRECT(qvm);
    QString target = data.takeAt(targetIndex).toMap().value(KEY_CHOICE_VALUE).toString();

    if ( arg1.isEmpty() ) {
        arg1 = target;
    } else {
        arg1 = arg1.arg(target);
    }

    std::random_shuffle( data.begin(), data.end() );

    int x = RESULT_SET_LIMIT;

    while ( data.size() >= x ) {
        data.removeLast();
    }

    data << qvm;

    std::random_shuffle( data.begin(), data.end() );

    return data;
}


QMap<qint64,QueryId::Type> Offloader::generateQuestions(QMap< QueryId::Type, QSet<qint64> > typeToQuestions, QMap< qint64, QList<QueryId::Type> >& questionToTypes)
{
    QMap<qint64, QueryId::Type> result;
    const int total = questionToTypes.size();

    QList< QPair<QueryId::Type, qreal> > priorities;
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomOrderedQuestion, 0.35);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomAfterQuestion, 0.1);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomBeforeQuestion, 0.1);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomStandardQuestion, 0.35);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomBoolCountQuestion, 0.02);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomBoolStandardQuestion, 0.02);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomCountQuestion, 0.02);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomPromptCountQuestion, 0.02);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomPromptStandardQuestion, 0.02);
    priorities << qMakePair<QueryId::Type, qreal>(QueryId::CustomStandardNegation, 0.02);
    const int totalPriorities = priorities.size();

    for (int i = 0; i < totalPriorities; i++)
    {
        QPair<QueryId::Type, qreal> current = priorities[i];
        QSet<qint64> questions = typeToQuestions.value(current.first);
        typeToQuestions.remove(current.first);

        int minimum = ceil(total*current.second); // 100*0.35 = 35 questions minimum
        int available = questions.size(); // 5 questions available
        int diff = available-minimum; // -30

        if (diff < 0) // means there are less questions available, than desired for this, so we need to distribute the rest of the percentage evenly to the other types
        {
            qreal actualPercentage = questions.isEmpty() ? 0 : available/((qreal)total); // 0.05
            qreal remaining = current.second-actualPercentage; // 0.30 to be distributed
            current.second = actualPercentage; // update the current percentage for the sake of clarity
            priorities[i] = current;

            for (int j = i+1; j < totalPriorities; j++)
            {
                QPair<QueryId::Type, qreal> p = priorities[j];
                p.second += p.second*remaining;
                priorities[j] = p;
            }
        }

        minimum = qMin(available, minimum);

        foreach (qint64 questionId, questions)
        {
            --minimum;

            if ( questionToTypes.contains(questionId) )
            {
                QList<QueryId::Type> types = questionToTypes.value(questionId);
                questionToTypes.remove(questionId);

                foreach (QueryId::Type t, types) // this question must be removed from all other question types that can possily use this
                {
                    if ( typeToQuestions.contains(t) )
                    {
                        QSet<qint64> questions = typeToQuestions.value(t);
                        questions.remove(questionId);
                        typeToQuestions[t] = questions;
                    }
                }

                result[questionId] = current.first;

                if (minimum <= 0) {
                    break;
                }
            }
        }
    }

    return result;
}


QVariantMap Offloader::generateNoneOfTheAbove(QVariantList const& data)
{
    QVariantMap result;
    result[KEY_CHOICE_VALUE] = tr("None of the Above");
    result["none"] = 1;

    bool noneCorrect = true;

    foreach (QVariant const& q, data)
    {
        if ( IS_CORRECT( q.toMap() ) )
        {
            noneCorrect = false;
            break;
        }
    }

    if (noneCorrect) {
        MARK_CORRECT(result);
    }

    return result;
}


Offloader::~Offloader()
{
}

} /* namespace quran */
