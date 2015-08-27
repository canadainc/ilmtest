import bb.cascades 1.0

BasePage
{
    property int contactId
    
    onContactIdChanged: {
        var details = app.contactDetails(contactId);
        avatar.imageSource = details.smallAvatar;
        result.contactName = details.displayName;
    }
    
    function onUsed(value, confidence)
    {
        result.value = value;
        result.confidence = confidence;
    }
    
    onCreationCompleted: {
        game.lifelineAskFriendUsed.connect(onUsed);
        game.lifelineAskFriend();
    }
    
    contentContainer: Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        bottomPadding: 20; topPadding: 20; rightPadding: 20; leftPadding: 20
        
        ImageView {
            id: avatar
            horizontalAlignment: HorizontalAlignment.Center
        }
        
        Label {
            property string contactName
            property int confidence;
            property variant value;
            multiline: true
            id: result
            horizontalAlignment: HorizontalAlignment.Fill
            textStyle.textAlign: TextAlign.Center
            text: qsTr("%1 says, 'I think the answer is %2. I'm %3% sure...'").arg(contactName).arg(value).arg(confidence);
        }
    }
}
