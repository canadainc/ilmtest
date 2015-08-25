import bb.cascades 1.2

Container
{
    signal bannerTapped();
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    
    Container
    {
        layout: DockLayout {}
        preferredHeight: 95
        preferredWidth: 95
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        ImageView
        {
            id: avatarImageView
            imageSource: user.female ? "images/ic_bugs.png" : "images/ic_donate.png"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            scalingMethod: ScalingMethod.AspectFill
        }
        
        Container
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: Color.create("#aaaaaa")
            layout: DockLayout {}
            
            Label
            {
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                textStyle.color: Color.White
            }
        }
    }
    
    Container // holds name & description
    {
        id: nameAndDescription
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Center

        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        
        Container // busy icon, name
        {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            Container
            {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                
                Label {
                    verticalAlignment: VerticalAlignment.Fill
                    content.flags: TextContentFlag.EmoticonsOff | TextContentFlag.ActiveTextOff
                    leftMargin: 0
                    text: user.profileSet ? qsTr("%1 %2").arg(user.kunya).arg(user.name) : qsTr("Please tap to set profile")
                    textFormat: TextFormat.Plain
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    textStyle.fontSize: FontSize.Medium
                    
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: -1
                    }
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
            
            Label {
                verticalAlignment: VerticalAlignment.Fill
                leftMargin: 0
                text: qsTr("Points: 0")
                textFormat: TextFormat.Plain
                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                textStyle.fontSize: FontSize.Small
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
            }
        }
    }
    
    gestureHandlers: [
        TapHandler {
            onTapped: {
                console.log("UserEvent: CustomTitleBannerControlTapped");
                bannerTapped();
            }
        }
    ]
}