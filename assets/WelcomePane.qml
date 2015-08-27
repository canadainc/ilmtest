import bb.cascades 1.3

NavigationPane
{
    id: navigationPane
    
    onPopTransitionEnded: {
        deviceUtils.cleanUpAndDestroy(page);
    }
    
    Page
    {
        id: welcomePage
        
        actions: [
            ActionItem
            {
                ActionBar.placement: ActionBarPlacement.Signature
                imageSource: "images/menu/ic_start.png"
                title: qsTr("Start") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    var x = definition.init("ExamPage.qml");
                    x.nextQuestion();
                    
                    navigationPane.push(x);
                }
            }
        ]
        
        titleBar: TitleBar
        {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties
            {
                UserTitleBanner
                {
                    onBannerTapped: {
                        var p = definition.init("UserProfilePage.qml");
                        navigationPane.push(p);
                    }
                }
            }
        }
        
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            PermissionToast
            {
                id: permissions
                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Center
                
                function process()
                {
                    var allMessages = [];
                    var allIcons = [];
                    
                    if (allMessages.length > 0)
                    {
                        messages = allMessages;
                        icons = allIcons;
                        delegateActive = true;
                    }
                }
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: definition
                
                function init(qml)
                {
                    source = qml;
                    return createObject();
                }
            }
        ]
    }
}