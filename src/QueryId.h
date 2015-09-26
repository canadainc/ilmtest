#ifndef QUERYID_H_
#define QUERYID_H_

#include <qobjectdefs.h>

namespace ilmtest {

class QueryId
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        AfterRevealedSurah,
        AfterSurah,
        AfterVerse,
        BeforeRevealedSurah,
        BeforeSurah,
        BeforeVerse,
        BoolSurahLocation,
        CustomAfterQuestion, // Which pillar of eemaan comes after Belief in Allah?
        CustomBeforeQuestion, // Which pillar of eemaan comes before Belief in the Angels?
        CustomBoolCountQuestion, // There are [5,7] obligatory prayers
        CustomBoolStandardQuestion, // ["Belief in the Big Bang","Belief in Angels"] is one of the pillars of eeman
        CustomCountQuestion, // How many pillars of eemaan are there?
        CustomOrderedQuestion, // Rearrange the pillars of eemaan in order
        CustomPromptCountQuestion, // Are there [5,7] obligatory prayers daily?
        CustomPromptStandardQuestion, // Is ["Belief in the Big Bang","Belief in Angels"] one of the pillars of faith?
        CustomStandardNegation, // Which of the following are NOT pillars of faith?
        CustomStandardQuestion, // Which of the following are pillars of faith?
        NumericBirthYearForIndividual,
        NumericDeathYearForIndividual,
        NumericMaxVerseCount,
        NumericMinVerseCount,
        NumericTotalAyatCount,
        NumericTotalSurahCount,
        NumericVerseCount,
        OrderedPeopleByBirth,
        OrderedPeopleByDeath,
        OrderedRevelationSurahs,
        OrderedSurahs,
        OrderedSurahsByLength,
        OrderedSurahVerses,
        StandardFemale,
        StandardSahabah,
        StandardSajdaSurah,
        StandardStudent,
        StandardTabiee,
        StandardTabiTabiee,
        StandardTeacher,
        StandardVersesForSurah,
        TempArgument1,
        FetchIndividualArgument1,
        MultipleChoice,
        TextInput,
        GenerateTruth,
        GenerateFalsehood,
        AnswersForCustomAfterQuestion,
        AnswersForCustomBeforeQuestion,
        AnswersForCustomBoolCountQuestion,
        AnswersForCustomBoolStandardQuestion,
        AnswersForCustomCountQuestion,
        AnswersForCustomOrderedQuestion,
        AnswersForCustomPromptCountQuestion,
        AnswersForCustomPromptStandardQuestion,
        AnswersForCustomStandardNegation,
        AnswersForCustomStandardQuestion,
        EditProfile,
        FetchAllProfiles,
        FetchDictionary,
        FetchProfile,
        RecordStats,
        Setup,
        Debug
    };
};

}

#endif /* QUERYID_H_ */
