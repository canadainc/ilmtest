import bb.cascades 1.2

QtObject
{
    id: globalRoot
    property int presentAnimSpeed: 1000
    property int suspenseDuration: 2000
    property int lifeSuspenseDuration: 5000
    property alias bgPaint: welcomeBack.imagePaint
    
    property variant mainBackground: ImagePaintDefinition
    {
        id: welcomeBack
        imageSource: "images/backgrounds/welcome_bg.png"
        repeatPattern: RepeatPattern.XY
    }
    
    function randomInt(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    }
    
    function onPromptFinished(result, cookie)
    {
        if (result && cookie.app == "quran10") {
            persist.openUri("http://quran10.canadainc.org");
        }
        
        reporter.record( cookie.app, result.toString() );
    }
    
    function onTargetLookupFinished(target, success)
    {
        if (!success)
        {
            reporter.record("Quran10TargetNotFound");
            persist.showDialog(root, {'app': "quran10"}, qsTr("Quran10"), qsTr("This feature requires the app Quran10 v4.0.0.0 or greater to be installed on your device. Would you like to download and install it now?"), qsTr("Yes"), qsTr("No"), true, "", false, "onPromptFinished" );
        }
    }
    
    function onFinished(result, data)
    {
        if (data.cookie == "dbUpdate")
        {
            reporter.record( "UpdateDBResult", result.toString() );
            
            if (result) {
                network.downloadLatestDatabase();
            }
        }
    }
    
    function onUpdateAvailable(size, version)
    {
        var xyz = Qt.formatDate( new Date(version) );
        persist.showDialog( globalRoot, {'cookie': "dbUpdate"}, qsTr("Updates"), qsTr("A new questions database was posted on %1. The total download size is %2. Would you like to download it now?").arg( Qt.formatDate( new Date(version), "MMM d, yyyy").toString() ).arg( textUtils.bytesToSize(size) ), qsTr("Yes"), qsTr("No") );
    }
    
    onCreationCompleted: {
        network.dbUpdateAvailable.connect(onUpdateAvailable);
    }
}