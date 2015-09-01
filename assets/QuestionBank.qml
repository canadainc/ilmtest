import bb.cascades 1.3
import com.canadainc.data 1.0

QtObject
{
    property string surahName
    property variant numericDB
    property variant rearrangeDB
    property variant standardDB
    property variant boolDB
    
    function isNumeric(type)
    {
        if (!numericDB)
        {
            var map = {};
            map[QueryId.FetchRandomVerseCount] = qsTr("How many verses does %1 contain?");
            map[QueryId.FetchTotalSurahCount] = qsTr("How many surahs are there in the Qu'ran?");
            map[QueryId.FetchTotalAyatCount] = qsTr("How many total ayats are there in the Qu'ran?");
            map[QueryId.FetchMaxVerseCount] = qsTr("How many verses are there in the longest surah in the Qu'ran?");
            map[QueryId.FetchMinVerseCount] = qsTr("How many verses are there in the shortest surah in the Qu'ran?");
            numericDB = map;
        }
        
        return type in numericDB;
    }
    
    function isOrdered(type)
    {
        if (!rearrangeDB)
        {
            var map = {};
            map[QueryId.FetchRandomSurahs] = qsTr("Please arrange the following surahs in order.");
            map[QueryId.FetchSurahsByRevealed] = qsTr("<html>Please arrange the following surahs in the <b>original order of revelation</b></html>");
            map[QueryId.FetchSurahRandomVerses] = qsTr("Please arrange the following verses from %1 in order");
            map[QueryId.FetchRandomSurahsByLength] = qsTr("Please arrange the following surahs in order from shortest to longest.");
            rearrangeDB = map;
        }
        
        return type in rearrangeDB;
    }
    
    function isStandard(type)
    {
        if (!standardDB)
        {
            var map = {};
            map[QueryId.FetchVersesForRandomSurah] = qsTr("Which of the following are verses found in %1?");
            map[QueryId.FetchRandomSajdaSurah] = qsTr("Which of the following surahs contain a Sujud al-Tilawah (Prostration of Qu'ran Recitation)?");
            standardDB = map;
        }
        
        return type in standardDB;
    }
    
    function isBool(type)
    {
        if (!boolDB)
        {
            var map = {};
            boolDB = map;
        }
        
        return type in boolDB;
    }
    
    function getBody(type)
    {
        if ( isNumeric(type) ) {
            return numericDB[type];
        } else if ( isOrdered(type) ) {
            return rearrangeDB[type];
        } else if ( isBool(type) ) {
            return boolDB[type];
        } else {
            return standardDB[type];
        }
    }
}