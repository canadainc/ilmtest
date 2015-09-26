import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: pickerPage
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    signal picked(variant userId)
    
    onCreationCompleted: {
        user.fetchAllProfiles(listView);
    }
    
    titleBar: TitleBar
    {
        title: qsTr("Change Profile") + Retranslate.onLanguageChanged
    }
    
    actions: [
        ActionItem
        {
            id: addAction
            imageSource: "images/menu/ic_add_user.png"
            title: qsTr("Add") + Retranslate.onLanguageChanged
            ActionBar.placement: 'Signature' in ActionBarPlacement ? ActionBarPlacement["Signature"] : ActionBarPlacement.OnBar
            
            shortcuts: [
                SystemShortcut {
                    type: SystemShortcuts.CreateNew
                }
            ]
            
            function onCreate(id, name, kunya, female)
            {
                var result = user.createProfile(name, kunya, female);
                
                if (result.id)
                {
                    adm.insert(0, result);
                    persist.showToast( qsTr("Successfully added user"), imageSource.toString() );
                    listView.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Smooth);
                    
                    while (navigationPane.top != pickerPage) {
                        navigationPane.pop();
                    }
                }
            }
            
            onTriggered: {
                console.log("UserEvent: NewUser");
                definition.source = "CreateProfilePage.qml";
                var page = definition.createObject();
                page.saveProfile.connect(onCreate);
                
                navigationPane.push(page);
            }
        }
    ]
    
    Container
    {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: Color.create("#1e2127")
        
        ListView
        {
            id: listView
            scrollRole: ScrollRole.Main
            
            dataModel: ArrayDataModel {
                id: adm
            }
            
            listItemComponents: [
                ListItemComponent
                {
                    StandardListItem
                    {
                        id: sli
                        imageSource: ListItemData.female == 1 ? "images/title/ic_female.png" : "images/title/ic_male.png"
                        title: ListItemData.name
                        description: ListItemData.kunya
                        status: ListItemData.points ? ListItemData.points.toString() : ""
                    }
                }
            ]
            
            function onDataLoaded(id, data)
            {
                if (id == QueryId.FetchAllProfiles)
                {
                    adm.clear();
                    adm.append(data);
                }
            }
            
            onTriggered: {
                picked( dataModel.data(indexPath).id );
            }
        }
    }
    
    function cleanUp() {}
}
