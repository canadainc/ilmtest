import bb.cascades 1.3

Page
{
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll

    actions: [
        ActionItem
        {
            id: updateCheck
            imageSource: "images/menu/ic_check_for_updates.png"
            ActionBar.placement: ActionBarPlacement.Signature
            title: qsTr("Check for Updates") + Retranslate.onLanguageChanged
            
            onTriggered: {
                console.log("UserEvent: CheckForUpdate");
                reporter.record("CheckForUpdate");
                enabled = false;
                network.checkForUpdates();
            }
            
            function onUpdateCheckComplete() {
                enabled = true;
            }
        }
    ]

    titleBar: AboutTitleBar
    {
        id: atb
        videoTutorialUri: "http://youtu.be/YOXtjnNWVZM"
    }
    
    Container
    {
        leftPadding: 10; rightPadding: 10; topPadding: 10
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Fill
        
        Label
        {
            id: versionInfo
            horizontalAlignment: HorizontalAlignment.Fill
            textStyle.textAlign: TextAlign.Center
            
            function recompute()
            {
                var qbVersion = parseInt( persist.getFlag("dbVersion") );
                text = qsTr("Question Bank: %1").arg( new Date(qbVersion).toDateString() );
            }
        }
    }
    
    function cleanUp()
    {
        network.updateCheckComplete.disconnect(updateCheck.onUpdateCheckComplete);
        network.updateCheckComplete.disconnect(versionInfo.recompute);
    }
    
    onCreationCompleted: {
        network.updateCheckComplete.connect(updateCheck.onUpdateCheckComplete);
        network.questionBankUpdated.connect(versionInfo.recompute);
        versionInfo.recompute();
    }
}