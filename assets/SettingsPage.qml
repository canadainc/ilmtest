import bb.cascades 1.3

Page
{
    id: settingsPage
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    
    titleBar: TitleBar {
        title: qsTr("Settings") + Retranslate.onLanguageChanged
    }
    
    actions: [
        ActionItem
        {
            id: updateCheck
            imageSource: "images/menu/ic_check_for_updates.png"
            ActionBar.placement: ActionBarPlacement.Signature
            title: qsTr("Check for Updates") + Retranslate.onLanguageChanged
            
            onTriggered: {
                console.log("UserEvent: CheckForUpdate");
                enabled = false;
                network.checkForUpdates();
            }
            
            function onUpdateCheckComplete() {
                enabled = true;
            }
            
            onCreationCompleted: {
                network.updateCheckComplete.connect(onUpdateCheckComplete);
            }
        }
    ]
    
    function cleanUp() {}
    
    Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {}
        
        ScrollView
        {  
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            scrollRole: ScrollRole.Main
            
            Container
            {
                leftPadding: 10
                topPadding: 10
                rightPadding: 10
                bottomPadding: 10
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                CheckBox
                {
                    checked: sound.muted
                    text: qsTr("Mute Sounds") + Retranslate.onLanguageChanged
                    
                    onCheckedChanged: {
                        sound.muted = checked;
                    }
                }
                
                PersistCheckBox
                {
                    key: "customOnly"
                    text: qsTr("Show Custom Questions Only") + Retranslate.onLanguageChanged
                }
            }
        }
    }
}