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
                dataModel: shop.model
                property variant userModel: user
                
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
                        shop.purchase(data.value);
                        persist.showToast( qsTr("Purchased %1 for %2 points!").arg(data.title).arg(data.price), data.imageSource );
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
}