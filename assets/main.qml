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
        id: userTab
        title: qsTr("Profile") + Retranslate.onLanguageChanged
        imageSource: "images/tabs/ic_user.png"
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
        
        delegate: Delegate {
            source: "UserProfilePane.qml"
        }
    }
}