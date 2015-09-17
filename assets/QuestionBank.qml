import bb.cascades 1.3
import com.canadainc.data 1.0

QtObject
{
    property variant questionDB
    
    function getBodies(type)
    {
        if (!questionDB)
        {
            var map = {};
            map[QueryId.AfterRevealedSurah] = {'choiceTexts': [qsTr("Which surah was revealed right after %1?")]};
            map[QueryId.AfterSurah] = {'choiceTexts': [qsTr("Which surah comes right after %1?")]};
            map[QueryId.AfterVerse] = {'choiceTexts': [qsTr("Which ayat comes right after %1?")]};
            map[QueryId.BeforeRevealedSurah] = {'choiceTexts': [qsTr("Which surah was revealed right before %1?")]};
            map[QueryId.BeforeSurah] = {'choiceTexts': [qsTr("Which surah comes right before %1?")]};
            map[QueryId.BeforeVerse] = {'choiceTexts': [qsTr("Which verse comes right before %1?")]};
            map[QueryId.BoolSurahLocation] = {'trueStrings': [ qsTr("%1 was revealed in Mecca") ], 'falseStrings': [qsTr("%1 was revealed in Medina"), qsTr("%1 was not revealed in Mecca")], 'truePrompts': [qsTr("Was %1 revealed in Mecca?")], 'falsePrompts': [qsTr("Was %1 revealed in Medina?")], 'choiceTexts': [qsTr("%1 was revealed in")], 'corrects': [qsTr("Mecca")], 'incorrects': [qsTr("Medina")]};
            map[QueryId.NumericVerseCount] = {'choiceTexts': [qsTr("How many verses does %1 contain?")]};
            map[QueryId.NumericTotalSurahCount] = {'choiceTexts': [qsTr("How many surahs are there in the Qu'ran?")]};
            map[QueryId.NumericTotalAyatCount] = {'choiceTexts': [qsTr("How many total ayats are there in the Qu'ran?")]};
            map[QueryId.NumericMaxVerseCount] = {'choiceTexts': [qsTr("How many verses are there in the longest surah in the Qu'ran?")]};
            map[QueryId.NumericMinVerseCount] = {'choiceTexts': [qsTr("How many verses are there in the shortest surah in the Qu'ran?")]};
            map[QueryId.NumericBirthYearForIndividual] = {'choiceTexts': [qsTr("What year (after Hijrah) was %1 born?")]};
            map[QueryId.NumericDeathYearForIndividual] = {'choiceTexts': [qsTr("What year (after Hijrah) did %1 pass away?")]};
            map[QueryId.OrderedPeopleByBirth] = {'choiceTexts': [qsTr("Rearrange the following individuals in order of birth (from the earliest to the latest)?")]};
            map[QueryId.OrderedPeopleByDeath] = {'choiceTexts': [qsTr("Rearrange the following individuals in order of death (from the earliest to the latest)?")]};
            map[QueryId.OrderedSurahs] = {'choiceTexts': [qsTr("Please arrange the following surahs in order.")]};
            map[QueryId.OrderedRevelationSurahs] = {'choiceTexts': [qsTr("<html>Please arrange the following surahs in the <b>original order of revelation</b></html>")]};
            map[QueryId.OrderedSurahVerses] = {'choiceTexts': [qsTr("Please arrange the following verses from %1 in order")]};
            map[QueryId.OrderedSurahsByLength] = {'choiceTexts': [qsTr("Please arrange the following surahs in order from shortest to longest.")]};
            map[QueryId.StandardSahabah] = {'choiceTexts': [qsTr("Who of the following was a Sahabah?"), qsTr("Who of the following was a companion of the Prophet sallalahu alayhi wa'sallam?")]};
            map[QueryId.StandardFemale] = {'choiceTexts': [qsTr("Who of the following is female?")]};
            map[QueryId.StandardStudent] = {'choiceTexts': [qsTr("Who of the following was a student of %1?")]};
            map[QueryId.StandardTabiee] = {'choiceTexts': [qsTr("Who of the following was a Tabiee?"), qsTr("Who of the following was a student of the companions of the Prophet sallalahu alayhi wa'sallam?")]};
            map[QueryId.StandardTabiTabiee] = {'choiceTexts': [qsTr("Who of the following was a Tabi‘ al-Tabi‘in?")]};
            map[QueryId.StandardTeacher] = {'choiceTexts': [qsTr("Who of the following was a teacher of %1?")]};
            map[QueryId.StandardSajdaSurah] = {'choiceTexts': [qsTr("Which of the following surahs contain a Sujud al-Tilawah (Prostration of Qu'ran Recitation)?")]};
            map[QueryId.StandardVersesForSurah] = {'choiceTexts': [qsTr("Which of the following are verses found in %1?")]};
            questionDB = map;
        }
        
        return questionDB[type];
    }
}