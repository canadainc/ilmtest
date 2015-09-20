#ifndef OFFLOADER_H_
#define OFFLOADER_H_

#include <QObject>
#include <QVariant>

#include "QueryId.h"

namespace bb {
    namespace cascades {
        class AbstractTextControl;
        class ArrayDataModel;
    }
}

namespace ilmtest {

class Offloader : public QObject
{
    Q_OBJECT

public:
    Offloader();
    virtual ~Offloader();

    static QVariantList generateChoices(int correctAnswer);
    static QVariantList mergeAndShuffle(QVariantList i1, QVariantList const& i2);

    /**
     * Possibilities:
     * 1) Surah Fatiha was revealed in Mecca [True] [False] == true
     * 2) {Surah Fatiha was not revealed in Mecca, Surah Fatiha was revealed in Medina} == false
     * 3) Was Surah Fatiha revealed in Mecca? [Yes][No]
     * 4) Was Surah Fatiha revealed in Madina? [Yes][No]
     * 5) Surah Fatiha was revealed in: [Mecca][Madinah]
     *
     * 1) Surah Fatiha has 7 ayats [True][False]
     * 2) Surah Fatiha has 5 ayats [True][False]
     * 3) Does Surah Fatiha have 7 ayats? [Yes][No]
     * 4) Does Surah Fatiha have 5 ayats? [Yes][No]
     * 5) Surah Fatiha has: [7 ayats] [5 ayats]
     *
     * @return From these lists, each QVariantMap will contain a "correct" property if it is the correct one.
     */
    Q_INVOKABLE static QVariantList generateBooleanChoices(bb::cascades::AbstractTextControl* atc, QVariantList const& trueStrings, QVariantList const& falseStrings, QVariantList const& truePrompts, QVariantList const& falsePrompts, QVariantList const& choiceTexts, QVariantList const& corrects, QVariantList const& incorrects, QString const& arg);

    Q_INVOKABLE static bool verifyMultipleChoice(bb::cascades::ArrayDataModel* adm, QVariantList const& selected);
    Q_INVOKABLE static bool verifyOrdered(bb::cascades::ArrayDataModel* adm);

    static QVariantList setChoices(QString const& trueString=QObject::tr("Yes"), QString const& falseString=QObject::tr("No"), bool yesCorrect=true);
    static QVariantList transformToStandard(QVariantList data, bool trim=true, bool flipped=false);
    static QVariantList useRandomSources(QVariantList data, bool flipped=false);
    static QVariantMap fetchRandomElement(QVariantList data, bool correctOnly);
    static QVariantList generateBoolStrings(bool trueIsCorrect);
    static QVariantList processOrdered(QVariantList data, QString& arg1, bool before, bool hasSourceId=true);
    static QMap<qint64,QueryId::Type> generateQuestions(QMap< QueryId::Type, QSet<qint64> > typeToQuestions, QMap< qint64, QList<QueryId::Type> >& questionToTypes);
};

} /* namespace quran */

#endif /* OFFLOADER_H_ */
