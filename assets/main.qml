import bb.cascades 1.3
import com.canadainc.data 1.0
import "paddings.js" as Paddings

NavigationPane
{
    id: navigationPane
    
    onPopTransitionEnded: {
        deviceUtils.cleanUpAndDestroy(page);
    }
    
    Menu.definition: CanadaIncMenu
    {
        id: menuDef
        projectName: "ilmtest"
        allowDonations: true
        bbWorldID: "21198062"
        help.imageSource: "images/menu/ic_help.png"
        help.title: qsTr("Help") + Retranslate.onLanguageChanged
        settings.imageSource: "images/menu/ic_settings.png"
        settings.title: qsTr("Settings") + Retranslate.onLanguageChanged
    }
    
    Page
    {
        id: mainPage
        
        Container
        {
            layout: DockLayout {}
            
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