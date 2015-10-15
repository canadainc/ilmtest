import bb.cascades 1.3

TextField
{
    id: numericInput
    property alias anim: ft
    property bool expose: false
    inputMode: TextFieldInputMode.NumbersAndPunctuation
    input.submitKey: SubmitKey.Submit
    
    onExposeChanged: {
        if (expose) {
            numericValidator.errorMessage = qsTr("Correct answer was: %1").arg(game.currentQuestion.answer);
            numericValidator.valid = false;
        }
    }
    
    validator: Validator
    {
        id: numericValidator
        errorMessage: qsTr("Only digits can be entered!") + Retranslate.onLanguageChanged
        mode: expose ? ValidationMode.Immediate : ValidationMode.FocusLost
        
        onValidate: {
            valid = /^\d+$/.test( numericInput.text.trim() );
        }
    }
    
    function reset()
    {
        visible = false;
        loseFocus();
        resetText();
        hintText = qsTr("Enter a numeric value");
        enabled = true;
    }
    
    onVisibleChanged: {
        opacity = 0;
        
        if (visible) {
            if ( !ft.isPlaying() && !ft.isStarted() ) {
                ft.play();
            }
        } else {
            resetText();
        }
    }
    
    animations: [
        FadeTransition {
            id: ft
            fromOpacity: 0
            toOpacity: 1
            duration: 1000
            easingCurve: StockCurve.ExponentialOut
        }
    ]
}