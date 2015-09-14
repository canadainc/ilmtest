import bb.cascades 1.3
import com.canadainc.data 1.0

QtObject
{
    function cleanUp()
    {
        life.lifeLineAvailable.disconnect(onAvailable);
        life.lifeLineUsed.disconnect(onUsed);
    }
    
    property variant actionDef: ComponentDefinition
    {
        ActionItem
        {
            property int key
            
            onTriggered: {
                console.log( "UserEvent: "+life.keyToString(key) );
                
                clock.stop();
                sound.playLifeLineSelect();
                life.useLifeline(key, listView.dataModel, numericInput, listView.rearrangeHandler.active);
            }
        }
    }
    
    function onAvailable(title, image, key)
    {
        var x = actionDef.createObject();
        x.key = key;
        x.title = title;
        x.imageSource = image;
        
        examPage.addAction(x);
    }
    
    function onUsed(key)
    {
        for (var i = examPage.actionCount()-1; i >= 0; i--)
        {
            var current = examPage.actionAt(i);
            
            if (current.key == key) {
                examPage.removeAction(current);
                break;
            }
        }
    }
    
    onCreationCompleted: {
        life.lifeLineAvailable.connect(onAvailable);
        life.lifeLineUsed.connect(onUsed);
    }
}