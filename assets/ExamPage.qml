import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: examPage
    
    function cleanUp()
    {
        clock.stop();
        game.currentQuestionChanged.disconnect(onNewQuestion);
    }
    
    function nextQuestion()
    {
        var result = global.randomInt(QueryId.Unknown+1, QueryId.TempArgument1-1);
        game.nextQuestion(QueryId.NumericMaxVerseCount);
    }
    
    function onNewQuestion()
    {
        var current = game.currentQuestion;
        
        numericInput.visible = false;
        listView.visible = false;
        listView.rearrangeHandler.active = false;
        
        question.text = qb.getBody(current.id);
        
        if (game.multipleChoice)
        {
            adm.clear();
            adm.append(current.choices);
            
            listView.visible = true;
            
            if (current.ordered) {
                listView.rearrangeHandler.active = true;
            }
        } else if (game.numeric) {
            numericInput.visible = true;
        }
        
        clock.reset();
    }
    
    onCreationCompleted: {
        game.currentQuestionChanged.connect(onNewQuestion);
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties
        {
            Clock {
                id: clock
                
                onExpired: {
                    persist.showToast("POPPING");
                    navigationPane.pop();
                }
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
            
            function answered(correctly)
            {
                if (correctly) {
                    nextQuestion();
                } else {
                    persist.showToast( "You failedsh Mush!!", "images/bugs/ic_bugs_cancel.png" );
                    navigationPane.pop();
                }
            }
            
            onTriggered: {
                console.log("UserEvent: FinalANswer");
                
                clock.stop();
                sound.playUserInput();
                
                if (numericInput.visible)
                {
                    var input = numericInput.text.trim();
                    answered( input.length > 0 && parseInt(input) == game.currentQuestion.answer );
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
                
                dataModel: ArrayDataModel {
                    id: adm
                }
                
                onLayoutComplete: {
                    if (visible) {
                        clock.start();
                    }
                }
            }
            
            TextField
            {
                id: numericInput
                inputMode: TextFieldInputMode.NumbersAndPunctuation
                input.submitKey: SubmitKey.Submit
                input.onSubmitted: {
                    finalAnswer.triggered();
                }
                
                onVisibleChanged: {
                    opacity = 0;
                    
                    if (visible) {
                        ft.play();
                    } else {
                        resetText();
                    }
                }
                
                animations: [
                    FadeTransition {
                        id: ft
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