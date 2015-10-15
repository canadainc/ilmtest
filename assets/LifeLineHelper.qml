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
            enabled: finalAnswer.enabled
            
            onTriggered: {
                console.log( "UserEvent: "+life.keyToString(key) );
                
                if (key == Lifeline.ExtendTime) {
                    clock.currentValue += 30;
                } else {
                    clock.stop();
                }
                
                sound.playLifeLineSelect();
                user.recordLifeLine(key);

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
    
    function onUsed(key, data)
    {
        for (var i = examPage.actionCount()-1; i >= 0; i--)
        {
            var current = examPage.actionAt(i);
            
            if (current.key == key) {
                examPage.removeAction(current);
                break;
            }
        }
        
        if (key == Lifeline.PopularOpinion)
        {
            definition.source = "PopularOpinionDialog.qml";
            var x = definition.createObject();
            x.statistics = data;
            x.open();
        } else if (key == Lifeline.PhoneFriend) {
            var choices = [];
            deviceUtils.log(data);
            
            for (var i = 0; i < data.length; i++) {
                choices.push(data[i].value);
            }
            
            var message = qsTr("%1 are the correct ones I think. I'm %2% sure...").arg( choices.join(", ") ).arg(data[0].ratio);
            
            if (choices.length == 1) {
                message = qsTr("%1 is the correct one I believe. I'm %2% sure...").arg( choices.join(", ") ).arg(data[0].ratio);
            }
            
            persist.showDialog( qsTr("Phone a Friend"), message );
        } else if (key == Lifeline.ChangeQuestion) {
            examPage.nextQuestion();
        }
    }
    
    onCreationCompleted: {
        life.lifeLineAvailable.connect(onAvailable);
        life.lifeLineUsed.connect(onUsed);
    }
}