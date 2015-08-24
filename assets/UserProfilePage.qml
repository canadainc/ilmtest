import QtQuick 1.0
import bb.cascades 1.3
import bb.system 1.2
import com.canadainc.data 1.0

Page
{
    id: createRijaal
    property alias name: tftk.textField
    property variant individualId
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarFollowKeyboardPolicy: ActionBarFollowKeyboardPolicy.Never
    
    function cleanUp() {
        
    }
    
    titleBar: TitleBar
    {
        kind: TitleBarKind.TextField
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
                    topMargin: 10
                    
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
                    }
                    
                    CheckBox
                    {
                        id: female
                        text: qsTr("Female") + Retranslate.onLanguageChanged
                    }
                }
            }
        }
    }
    
    attachedObjects: [
        Timer {
            interval: 250
            running: true
            repeat: false
            
            onTriggered: {
                var profile = persist.getValueFor("userProfile");
                
                if (profile)
                {
                    name.text = profile.name;
                    female.checked = profile.female == 1;
                } else {
                    name.requestFocus();
                }
            }
        }        
    ]
}