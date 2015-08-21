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
            
            if (result == 1 || true)
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
        } else if (id == QueryId.FetchRandomSurahs) {
            listView.rearrangeHandler.active = false;
            adm.clear();
            adm.append(data);
            
            question.text = qsTr("Please arrange the following surahs in order.");
            numericInput.visible = false;
            listView.visible = true;
            listView.rearrangeHandler.active = true;
        } else {
            question.text = qsTr("Internal error! No question found~");
        }
    }
    
    function nextQuestion()
    {
        var result = global.randomInt(1,2);
        
        if (result == 1 && false) {
            quran.fetchRandomVerseCount(navigationPane);
        } else {
            quran.fetchRandomSurahs(navigationPane);
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
                        var sorted = true;
                        
                        for (var i = 0; i < adm.size()-1; i++)
                        {
                            if (adm.value(i).sort_order > adm.value(i+1).sort_order) {
                                sorted = false;
                                break;
                            }
                        }
                        
                        if (sorted) {
                            nextQuestion();
                        }
                    } else {
                        var all = listView.selected();
                        
                        if (all && all.length > 0 && adm.data(all).correct) {
                            nextQuestion();
                        }
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
                
                ListView
                {
                    id: listView
                    visible: false
                    
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
                            StandardListItem {
                                title: ListItemData.value.toString()
                            }
                        }
                    ]
                    
                    onTriggered: {
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
        }
    ]
}