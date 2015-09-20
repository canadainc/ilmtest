import bb.cascades 1.3

ListView
{
    property variant soundManager: sound
    signal layoutComplete()
    
    function reset()
    {
        visible = false;
        rearrangeHandler.active = false;
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
        
        onMoveEnded: {
            sound.playPresentChoice();
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
            StandardListItem
            {
                id: sli
                enabled: ListItemData.disabled != 1
                title: ListItemData.value.toString()
                description: ListItemData.description ? ListItemData.description : ""
                imageSource: ListItemData.none ? "images/list/choices/none.png" : "images/list/choices/%1.png".arg(ListItem.indexPath[0])
                opacity: 0
                
                ListItem.onInitializedChanged: {
                    if (initialized)
                    {
                        opacity = 0;
                        ft.play();
                    }
                }
                
                onOpacityChanged: {
                    if (opacity == 1) {
                        sli.ListItem.view.soundManager.playPresentChoice();
                    }
                }
                
                animations: [
                    FadeTransition {
                        id: ft
                        fromOpacity: 0
                        toOpacity: 1
                        easingCurve: StockCurve.SineOut
                        duration: global.presentAnimSpeed
                        delay: sli.ListItem.indexPath[0] * global.presentAnimSpeed
                        
                        onEnded: {
                            if ( sli.ListItem.indexPath[0] == sli.ListItem.view.dataModel.size()-1 ) {
                                sli.ListItem.view.layoutComplete();
                            }
                        }
                    }
                ]
            }
        }
    ]
    
    onTriggered: {
        if (!rearrangeHandler.active)
        {
            if (game.numeric || game.booleanQuestion) {
                clearSelection();
            }
            
            if ( isSelected(indexPath) ) {
                sound.playDeselect();
            } else {
                sound.playSelectChoice();
            }
            
            toggleSelection(indexPath);
        }
    }
}