import bb.cascades 1.2

CustomListItem
{
    id: sli
    highlightAppearance: HighlightAppearance.Full
    opacity: 0
    
    ListItem.onInitializedChanged: {
        if (initialized)
        {
            opacity = 0;
            
            if ( !ft.isPlaying() && !ft.isStarted() ) {
                ft.play();
            }
        }
    }
    
    Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        Container
        {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            
            ImageView
            {
                id: avatarImageView
                imageSource: sli.ListItem.view.expose ? ListItemData.correct == 1 ? "images/list/choices/ca_correct.png" : "images/list/choices/ca_incorrect.png" : ListItemData.none ? "images/list/choices/none.png" : "images/list/choices/%1.png".arg(sli.ListItem.indexPath[0])
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                scalingMethod: ScalingMethod.AspectFill
            }
            
            Label
            {
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                textStyle.color: Color.White
                text: String.fromCharCode(65+sli.ListItem.indexPath[0])
                visible: !sli.ListItem.view.expose
            }
        }
        
        Container
        {
            id: nameAndDescription
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            leftPadding: 10
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            
            Label
            {
                id: titleLabel
                verticalAlignment: VerticalAlignment.Center
                content.flags: TextContentFlag.EmoticonsOff | TextContentFlag.ActiveTextOff
                text: ListItemData.value.toString()
                textFormat: TextFormat.Plain
                textStyle.base: SystemDefaults.TextStyles.TitleText
                multiline: true
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
            }
            
            Label
            {
                id: description
                verticalAlignment: VerticalAlignment.Center
                text: ListItemData.description ? ListItemData.description : ""
                textFormat: TextFormat.Plain
                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                textStyle.fontSize: FontSize.Small
                visible: text.length > 0
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
            }
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