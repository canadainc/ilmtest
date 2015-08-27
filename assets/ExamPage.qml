import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: examPage
    property string surahName
    property variant tempData
    signal answerPending(bool numeric, bool ordered);
    signal answered(bool correctly);
    
    function cleanUp() {
        clock.stop();
    }
    
    onAnswerPending: {
        listView.rearrangeHandler.active = false;
        
        listView.visible = !numeric;
        numericInput.visible = numeric;
        listView.rearrangeHandler.active = ordered;
        
        clock.reset();
        clock.start();
    }
    
    onAnswered: {
        if (correctly) {
            nextQuestion();
        } else {
            persist.showToast( "You failedsh Mush!!", "images/bugs/ic_bugs_cancel.png" );
            navigationPane.pop();
        }
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties
        {
            Clock {
                id: clock
                
                onExpired: {
                    navigationPane.pop();
                }
            }
        }
    }
    
    function onDataLoaded(id, data)
    {
        if (data.length == 0) {
            nextQuestion();
        }
        
        if (id == QueryId.FetchRandomVerseCount)
        {
            question.text = qsTr("How many verses does %1 contain?").arg(data[0].surah_name);
            var result = global.randomInt(1,2);
            
            if (result == 1)
            {
                data = offloader.generateChoices(data[0].verse_count);
                adm.clear();
                adm.append(data);
            } else {
                numericInput.answer = data[0].verse_count;
            }
            
            answerPending(result == 1, false);
        } else if (id == QueryId.FetchRandomSurahs || id == QueryId.FetchSurahsByRevealed || id == QueryId.FetchSurahRandomVerses) {
            listView.rearrangeHandler.active = false;
            adm.clear();
            adm.append(data);
            
            if (id == QueryId.FetchRandomSurahs) {
                question.text = qsTr("Please arrange the following surahs in order.");
            } else if (id == QueryId.FetchSurahsByRevealed) {
                question.text = qsTr("<html>Please arrange the following surahs in the <b>original order of revelation</b></html>");
            } else {
                question.text = qsTr("Please arrange the following verses from %1 in order").arg(surahName);
            }

            answerPending(false, true);
        } else if (id == QueryId.FetchVersesForRandomSurah || id == QueryId.FetchRandomSajdaSurah) {
            data = offloader.mergeAndShuffle(data, tempData);
            adm.clear();
            adm.append(data);
            
            if (id == QueryId.FetchVersesForRandomSurah) {
                question.text = qsTr("Which of the following are verses found in %1?").arg(surahName);
            } else if (id == QueryId.FetchRandomSajdaSurah) {
                question.text = qsTr("Which of the following surahs contain a Sujud al-Tilawah (Prostration of Qu'ran Recitation)?");
            }
            
            answerPending(false, false);
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

            answerPending(false, false);
        } else {
            question.text = qsTr("Internal error! No question found~");
        }
    }
    
    function nextQuestion()
    {
        var result = global.randomInt(1,7);
        
        switch (result)
        {
            case 1:
                quran.fetchVersesForRandomSurah(examPage);
                break;
            case 2:
                quran.fetchRandomVerseCount(examPage);
                break;
            case 3:
                quran.fetchRandomSajdaSurah(examPage);
                break;
            case 4:
                quran.fetchRandomSurahLocation(examPage);
                break;
            case 5:
                quran.fetchRandomSurahs(examPage, true);
                break;
            case 6:
                quran.fetchSurahRandomVerses(examPage);
                break;
            default:
                quran.fetchRandomSurahs(examPage);
                break;
        }
    }
    
    actions: [
        ActionItem
        {
            imageSource: "images/menu/ic_check.png"
            title: qsTr("Accept")
            ActionBar.placement: ActionBarPlacement.Signature
            
            onTriggered: {
                clock.stop();
                sound.playUserInput();
                
                if (numericInput.visible)
                {
                    numericInput.resetText();
                    
                    var input = numericInput.text.trim();
                    answered( input.length > 0 && parseInt(input) == numericInput.answer );
                } else if (listView.rearrangeHandler.active) {
                    answered( offloader.verifyOrdered(adm) );
                } else {
                    answered( offloader.verifyMultipleChoice( adm, listView.selectionList() ) );
                }
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
            
            Divider {
                bottomMargin: 0
            }
            
            ListView
            {
                id: listView
                visible: false
                property variant soundManager: sound
                
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
                        sound.playPresentChoice();
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
                            description: ListItemData.description ? ListItemData.description : ""
                            imageSource: "images/list/choices/%1.png".arg(ListItem.indexPath[0])
                            opacity: 0
                            
                            ListItem.onInitializedChanged: {
                                if (initialized)
                                {
                                    opacity = 0;
                                    ft.play();
                                }
                            }
                            
                            onOpacityChanged: {
                                if (opacity == 1) {
                                    sli.ListItem.view.soundManager.playPresentChoice();
                                }
                            }
                            
                            animations: [
                                FadeTransition {
                                    id: ft
                                    fromOpacity: 0
                                    toOpacity: 1
                                    easingCurve: StockCurve.SineOut
                                    duration: global.presentAnimSpeed
                                    delay: sli.ListItem.indexPath[0] * global.presentAnimSpeed
                                }
                            ]
                        }
                    }
                ]
                
                onTriggered: {
                    if ( isSelected(indexPath) ) {
                        sound.playDeselect();
                    } else {
                        sound.playSelectChoice();
                    }
                    
                    toggleSelection(indexPath);
                    xyz.open();
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
    }
    
    attachedObjects: [
        LifeLinePane {
            
        }
    ]
}