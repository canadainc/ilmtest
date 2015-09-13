import bb.system 1.2
import bb.cascades 1.3

TabbedPane
{
    id: root
    activeTab: welcomeTab
    showTabsOnActionBar: true
    
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
        
        onFinished: {
            if ( clean && !persist.containsFlag("dbVersion") ) {
                network.checkForUpdates();
            }
        }
    }
    
    Tab
    {
        id: welcomeTab
        title: qsTr("Home") + Retranslate.onLanguageChanged
        imageSource: "images/tabs/ic_home.png"
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
        
        delegate: Delegate {
            source: "WelcomePane.qml"
        }
    }
    
    Tab
    {
        id: shopTab
        title: qsTr("Shop") + Retranslate.onLanguageChanged
        imageSource: "images/tabs/ic_shop.png"
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
        
        delegate: Delegate {
            source: "ShopPane.qml"
        }
    }
    
    Tab
    {
        id: userTab
        title: qsTr("Profile") + Retranslate.onLanguageChanged
        imageSource: "images/tabs/ic_user.png"
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
        
        delegate: Delegate {
            source: "UserProfilePane.qml"
        }
    }
    
    function onLoading(current, total)
    {
        toaster.body = qsTr("Loading audio...");
        toaster.statusMessage = "%1/%2".arg(current).arg(total);
        toaster.progress = (current/total)*100;
        
        if (current == total) {
            toaster.cancel();
        } else {
            toaster.show();
        }
    }
    
    function onError(message) {
        persist.showToast(message);
        console.log(message);
    }
    
    onCreationCompleted: {
        sound.loadProgress.connect(onLoading);
        game.error.connect(onError);
    }
    
    attachedObjects: [
        SystemProgressToast {
            id: toaster
            autoUpdateEnabled: true
            state: SystemUiProgressState.Active
        }
    ]
}