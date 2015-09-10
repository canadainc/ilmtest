import bb.cascades 1.3

Page
{
    property bool isBusy: true
    
    Container
    {
        background: welcomeBack.imagePaint
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        ProgressControl
        {
            id: busy
            asset: "images/progress/spinner_clock.png"
            delegateActive: isBusy
            loadingText: qsTr("Processing...") + Retranslate.onLanguageChanged
        }
    }
    
    attachedObjects: [
        ImagePaintDefinition {
            id: welcomeBack
            imageSource: "images/backgrounds/welcome_bg.png"
            repeatPattern: RepeatPattern.XY
        }
    ]
}