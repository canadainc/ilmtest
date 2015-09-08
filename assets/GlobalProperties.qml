import bb.cascades 1.2

QtObject
{
    property int presentAnimSpeed: 1000
    
    function randomInt(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    }
    
    function onPromptFinished(result, cookie)
    {
        if (result && cookie.app == "quran10") {
            persist.openUri("http://quran10.canadainc.org");
        }
        
        reporter.record(cookie.app, result);
    }
    
    function onTargetLookupFinished(target, success)
    {
        if (!success) {
            persist.showDialog(root, {'app': "quran10"}, qsTr("Quran10"), qsTr("This feature requires the app Quran10 v4.0.0.0 or greater to be installed on your device. Would you like to download and install it now?"), qsTr("Yes"), qsTr("No"), true, "", false, "onPromptFinished" );
        }
    }
}