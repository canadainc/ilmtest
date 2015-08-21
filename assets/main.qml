import bb.cascades 1.3
import com.canadainc.data 1.0
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
            question.text = qsTr("How many verses does %1 (%2) contain?").arg(data[0].name).arg(data[0].transliteration);
            var result = global.randomInt(1,2);
            
            if (result == 1)
            {
                data = offloader.generateChoices(data[0].verse_count);
                adm.clear();
                adm.append(data);
                
                numericInput.visible = false;
                listView.visible = true;
            } else {
                numericInput.answer = data[0].verse_count;
                
                numericInput.visible = true;
                listView.visible = false;
            }
        } else {
            question.text = qsTr("Internal error! No question found~");
        }
    }
    
    function nextQuestion() {
        quran.fetchRandomVerseCount(navigationPane);
    }
    
    Page
    {
        id: mainPage
        
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
                    
                    dataModel: ArrayDataModel {
                        id: adm
                    }
                    
                    listItemComponents: [
                        ListItemComponent
                        {
                            StandardListItem
                            {
                                title: ListItemData.value.toString()
                            }
                        }
                    ]
                    
                    onTriggered: {
                        if ( dataModel.data(indexPath).correct ) {
                            nextQuestion();
                        }
                    }
                }
                
                TextField
                {
                    id: numericInput
                    property int answer
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                    visible: false
                    
                    validator: Validator
                    {
                        errorMessage: qsTr("Incorrect!")
                        mode: ValidationMode.Custom
                        
                        onValidate: {
                            valid = numericInput.text.length > 0 && parseInt(numericInput.text) == numericInput.answer;
                        }
                    }
                    
                    input.submitKey: SubmitKey.Submit
                    input.onSubmitted: {
                        console.log("UserEvent: NumericAnswerSubmitted", numericInput.text);
                        validator.validate();
                        
                        if (validator.valid)
                        {
                            numericInput.resetText();
                            nextQuestion();
                        }
                    }
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
        }
    ]
}