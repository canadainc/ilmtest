import bb.cascades 1.3

Container
{
    id: refContainer
    property string plainText
    property string htmlText
    horizontalAlignment: HorizontalAlignment.Right
    verticalAlignment: VerticalAlignment.Bottom
    
    function show()
    {
        labelField.opacity = 0;
        ft.play();
    }
    
    function apply(refData)
    {
        labelField.opacity = 0;

        var titleValue = refData.pageTitle ? "%1: %2".arg(refData.title).arg(refData.pageTitle) : refData.title;
        plainText = "“%1” - %2".arg(titleValue).arg(refData.author);
        htmlText = "<html><i><a href=\"%1\">“%2”</a></i> - %3</html>".arg( refData.id.toString() ).arg( titleValue.replace(/&/g,"&amp;") ).arg( refData.author.replace(/&/g,"&amp;") );
        ath.uri = refData.uri;
        ath.target = refData.target;
    }
    
    Container
    {
        background: ipd.imagePaint
        maxWidth: 400
        maxHeight: 300
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Bottom
        leftPadding: 20; rightPadding: 20; topPadding: 20; bottomPadding: 50
        
        Label {
            id: labelField
            text: refContainer.enabled ? htmlText : plainText
            textStyle.color: Color.Black
            textStyle.textAlign: TextAlign.Center
            textStyle.fontSize: FontSize.XXSmall
            horizontalAlignment: HorizontalAlignment.Fill
            multiline: true
            opacity: 0
            
            activeTextHandler: ActiveTextHandler
            {
                id: ath
                property string uri
                property string target
                
                onTriggered: {
                    var link = event.href.toString();
                    
                    if ( link.match("\\d+") ) {
                        persist.invoke( target, "", "", uri+link, "", global );
                    }
                    
                    event.abort();
                }
            }
            
            animations: [
                FadeTransition {
                    id: ft
                    fromOpacity: 0
                    toOpacity: 1
                    duration: 2500
                    easingCurve: StockCurve.ExponentialInOut
                }
            ]
        }
        
        attachedObjects: [
            ImagePaintDefinition {
                id: ipd
                imageSource: "images/toast/permission_toast_bg.amd"
            }
        ]
    }
}