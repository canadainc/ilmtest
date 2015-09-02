import bb.cascades 1.3
import com.canadainc.data 1.0

QtObject
{
    property variant questionDB
    
    function getBody(type)
    {
        if (!questionDB)
        {
            var map = {};
            map[QueryId.NumericVerseCount] = qsTr("How many verses does %1 contain?");
            map[QueryId.NumericTotalSurahCount] = qsTr("How many surahs are there in the Qu'ran?");
            map[QueryId.NumericTotalAyatCount] = qsTr("How many total ayats are there in the Qu'ran?");
            map[QueryId.NumericMaxVerseCount] = qsTr("How many verses are there in the longest surah in the Qu'ran?");
            map[QueryId.NumericMinVerseCount] = qsTr("How many verses are there in the shortest surah in the Qu'ran?");
            map[QueryId.NumericBirthYearForIndividual] = qsTr("What year (after Hijrah) was %1 born?");
            map[QueryId.NumericDeathYearForIndividual] = qsTr("What year (after Hijrah) did %1 pass away?");
            map[QueryId.OrderedSurahs] = qsTr("Please arrange the following surahs in order.");
            map[QueryId.OrderedRevelationSurahs] = qsTr("<html>Please arrange the following surahs in the <b>original order of revelation</b></html>");
            map[QueryId.OrderedSurahVerses] = qsTr("Please arrange the following verses from %1 in order");
            map[QueryId.OrderedSurahsByLength] = qsTr("Please arrange the following surahs in order from shortest to longest.");
            map[QueryId.StandardVersesForSurah] = qsTr("Which of the following are verses found in %1?");
            map[QueryId.StandardSajdaSurah] = qsTr("Which of the following surahs contain a Sujud al-Tilawah (Prostration of Qu'ran Recitation)?");
            questionDB = map;
        }
        
        return questionDB[type];
    }
}