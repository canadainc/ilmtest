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
        //allowDonations: true
        analyticDiffDays: 355
        bbWorldID: "21198062"
        helpPageQml: "IlmTestHelp.qml"
        
        onFinished: {
            if (clean)
            {
                if ( !persist.containsFlag("dbVersion") || (analyticResult == 0 || analyticResult == 5) ) {
                    network.checkForUpdates();
                } else {
                    tutorial.execAppMenu();
                    tutorial.execActionBar( "homeTab", qsTr("The main controls for starting the game are in the %1 tab.").arg(welcomeTab.title), "b" );
                    tutorial.execActionBar( "market", qsTr("To purchase additional lifelines and bonus enhancements for the game, you can visit the %1.").arg(shopTab.title) );
                }
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
        
        onTriggered: {
            console.log("UserEvent: HomeTab");
            reporter.record("HomeTab");
        }
    }
    
    Tab
    {
        id: shopTab
        title: qsTr("Market") + Retranslate.onLanguageChanged
        imageSource: "images/tabs/ic_shop.png"
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
        
        delegate: Delegate {
            source: "ShopPane.qml"
        }
        
        onTriggered: {
            console.log("UserEvent: MarketTab");
            reporter.record("MarketTab");
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
    
    function onTransferring(cookie, current, total)
    {
        if (cookie.dload_db)
        {
            toaster.body = qsTr("Downloading...");
            toaster.statusMessage = "%1/%2".arg(current).arg(total);
            toaster.progress = (current/total)*100;
            
            toaster.show();
        }
    }
    
    function onTransferComplete() {
        toaster.cancel();
    }
    
    function onError(message)
    {
        persist.showToast(message);
        console.log(message);
    }
    
    onCreationCompleted: {
        sound.loadProgress.connect(onLoading);
        game.error.connect(onError);
        network.transferProgress.connect(onTransferring);
        network.questionBankUpdated.connect(onTransferComplete);
    }
    
    attachedObjects: [
        SystemProgressToast {
            id: toaster
            autoUpdateEnabled: true
            state: SystemUiProgressState.Active
        }
    ]
}