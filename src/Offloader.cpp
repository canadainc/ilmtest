#include "precompiled.h"

#include "Offloader.h"
#include "Logger.h"

#define KEY_FLAG_CORRECT "correct"
#define KEY_CHOICE_VALUE "value"

namespace {

bool compareInts(QVariant const& v1, QVariant const& v2) {
    return v1.toMap().value(KEY_CHOICE_VALUE).toInt() < v2.toMap().value(KEY_CHOICE_VALUE).toInt();
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
    //std::random_shuffle( qvl.begin(), qvl.end() );

    return qvl;
}


Offloader::~Offloader()
{
}

} /* namespace quran */
