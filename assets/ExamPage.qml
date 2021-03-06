import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: examPage
    
    function cleanUp()
    {
        user.numPlayed = user.numPlayed+1;
        user.highestLevel = game.level;
        user.commitChanges();
        
        clock.stop();
        llp.cleanUp();
        game.currentQuestionChanged.disconnect(onNewQuestion);
        game.reset();
        sound.stopMainLoop();
    }
    
    function nextQuestion(sameLevel)
    {
        var result = global.randomInt(QueryId.Unknown+1, QueryId.TempArgument1-1);
        
        if ( persist.getValueFor("customOnly") == 1 ) {
            result = global.randomInt(QueryId.CustomBoolCountQuestion, QueryId.CustomStandardQuestion);
        }
        
        var formatFlag = global.randomInt(QueryId.MultipleChoice, QueryId.TextInput);
        var truthFlag = global.randomInt(QueryId.GenerateTruth, QueryId.GenerateFalsehood);
        game.nextQuestion(result, formatFlag, truthFlag, sameLevel);
    }
    
    function onNewQuestion()
    {
        numericInput.reset();
        listView.reset();
        sound.playMainLoop();
        reference.translationY = 300;
        mainContainer.opacity = 1;
        reference.enabled = false;
        llp.secondChanceActive = false;
        
        var current = game.currentQuestion;
        var bodies = qb.getBodies(current.type);
        
        if (current.reference) {
            reference.apply(current.reference);
        }
        
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
    
    function pendingInput()
    {
        finalAnswer.enabled = true;
        clock.start();
        
        if ( game.currentQuestion.reference && !refAnim.isPlaying() && !refAnim.isStarted() ) {
            refAnim.play();
        }
        
        if (numericInput.visible) {
            numericInput.requestFocus();
        }
    }
    
    function answered(correctly)
    {
        if (correctly)
        {
            sound.playCorrect();
            
            persist.showToast( qsTr("Correct!"), "images/toast/ic_correct.png" );
            user.points = user.points+game.level;
            
            if ( !restore.isPlaying() && !restore.isStarted() ) {
                restore.play();
            }
        } else {
            sound.playIncorrect();

            if (llp.secondChanceActive) {
                persist.showToast( qsTr("Your answer was incorrect! Here is your second chance!"), "images/list/lifelines/ic_lifeline_second.png" );
                onNewQuestion();
            } else {
                persist.showToast( qsTr("You failed!"), "images/toast/ic_incorrect.png" );
                reference.enabled = true;
                
                if (shop.isExposePurchased)
                {
                    shop.refundPlugin(Plugin.ExposeAnswer);
                    listView.expose = numericInput.expose = true;
                }
            }
        }
        
        user.recordStats( adm, listView.selectionList(), numericInput, clock.savedElapsed, correctly );
    }
    
    onCreationCompleted: {
        game.currentQuestionChanged.connect(onNewQuestion);
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties
        {
            Container
            {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                layout: DockLayout {}
                
                Clock
                {
                    id: clock
                    
                    onExpired: {
                        reporter.record("ClockExpired", game.currentQuestion.id);
                        
                        persist.showToast( qsTr("Time expired! You failed!"), "images/toast/ic_expired.png" );
                        navigationPane.pop();
                    }
                }
                
                Label
                {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                    textStyle.fontWeight: FontWeight.Bold
                    text: qsTr("Level: %1").arg(game.level) + Retranslate.onLanguageChanged
                    visible: clock.scaleX == 0
                    scaleX: 1.2
                    scaleY: 1.2
                    
                    onVisibleChanged: {
                        if (visible) {
                            scaler.play();
                        } else {
                            scaleX = scaleY = 1.2;
                        }
                    }
                    
                    animations: [
                        ScaleTransition {
                            id: scaler
                            fromX: 1.2
                            fromY: 1.2
                            toX: 1
                            toY: 1
                            easingCurve: StockCurve.DoubleElasticIn
                            duration: 1000
                        }
                    ]
                }
            }
        }
    }
    
    actions: [
        ActionItem
        {
            id: finalAnswer
            imageSource: "images/menu/ic_check.png"
            title: qsTr("Accept") + Retranslate.onLanguageChanged
            ActionBar.placement: ActionBarPlacement.Signature
            enabled: false
            
            onTriggered: {
                console.log("UserEvent: FinalAnswer");
                clock.savedElapsed = clock.elapsed();
                reporter.record("FinalAnswer");
                
                if (numericInput.visible)
                {
                    numericInput.validator.validate();
                    
                    if (!numericInput.validator.valid) {
                        reporter.record( "InvalidNumericInput", numericInput.text.trim() );
                        return;
                    }
                    
                    numericInput.loseFocus();
                }

                enabled = false;
                numericInput.enabled = false;
                clock.stop();
                sound.stopMainLoop();
                sound.playUserInput();
                
                if ( !suspense.isPlaying() && !suspense.isStarted() ) {
                    suspense.play();
                }
            }
        }
    ]
    
    Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: Color.create("#1e2127")
        layout: DockLayout {}
        
        Container
        {
            id: mainContainer
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            topPadding: 10
            
            animations: [
                FadeTransition
                {
                    id: suspense
                    fromOpacity: 1
                    toOpacity: 0.5
                    duration: global.suspenseDuration
                    easingCurve: StockCurve.ExponentialOut
                    
                    onEnded: {
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
                },
                
                FadeTransition
                {
                    id: restore
                    fromOpacity: 0.5
                    toOpacity: 1
                    duration: global.suspenseDuration
                    easingCurve: StockCurve.QuarticOut
                    
                    onEnded: {
                        nextQuestion();
                    }
                }
            ]
            
            Container
            {
                horizontalAlignment: HorizontalAlignment.Fill
                leftPadding: 10; rightPadding: 10
                
                Label
                {
                    id: question
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Fill
                    textStyle.textAlign: TextAlign.Center
                    textStyle.fontSize: FontSize.Large
                    opacity: 0
                    
                    onTextChanged: {
                        opacity = 0;
                        
                        if ( !questionAnim.isStarted() && !questionAnim.isPlaying() ) {
                            questionAnim.play();
                        }
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
                        pendingInput();
                    }
                }
            }
            
            NumericInputField
            {
                id: numericInput

                input.onSubmitted: {
                    reporter.record("NumericInputReturn");
                    finalAnswer.triggered();
                }
                
                anim.onEnded: {
                    pendingInput();
                }
            }
        }

        QuestionReference
        {
            id: reference
            translationY: 300
            
            animations: [
                TranslateTransition
                {
                    id: refAnim
                    fromY: 300
                    toY: 0
                    easingCurve: StockCurve.CircularInOut
                    duration: 1000
                    
                    onEnded: {
                        reference.show();
                    }
                }
            ]
        }
    }
    
    attachedObjects: [
        LifeLineHelper {
            id: llp
        },
        
        QuestionBank {
            id: qb
        }
    ]
}