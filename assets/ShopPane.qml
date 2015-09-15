import bb.cascades 1.3
import com.canadainc.data 1.0

NavigationPane
{
    id: navigationPane
    
    onPopTransitionEnded: {
        deviceUtils.cleanUpAndDestroy(page);
    }
    
    Page
    {
        id: shopPage
        
        actions: [
            ActionItem
            {
                ActionBar.placement: ActionBarPlacement.Signature
                imageSource: "images/menu/ic_start.png"
                title: qsTr("Start") + Retranslate.onLanguageChanged
                
                onTriggered: {
                    var x = definition.init("ExamPage.qml");
                    x.nextQuestion();
                    
                    navigationPane.push(x);
                }
            }
        ]
        
        titleBar: TitleBar
        {
            title: qsTr("%n points", "", user.points) + Retranslate.onLanguageChanged
        }
        
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: shopBack.imagePaint
            
            ListView
            {
                id: listView
                property variant userModel: user
                
                dataModel: ArrayDataModel {
                    id: adm
                }
                
                listItemComponents: [
                    ListItemComponent
                    {
                        StandardListItem
                        {
                            enabled: ListItemData.price <= ListItem.view.userModel.points
                            imageSource: ListItemData.imageSource
                            title: ListItemData.title
                            description: ListItemData.description
                            status: ListItemData.price
                        }
                    }
                ]
                
                onTriggered: {
                    var data = dataModel.data(indexPath);
                    
                    if (data.price <= user.points)
                    {
                        if (data.type == "lifeline")
                        {
                            user.points = user.points-data.price;
                            adm.removeAt(indexPath[0]);
                            
                            var purchases = getPurchases();
                            
                            purchases[data.value] = 1;
                            persist.saveValueFor("purchases", purchases);
                            
                            persist.showToast( qsTr("Purchased %1 for %2 points!").arg(data.title).arg(data.price), data.imageSource );
                        }
                    } else {
                        persist.showToast( qsTr("Insufficient funds!"), "images/toast/insufficient_funds.png" );
                    }
                }
            }
        }
    }
    
    attachedObjects: [
        ImagePaintDefinition {
            id: shopBack
            imageSource: "images/backgrounds/welcome_bg.png"
            repeatPattern: RepeatPattern.XY
        }
    ]
    
    function getPurchases()
    {
        var purchases = persist.getValueFor("purchases");
        
        if (!purchases) {
            purchases = {};
        }
        
        return purchases;
    }
    
    function onLifelineUsed(key)
    {
        var purchases = getPurchases();
        var strValue = life.keyToString(key);
        
        if (strValue in purchases)
        {
            delete purchases[strValue];
            persist.saveValueFor("purchases", purchases);
        }
    }
    
    onCreationCompleted: {
        var purchases = getPurchases();
        
        if ( !(life.keyToString(Lifeline.AskAnExpert) in purchases) ) {
            adm.append({'title': qsTr("Ask an Expert"), 'description': qsTr("Always get the right answer!"), 'price': 200, 'imageSource': "images/list/lifelines/ic_lifeline_expert.png", 'value': life.keyToString(Lifeline.AskAnExpert), 'type': "lifeline"});
        }
        
        if ( !(life.keyToString(Lifeline.SecondChance) in purchases) ) {
            adm.append({'title': qsTr("Second Chance"), 'description': qsTr("Get two guesses to the correct answer!"), 'price': 3000, 'imageSource': "images/list/lifelines/ic_lifeline_second.png", 'value': life.keyToString(Lifeline.SecondChance), 'type': "lifeline"});
        }
        
        if ( !(life.keyToString(Lifeline.TakeOne) in purchases) ) {
            adm.append({'title': qsTr("Take One"), 'description': qsTr("Remove a wrong answer!"), 'price': 100, 'imageSource': "images/list/lifelines/ic_lifeline_take_one.png", 'value': life.keyToString(Lifeline.TakeOne), 'type': "lifeline"});
        }
        
        life.lifeLineUsed.connect(onLifelineUsed);
    }
}