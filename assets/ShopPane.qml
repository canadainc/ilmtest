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
            background: global.bgPaint
            
            ListView
            {
                id: listView
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
                
                onCreationCompleted: {
                    dataModel = shop.model;
                }
                
                onTriggered: {
                    var data = dataModel.data(indexPath);
                    var diff = user.points-data.price;
                    
                    if (diff >= 0)
                    {
                        shop.purchase(data.value);
                        reporter.record("PurchaseGood", data.value);
                        persist.showToast( qsTr("Purchased %1 for %2 points!").arg(data.title).arg(data.price), data.imageSource );
                    } else {
                        reporter.record("InsufficientFunds", diff.toString());
                        persist.showToast( qsTr("Insufficient funds!"), "images/toast/insufficient_funds.png" );
                    }
                }
            }
        }
    }
}