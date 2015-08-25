import bb.cascades 1.3

Page
{
    titleBar: TitleBar
    {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties
        {
            UserTitleBanner
            {
                onBannerTapped: {
                    var p = definition.init("UserProfilePage.qml");
                    navigationPane.push(p);
                }
            }
        }
    }
    
    Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        ListView
        {
            id: listView
            
            dataModel: ArrayDataModel {
                id: adm
            }
            
            listItemComponents: [
                ListItemComponent
                {
                    StandardListItem {
                        title: ListItemData.title
                    }
                }
            ]
            
            onTriggered: {
                var p = definition.init("ExamPage.qml");
                navigationPane.push(p);
                
                p.nextQuestion();
            }
            
            onCreationCompleted: {
                adm.append({'title': qsTr("Play")})
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