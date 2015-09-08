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
        
        if ( persist.getValueFor("customOnly") == 1 )
        {
            result = global.randomInt(QueryId.CustomAfterQuestion, QueryId.CustomStandardQuestion);
            
            if (result >= 2 && result <= 3) {
                result = 4;
            }
        }
        
        var formatFlag = global.randomInt(QueryId.MultipleChoice, QueryId.TextInput);
        var truthFlag = global.randomInt(QueryId.GenerateTruth, QueryId.GenerateFalsehood);
        game.nextQuestion(result, formatFlag, truthFlag);
    }
    
    function onNewQuestion()
    {
        numericInput.reset();
        listView.reset();
        
        var current = game.currentQuestion;
        var bodies = qb.getBodies(current.id);
        
        if (current.question) {
            question.text = current.question;
        } else if (!game.booleanQuestion && bodies) {
            bodies = bodies.choiceTexts;
            question.text = game.formatQuestion( bodies[ global.randomInt(0, bodies.length-1) ] );
        } else if (game.booleanQuestion) {
            current.choices = offloader.generateBooleanChoices(question, bodies.trueStrings, bodies.falseStrings, bodies.truePrompts, bodies.falsePrompts, bodies.choiceTexts, bodies.corrects, bodies.incorrects, game.arg1);
        }
        
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
                    user.points = user.points+1;
                    nextQuestion();
                } else {
                    persist.showToast( "You failed!!", "images/bugs/ic_bugs_cancel.png" );
                    navigationPane.pop();
                }
            }
            
            onTriggered: {
                console.log("UserEvent: FinalAnswer");
                
                if (numericInput.visible)
                {
                    numericInput.validator.validate();
                    
                    if (!numericInput.validator.valid) {
                        return;
                    }
                }

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
                opacity: 0
                
                onTextChanged: {
                    opacity = 0;
                    questionAnim.play();
                }
                
                animations: [
                    FadeTransition
                    {
                        id: questionAnim
                        fromOpacity: 0
                        toOpacity: 1
                        easingCurve: StockCurve.CircularOut
                        duration: 500
                        
                        onStarted: {
                            sound.playPresentQuestion();
                        }
                    }
                ]
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
                
                validator: Validator
                {
                    id: numericValidator
                    errorMessage: qsTr("Only digits can be entered!") + Retranslate.onLanguageChanged
                    mode: ValidationMode.FocusLost
                    
                    onValidate: {
                        valid = /^\d+$/.test( numericInput.text.trim() );
                    }
                }
                
                function reset()
                {
                    visible = false;
                    loseFocus();
                    resetText();
                    hintText = qsTr("Enter a numeric value");
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