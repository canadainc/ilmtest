import bb.cascades 1.3
import com.canadainc.data 1.0

NavigationPane
{
    id: navigationPane
    
    onPopTransitionEnded: {
        deviceUtils.cleanUpAndDestroy(page);
    }
    
    function popToRoot()
    {
        while (navigationPane.top != welcomePage) {
            navigationPane.pop();
        }
    }
    
    Page
    {
        id: welcomePage
        
        keyListeners: [
            KeyListener {
                onKeyLongReleased: {
                    if (event.unicode == "x") {
                        user.points = user.points+300;
                    }
                }
            }
        ]
        
        actions: [
            ActionItem
            {
                id: startExam
                property bool soundsLoaded: false
                ActionBar.placement: ActionBarPlacement.Signature
                imageSource: "images/menu/ic_start.png"
                enabled: (sound.muted || soundsLoaded) && user.profileSet
                title: qsTr("Start") + Retranslate.onLanguageChanged
                
                function onLoading(current, total)
                {
                    if (current == total) {
                        soundsLoaded = true;
                    }
                }
                
                onCreationCompleted: {
                    sound.loadProgress.connect(onLoading);
                }
                
                function beginExam()
                {
                    var x = definition.init("ExamPage.qml");
                    x.nextQuestion();
                    navigationPane.push(x);
                }
                
                function onFinished(confirmed)
                {
                    if (confirmed)
                    {
                        shop.purchasePlugin(Plugin.ExposeAnswer);

                        var data = shop.getAvailablePlugin(Plugin.ExposeAnswer);
                        persist.showToast( qsTr("Purchased %1 for %2 points!").arg(data.title).arg(data.price), data.imageSource );
                    }
                    
                    beginExam();
                }
                
                onTriggered: {
                    console.log("UserEvent: StartExam");
                    reporter.record("StartExam");
                    
                    var plugin = shop.getAvailablePlugin(Plugin.ExposeAnswer);
                    
                    if ( !shop.isExposePurchased && (user.points >= plugin.price) ) {
                        persist.showConfirmDialog( startExam, qsTr("Would you like to purchase the %1 plugin for %2 points?\n\Description of the plugin: %3").arg(plugin.title).arg(plugin.price).arg(plugin.description) );
                    } else {
                        beginExam();
                    }
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
                    id: utb
                    
                    function onCreate(id, name, kunya, female)
                    {
                        var result;
                        
                        if (id) {
                            result = user.editProfile(id, name, kunya, female);
                        } else {
                            result = user.createProfile(name, kunya, female);
                        }
                        
                        if (result.id)
                        {
                            user.changeProfile(result.id);
                            persist.showToast( qsTr("Successfully saved profile"), "images/menu/ic_add_user.png" );
                            popToRoot();
                        }
                    }
                    
                    onBannerTapped: {
                        console.log("UserEvent: BannedTapped");
                        reporter.record("ProfileBannerTapped");
                        
                        var p = definition.init("CreateProfilePage.qml");
                        p.saveProfile.connect(onCreate);
                        
                        if (user.profileSet) {
                            p.userId = user.id;
                        }
                        
                        navigationPane.push(p);
                    }
                    
                    contextActions: [
                        ActionSet
                        {
                            title: user.name
                            subtitle: user.kunya
                            
                            actions: [
                                ActionItem
                                {
                                    enabled: user.profileSet
                                    imageSource: "images/menu/ic_profiles.png"
                                    title: qsTr("Change Profile") + Retranslate.onLanguageChanged
                                    ActionBar.placement: ActionBarPlacement.Signature
                                    
                                    function onPicked(userId)
                                    {
                                        reporter.record("ProfileSwitched");
                                        
                                        user.changeProfile(userId);
                                        popToRoot();
                                        
                                        persist.showToast( qsTr("Profile switched!"), imageSource.toString() );
                                    }
                                    
                                    onTriggered: {
                                        console.log("UserEvent: ChangeProfile");
                                        reporter.record("ChangeProfile");
                                        
                                        definition.source = "UserProfilePickerPage.qml";
                                        var d = definition.createObject();
                                        d.picked.connect(onPicked);
                                        navigationPane.push(d);
                                    }
                                }
                            ]
                        }
                    ]
                }
            }
        }
        
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: global.bgPaint
            
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