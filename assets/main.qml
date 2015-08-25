import bb.cascades 1.3
import com.canadainc.data 1.0
import bb.multimedia 1.0
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
        
        onFinished: {
            nextQuestion();
        }
    }
    
    function onDataLoaded(id, data)
    {
        if (id == QueryId.FetchRandomVerseCount)
        {
            question.text = qsTr("How many verses does %1 contain?").arg(data[0].surah_name);
            var result = global.randomInt(1,2);
            
            if (result == 1)
            {
                data = offloader.generateChoices(data[0].verse_count);
                adm.clear();
                adm.append(data);
                
                numericInput.visible = false;
                listView.visible = true;
                listView.rearrangeHandler.active = false;
            } else {
                numericInput.answer = data[0].verse_count;
                
                numericInput.visible = true;
                listView.visible = false;
            }
        } else if (id == QueryId.FetchRandomSurahs || id == QueryId.FetchSurahsByRevealed) {
            listView.rearrangeHandler.active = false;
            adm.clear();
            adm.append(data);
            
            question.text = id == QueryId.FetchRandomSurahs ? qsTr("Please arrange the following surahs in order.") : qsTr("<html>Please arrange the following surahs in the <b>original order of revelation</b></html>");
            numericInput.visible = false;
            listView.visible = true;
            listView.rearrangeHandler.active = true;
        } else if (id == QueryId.FetchVersesForRandomSurah || id == QueryId.FetchRandomSajdaSurah) {
            listView.rearrangeHandler.active = false;
            listView.visible = true;
            data = offloader.mergeAndShuffle(data, tempData);
            adm.clear();
            adm.append(data);
            
            if (id == QueryId.FetchVersesForRandomSurah) {
                question.text = qsTr("Which of the following are verses found in %1?").arg(surahName);
            } else if (id == QueryId.FetchRandomSajdaSurah) {
                question.text = qsTr("Which of the following surahs contain a Sujud al-Tilawah (Prostration of Qu'ran Recitation)?");
            }
        } else if (id == QueryId.PendingQuery) {
            tempData = data;
        } else if (id == QueryId.FetchSurahHeader) {
            surahName = data[0].surah_name;
        } else if (id == QueryId.FetchRandomSurahLocation) {
            var type = data[0].type;
            surahName = data[0].surah_name;
            var i = global.randomInt(1,2);

            data = offloader.generateBooleanChoices(
                question,
                qsTr("%1 was revealed in Mecca").arg(surahName),
                i == 1 ? qsTr("%1 was revealed in Medina").arg(surahName) : qsTr("%1 was not revealed in Mecca").arg(surahName),
                qsTr("Was %1 revealed in Mecca?").arg(surahName),
                qsTr("Was %1 revealed in Medina?").arg(surahName),
                qsTr("%1 was revealed in").arg(surahName),
                qsTr("Mecca"),
                qsTr("Medina")
            );
            
            adm.clear();
            adm.append(data);
            listView.visible = true;
        } else {
            question.text = qsTr("Internal error! No question found~");
        }
    }
    
    property string surahName
    property variant tempData
    
    function nextQuestion()
    {
        var result = global.randomInt(1,5);
        var count = 0;
        
        switch (result)
        {
            case 1:
                quran.fetchVersesForRandomSurah(navigationPane);
                break;
            case 2:
                quran.fetchRandomVerseCount(navigationPane);
                break;
            case 3:
                quran.fetchRandomSajdaSurah(navigationPane);
                break;
            case 4:
                quran.fetchRandomSurahLocation(navigationPane);
                break;
            case 5:
                quran.fetchRandomSurahs(navigationPane, true);
                break;
            default:
                quran.fetchRandomSurahs(navigationPane);
                break;
        }
    }
    
    Page
    {
        id: mainPage
        
        actions: [
            ActionItem {
                title: qsTr("Accept")
                ActionBar.placement: ActionBarPlacement.Signature
                
                onTriggered: {
                    if (numericInput.visible)
                    {
                        numericInput.resetText();
                        
                        var input = numericInput.text.trim();
                        
                        if ( input.length > 0 && parseInt(input) == numericInput.answer ) {
                            nextQuestion();
                        }
                    } else if (listView.rearrangeHandler.active) {
                        if ( offloader.verifyOrdered(adm) ) {
                            nextQuestion();
                        }
                    } else {
                        if ( offloader.verifyMultipleChoice( adm, listView.selectionList() ) ) {
                            nextQuestion();
                        }
                    }
                }
            },
            
            ActionItem
            {
                title: qsTr("50/50") + Retranslate.onLanguageChanged

                onTriggered: {
                    console.log("UserEvent: FiftyFifty");
                    life.useFiftyFifty(adm);
                }
            }
        ]
        
        Container
        {
            layout: DockLayout {}
            
            Container
            {
                Label
                {
                    id: question
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Fill
                    textStyle.textAlign: TextAlign.Center
                }
                
                ListView
                {
                    id: listView
                    visible: false
                    property alias mediaPlayer: mp
                    
                    dataModel: ArrayDataModel {
                        id: adm
                    }
                    
                    rearrangeHandler
                    {
                        property int itemOrigin: -1
                        property int lastItemDestination: -1
                        
                        function undoItemMove()
                        {
                            if (lastItemDestination != -1 && itemOrigin != -1)
                            {
                                adm.move(lastItemDestination, itemOrigin);
                                itemOrigin = -1;
                                lastItemDestination = -1;
                            }
                        }
                        
                        onMoveUpdated: {
                            event.denyMove();
                            lastItemDestination = event.toIndexPath[0];
                            adm.move(event.fromIndexPath[0], event.toIndexPath[0]);
                        }
                        
                        onMoveEnded: {
                            mp.sourceUrl = "asset:///audio/choice03.mp3"
                            mp.play();
                        }
                        
                        onMoveAborted: {
                            undoItemMove();
                        }
                        
                        onMoveStarted: {
                            itemOrigin = event.startIndexPath[0];
                        }
                    }
                    
                    listItemComponents: [
                        ListItemComponent
                        {
                            StandardListItem
                            {
                                id: sli
                                enabled: ListItemData.disabled != 1
                                title: ListItemData.value.toString()
                                opacity: 0
                                
                                ListItem.onInitializedChanged: {
                                    if (initialized) {
                                        ft.play();
                                    }
                                }
                                
                                animations: [
                                    FadeTransition {
                                        id: ft
                                        fromOpacity: 0
                                        toOpacity: 1
                                        easingCurve: StockCurve.SineOut
                                        duration: 1000
                                        delay: sli.ListItem.indexPath[0] * 1000
                                        
                                        onEnded: {
                                            sli.ListItem.view.mediaPlayer.sourceUrl = "asset:///audio/choice03.mp3";
                                            sli.ListItem.view.mediaPlayer.play();
                                        }
                                    }
                                ]
                            }
                        }
                    ]
                    
                    onTriggered: {
                        if ( isSelected(indexPath) ) {
                            mp.sourceUrl = "asset:///audio/choice02.mp3"
                        } else {
                            mp.sourceUrl = "asset:///audio/choice01.mp3"
                        }
                        
                        mp.play();
                        toggleSelection(indexPath);
                    }
                }
                
                TextField
                {
                    id: numericInput
                    property int answer
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                    visible: false
                    input.submitKey: SubmitKey.Submit
                }
            }
            
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
        },
        
        MediaPlayer {
            id: mp
        }
    ]
}