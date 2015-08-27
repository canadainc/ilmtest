import QtQuick 1.0
import bb.cascades 1.3
import bb.system 1.2
import com.canadainc.data 1.0

NavigationPane
{
    id: navigationPane
    property alias name: tftk.textField
    
    onPopTransitionEnded: {
        deviceUtils.cleanUpAndDestroy(page);
    }
    
    Page
    {
        id: createRijaal
        actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
        actionBarFollowKeyboardPolicy: ActionBarFollowKeyboardPolicy.Never
        
        function cleanUp()
        {
            nameValidator.validate();
            kunyaValidator.validate();
            
            if (nameValidator.valid && kunyaValidator.valid) {
                user.saveProfile( name.text.trim(), kunya.text.trim(), female.checked );
            }
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
                    name.text = user.name;
                    kunya.text = user.kunya;
                    female.checked = user.female;
                    
                    if ( !user.profileSet ) {
                        name.requestFocus();
                    }
                }
            }        
        ]
    }
}