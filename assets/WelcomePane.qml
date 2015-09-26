import bb.cascades 1.3

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
                                        user.changeProfile(userId);
                                        popToRoot();
                                        
                                        persist.showToast( qsTr("Profile switched!"), imageSource.toString() );
                                    }
                                    
                                    onTriggered: {
                                        console.log("UserEvent: ChangeProfile");
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