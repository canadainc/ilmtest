import QtQuick 1.0
import bb.cascades 1.3
import com.canadainc.data 1.0

Page
{
    id: createUser
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarFollowKeyboardPolicy: ActionBarFollowKeyboardPolicy.Never
    property variant userId
    property alias name: tftk.textField
    signal saveProfile(variant id, string name, string kunya, bool female)
    
    function cleanUp() {}
    
    onUserIdChanged: {
        if (userId) {
            user.fetchProfile(createUser, userId);
        } else {
            timer.running = true;
        }
    }
    
    function onDataLoaded(id, data)
    {
        if (id == QueryId.FetchProfile && data.length > 0)
        {
            var profile = data[0];
            
            name.text = profile.name;
            kunya.text = profile.kunya;
            female.checked = profile.female;
        }
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.TextField
        
        dismissAction: ActionItem
        {
            imageSource: "images/title/ic_save_profile.png"
            title: qsTr("Save") + Retranslate.onLanguageChanged
            
            onTriggered: {
                console.log("UserEvent: SaveProfile");

                nameValidator.validate();
                kunyaValidator.validate();
                
                if (nameValidator.valid && kunyaValidator.valid) {
                    saveProfile( userId, name.text.trim(), kunya.text.trim(), female.checked );
                }
            }
        }
        
        kindProperties: TextFieldTitleBarKindProperties
        {
            id: tftk
            
            textField {
                hintText: qsTr("Your Name...") + Retranslate.onLanguageChanged
                horizontalAlignment: HorizontalAlignment.Fill
                content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                input.flags: TextInputFlag.SpellCheckOff | TextInputFlag.AutoPeriodOff | TextInputFlag.AutoCorrectionOff
                input.keyLayout: KeyLayout.Contact
                inputMode: TextFieldInputMode.Text
                input.submitKey: SubmitKey.Next
                input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                
                validator: Validator
                {
                    id: nameValidator
                    errorMessage: qsTr("Invalid name") + Retranslate.onLanguageChanged
                    mode: ValidationMode.FocusLost
                    
                    onValidate: { 
                        valid = name.text.trim().length > 2;
                    }
                }
            }
        }
    }
    
    Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: global.bgPaint
        
        ScrollView
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            Container
            {
                topPadding: 10
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                Container
                {
                    topMargin: 10; rightPadding: 10; leftPadding: 20
                    
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    TextField
                    {
                        id: kunya
                        hintText: qsTr("Kunya (ie: Abu 'Ubayd)...") + Retranslate.onLanguageChanged
                        horizontalAlignment: HorizontalAlignment.Fill
                        content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff
                        input.flags: TextInputFlag.SpellCheckOff | TextInputFlag.AutoPeriodOff | TextInputFlag.AutoCorrectionOff
                        input.submitKey: SubmitKey.Next
                        input.keyLayout: KeyLayout.Contact
                        inputMode: TextFieldInputMode.Text
                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                        
                        validator: Validator
                        {
                            id: kunyaValidator
                            errorMessage: qsTr("Invalid nickname") + Retranslate.onLanguageChanged
                            mode: ValidationMode.FocusLost
                            
                            onValidate: { 
                                valid = kunya.text.trim().length > 4;
                            }
                        }
                    }
                    
                    CheckBox
                    {
                        id: female
                        text: qsTr("Female") + Retranslate.onLanguageChanged
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    }
    
    attachedObjects: [
        Timer {
            id: timer
            interval: 250
            running: false
            repeat: false
            
            onTriggered: {
                name.requestFocus();
            }
        }        
    ]
}