import bb.cascades 1.3

QtObject
{
    property variant fifty: ActionItem
    {
        imageSource: "images/list/lifelines/ic_lifeline_50.png"
        title: qsTr("50/50") + Retranslate.onLanguageChanged
        
        onTriggered: {
            console.log("UserEvent: FiftyFifty");

            clock.stop();
            life.useFiftyFifty(adm, numericInput, listView.rearrangeHandler.active);
            sound.playLifeLineSelect();
        }
    }
    
    property variant popularOpinion: ActionItem
    {
        imageSource: "images/list/lifelines/ic_lifeline_audience.png"
        title: qsTr("Popular Opinion") + Retranslate.onLanguageChanged
        
        onTriggered: {
            console.log("UserEvent: PopularOpinion");

            clock.stop();
            var dialog = definition.init("PopularOpinionDialog.qml");
            dialog.open();
        }
    }
    
    property variant takeOne: ActionItem
    {
        imageSource: "images/list/lifelines/ic_lifeline_take_one.png"
        title: qsTr("Take One") + Retranslate.onLanguageChanged
        
        onTriggered: {
            console.log("UserEvent: TakeOne");
            
            clock.stop();
            life.useTakeOne(adm, numericInput, listView.rearrangeHandler.active);
            sound.playLifeLineSelect();
        }
    }
    
    property variant changeQuestion: ActionItem
    {
        imageSource: "images/list/lifelines/ic_lifeline_change.png"
        title: qsTr("Change Question") + Retranslate.onLanguageChanged
        
        onTriggered: {
            console.log("UserEvent: ChangeQuestion");
            
            clock.stop();
            examPage.nextQuestion();
            sound.playLifeLineSelect();
        }
    }
    
    property variant askExpert: ActionItem
    {
        imageSource: "images/list/lifelines/ic_lifeline_expert.png"
        title: qsTr("Ask an Expert") + Retranslate.onLanguageChanged
        
        onTriggered: {
            console.log("UserEvent: AskExpert");
            
            clock.stop();
            life.useAskExpert(adm, numericInput, listView.rearrangeHandler.active);
            sound.playLifeLineSelect();
        }
    }
    
    property variant freezeClock: ActionItem
    {
        title: qsTr("Freeze Time") + Retranslate.onLanguageChanged
        imageSource: "images/list/lifelines/ic_lifeline_clock.png"
        
        onTriggered: {
            console.log("UserEvent: FreezeTime");
            
            clock.stop();
            sound.playLifeLineSelect();
        }
    }
    
    onCreationCompleted: {
        examPage.addAction(fifty);
        examPage.addAction(popularOpinion);
        examPage.addAction(takeOne);
        examPage.addAction(changeQuestion);
        examPage.addAction(askExpert);
        examPage.addAction(freezeClock);
    }
}