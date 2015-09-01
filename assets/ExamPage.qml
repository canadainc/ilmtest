import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: examPage
    property string surahName
    property variant tempData
    property variant numericDB
    signal answerPending(bool numeric, bool ordered, variant data);
    signal answered(bool correctly);
    
    function cleanUp() {
        clock.stop();
    }
    
    function nextQuestion()
    {
        var result = global.randomInt(QueryId.Unknown+1, QueryId.PendingQuery-1);
        game.nextQuestion(examPage, QueryId.FetchRandomSurahsByLength);
    }
    
    onAnswerPending: {
        listView.rearrangeHandler.active = false;
        listView.visible = !numeric;
        
        if (listView.visible)
        {
            adm.clear();
            adm.append(data);
        }
        
        numericInput.resetText();
        numericInput.visible = numeric;
        listView.rearrangeHandler.active = ordered;
        
        clock.reset();
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
    
    function processNumeric(answerValue)
    {
        var result = global.randomInt(1,2);
        var data;
        
        if (result == 1) {
            data = offloader.generateChoices(answerValue);
        } else {
            numericInput.answer = answerValue;
        }
        
        answerPending(result > 1, false, data);
    }
    
    function onDataLoaded(id, data)
    {
        if (data.length == 0) {
            nextQuestion();
        } else {
            if ( qb.isNumeric(id) )
            {
                var x = qb.getBody(id);
                
                if (id == QueryId.FetchRandomVerseCount) {
                    x = x.arg(data[0].surah_name);
                }
                
                question.text = x;
                processNumeric(data[0].total_count);
            } else if ( qb.isOrdered(id) ) {
                var x = qb.getBody(id);
                
                if (id == QueryId.FetchSurahRandomVerses) {
                    x = x.arg(surahName);
                }
                
                question.text = x;
                answerPending(false, true, data);
            } else if ( qb.isStandard(id) ) {
                data = offloader.mergeAndShuffle(data, tempData);
                var x = qb.getBody(id);
                
                if (id == QueryId.FetchVersesForRandomSurah) {
                    x = x.arg(surahName);
                }
                
                question.text = x;
                answerPending(false, false, data);
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
                
                answerPending(false, false, data);
            } else if (id == QueryId.PendingQuery) {
                tempData = data;
            } else if (id == QueryId.FetchSurahHeader) {
                surahName = data[0].surah_name;
            } else {
                question.text = qsTr("Internal error! No question found~");
            }
        }
    }
    
    actions: [
        ActionItem
        {
            id: finalAnswer
            imageSource: "images/menu/ic_check.png"
            title: qsTr("Accept")
            ActionBar.placement: ActionBarPlacement.Signature
            
            onTriggered: {
                clock.stop();
                sound.playUserInput();
                
                if (numericInput.visible)
                {
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
            
            MultipleChoiceListView
            {
                id: listView
                visible: false
                
                dataModel: ArrayDataModel {
                    id: adm
                }
                
                onLayoutComplete: {
                    clock.start();
                }
            }
            
            TextField
            {
                id: numericInput
                property int answer
                inputMode: TextFieldInputMode.NumbersAndPunctuation
                visible: false
                input.submitKey: SubmitKey.Submit
                input.onSubmitted: {
                    finalAnswer.triggered();
                }
                
                onVisibleChanged: {
                    opacity = 0;
                }
                
                animations: [
                    FadeTransition {
                        fromOpacity: 0
                        toOpacity: 1
                        duration: 1000
                        easingCurve: StockCurve.ExponentialOut
                        
                        onEnded: {
                            clock.start();
                        }
                    }
                ]
            }
        }
    }
    
    attachedObjects: [
        LifeLinePane {
            id: llp
        },
        
        QuestionBank {
            id: qb
        }
    ]
}