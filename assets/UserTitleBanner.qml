import bb.cascades 1.2

Container
{
    signal bannerTapped();
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: bgStrip.imagePaint
    
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    
    ImageView
    {
        id: avatarImageView
        preferredHeight: 95
        preferredWidth: 95
        imageSource: user.female ? "images/title/ic_female.png" : "images/title/ic_male.png"
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Center
        scalingMethod: ScalingMethod.AspectFill
    }
    
    Container
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
                text: qsTr("Points: %n", "", user.points) + Retranslate.onLanguageChanged
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
                bannerTapped();
            }
        }
    ]
    
    attachedObjects: [
        ImagePaintDefinition {
            id: bgStrip
            imageSource: "images/title/title_bar.jpg"
        }
    ]
}