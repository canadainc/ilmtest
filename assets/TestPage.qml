import QtQuick 1.0
import bb.cascades 1.0
import bb.cascades.pickers 1.0
import bb.multimedia 1.0

BasePage
{
    id: examPage
    
    function setup(text, numeric)
    {
        answerAction.answer = undefined;
        clock.reset();

        question.text = text;
        listDelegate.delegateActive = !numeric;
        inputDelegate.delegateActive = numeric;

		sfx.value = "asset:///audio/question.mp3";
		sfx.play();
        clock.start();
        answerAction.duration = new Date();
    }

    function onNumericInputQ(text) {
        setup(text, true);
    }

    function onStandardQ(text) {
        setup(text, false);
    }
    
    function onNoQuestionsFound() {
        finish("No more questions found!");
    }
    
    function onError(message) {
        finish(message);
    }
    
    function finish(message)
    {
        persist.showToast( message, qsTr("OK") );
        
        if (navProperties.navPane.top == examPage) {
            navProperties.navPane.pop();
        } else {
            navProperties.navPane.remove(examPage);
            examPage.parent = undefined;
            examPage.destroy();
        }
    }

    onCreationCompleted: {
        game.numericInputQ.connect(onNumericInputQ);
        game.standardQ.connect(onStandardQ);
        game.noQuestionsFound.connect(onNoQuestionsFound);
        game.error.connect(onError);
    }
    
    paneProperties: NavigationPaneProperties {
        id: navProperties
        property variant navPane: navigationPane
    }
    
    actions: [
        ActionItem {
            id: answerAction
            
            property variant answer
            property variant duration
            title: qsTr("Final Answer") + Retranslate.onLanguageChanged
            imageSource: "images/ic_check.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onAnswerChanged: {
                enabled = answer != undefined;
            }
            
            onTriggered: {
                duration = new Date() - duration;
                sfx.value = "asset:///audio/inputted.mp3"
                sfx.play();
                enabled = false;
                clock.stop();
                
                silentTimer.start();
            }
        },
        
        ActionItem {
            title: qsTr("Ask the Audience") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_audience.png"
            
            onTriggered: {
                clock.stop();
                definition.source = "AskAudience.qml";
                var audience = definition.createObject();
                navProperties.navPane.push(audience);
            }
            
            function onUsed(stats) {
                enabled = false;
            }
            
            onCreationCompleted: {
                game.lifelineAskAudienceUsed.connect(onUsed);
            }
        },

        ActionItem {
            property bool alreadyUsed: false
            title: qsTr("50/50") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_50.png"
            enabled: !alreadyUsed && listDelegate.delegateActive
            
            onTriggered: {
                clock.stop();
                game.lifelineFiftyFifty();
            }
            
            function onUsed(stats) {
                alreadyUsed = true;
                sfx.value = "asset:///audio/sfx02.mp3"
                sfx.play();
            }
            
            onCreationCompleted: {
                game.lifelineFiftyFiftyUsed.connect(onUsed);
            }
        },

        ActionItem {
            title: qsTr("Ask a Friend") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_friend.png"
            
            onTriggered: {
                enabled = false;
                clock.stop();
                contactPicker.open();
            }
        },
        
        ActionItem {
            title: qsTr("Change Question") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_change.png"
            enabled: false
            
            onTriggered: {
                clock.stop();
                game.lifelineChangeQuestion();
            }
            
            function onUsed(available)
            {
                if (available) {
                    persist.showToast( qsTr("Change Question Lifeline now available!") );
                }
                
                enabled = available;
                sfx.value = "asset:///audio/sfx02.mp3"
                sfx.play();
            }
            
            onCreationCompleted: {
                game.lifelineChangeQuestionAvailable.connect(onUsed);
            }
        },
        
        ActionItem {
            property bool available: false
            title: qsTr("Take One") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_take_one.png"
            enabled: available && listDelegate.delegateActive
            
            onTriggered: {
                clock.stop();
                game.lifelineTakeOne();
            }
            
            function onUsed(availableToUse)
            {
                if (availableToUse) {
                    persist.showToast( qsTr("Take One Lifeline now available!") );
                }
                
                available = availableToUse;
                sfx.value = "asset:///audio/sfx02.mp3"
                sfx.play();
            }
            
            onCreationCompleted: {
                game.lifelineTakeOneAvailable.connect(onUsed);
            }
        },
        
        ActionItem {
            title: qsTr("Freeze Time") + Retranslate.onLanguageChanged
            imageSource: "images/ic_lifeline_clock.png"
            enabled: false
            
            onTriggered: {
                clock.stop();
                game.lifelineFreezeTime();
            }
            
            function onUsed(availableToUse)
            {
                if (availableToUse) {
                    persist.showToast( qsTr("Freeze Time Lifeline now available!") );
                }
                
                enabled = availableToUse;
                sfx.value = "asset:///audio/sfx02.mp3"
                sfx.play();
            }
            
            onCreationCompleted: {
                game.lifelineFreezeTimeAvailable.connect(onUsed);
            }
        }
    ]

    contentContainer: Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        Label {
            text: qsTr("Money: $%1").arg(game.money)
            textStyle.textAlign: TextAlign.Center
            horizontalAlignment: HorizontalAlignment.Fill
        }
        
        Clock {
            id: clock
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Top
            
            onReady: {
                game.loadNextLevel();
            }
            
            onExpired: {
                finish( qsTr("Out of time!") );
            }
        }

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            topPadding: 10
            leftPadding: 10
            rightPadding: 10
            bottomPadding: 10

            Label {
                id: question
                multiline: true
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.textAlign: TextAlign.Center
                textStyle.fontSize: FontSize.Medium
            }

            ControlDelegate {
                id: inputDelegate
                delegateActive: false

                sourceComponent: ComponentDefinition {
                    TextField {
                        id: numericField
                        hintText: qsTr("Enter numeric answer")
                        inputMode: TextFieldInputMode.PhoneNumber
                        
                        onTextChanging:
                        {
                            var parsedNumber;
                            
                            if (text.length > 0) {
                                parsedNumber = parseInt(text);
                                sfx.value = "asset:///audio/choice02.mp3"
                            } else {
                                sfx.value = "asset:///audio/choice03.mp3"
                            }

                            sfx.play();

                            answerAction.answer = parsedNumber;
                        }
                        
                        input.onSubmitted: {
                            if (numericField.text.length > 0) {
                                answerAction.triggered();
                            }
                        }
                        
                        attachedObjects: [
                            Timer {
                                id: focusTimer
                                interval: 200
                                running: true
                                repeat: false
                                
                                onTriggered: {
                                    numericField.requestFocus();
                                }
                            }
                        ]
                    }
                }
            }
        }

        ControlDelegate {
            delegateActive: listDelegate.delegateActive

            sourceComponent: ComponentDefinition {
                Divider {
                    topMargin: 0
                    bottomMargin: 0
                    leftMargin: 0
                    rightMargin: 0
                }
            }
        }

        ControlDelegate {
            id: listDelegate
            delegateActive: false

            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }

            sourceComponent: ComponentDefinition {
                ListView {
                    id: listView

                    dataModel: game.dataModel()

                    listItemComponents: [
                        ListItemComponent {
                            StandardListItem {
                                description: ListItemData.value
                                enabled: ListItemData.disabled == undefined;
                            }
                        }
                    ]
                    
                    onSelectionChanged:
                    {
                        var selectedItem;

                        if (selected) {
                            selectedItem = listView.dataModel.data(indexPath);
                        }

                        answerAction.answer = selectedItem ? selectedItem.value : undefined;
                    }

                    onTriggered:
                    {
                        if ( dataModel.data(indexPath).disabled == undefined ) {
                            clearSelection();
                            toggleSelection(indexPath);
                            sfx.value = "asset:///audio/choice01.mp3"
                            sfx.play();   
                        }
                    }

                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                }
            }
        }
    }
    
    attachedObjects: [
        MediaPlayer {
            property string value
            id: sfx
            sourceUrl: value
        },
        
        Timer {
            id: silentTimer
            repeat: false
            running: false
            interval: 2000
            
            onTriggered: {
                var result = game.validateAnswer(answerAction.answer, answerAction.duration);
                var answers = game.currentAnswer();
                var answer = answers[0];

                if (result) {
                    persist.showToast( qsTr("Correct!\n\n%1 - %2\n%3").arg(answer.author).arg(answer.title).arg(answer.section) );
                    
                    if (inputDelegate.delegateActive) {
                        inputDelegate.control.text = "";
                    } else if (listDelegate.delegateActive) {
                        listDelegate.control.clearSelection();
                    }
                    
                    game.loadNextLevel();
                } else {
                    finish( qsTr("Incorrect answer!\n\n%1 - %2\n%3").arg(answer.author).arg(answer.title).arg(answer.section));
                }
            }
        },
        
        ContactPicker {
            id: contactPicker
            
            onContactSelected: {
                definition.source = "AskFriend.qml";
                var friend = definition.createObject();
                friend.contactId = contactId;
                navProperties.navPane.push(friend);
            }
        },
        
        ComponentDefinition {
            id: definition
        }
    ]
}