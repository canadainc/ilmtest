import QtQuick 1.0
import bb.cascades 1.0
import bb.multimedia 1.0

BasePage
{
    contentContainer: Container
    {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        bottomPadding: 20; topPadding: 20; rightPadding: 20
        layout: DockLayout {}
        
        Container {
            id: legend
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Top
            
            attachedObjects: [
                ComponentDefinition
                {
                    id: statsDefinition
                    
                    Container
                    {
                        property alias image: imageView
                        property int value
                        property string choice
                        
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        
                        ImageView {
                            id: imageView
                            preferredHeight: 20;
                            preferredWidth: 20
                            verticalAlignment: VerticalAlignment.Center
                        }
                        
                        Label {
                            text: "%1 (%2%)".arg(choice).arg(value)
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontSize: FontSize.XXSmall
                            horizontalAlignment: HorizontalAlignment.Fill
                            textStyle.textAlign: TextAlign.Left
                        }
                        
                        onCreationCompleted: {
                            fader.play();
                        }
                        
                        animations: [
                            FadeTransition {
                                id: fader
                                fromOpacity: 0
                                duration: 500
                            }
                        ]
                    }
                }
            ]
        }
        
        Container
        {
            id: barParent
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center
            visible: false
            
            Container
            {
                id: barContainer
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Fill
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                attachedObjects:
                [
                    ComponentDefinition
                    {
                        id: barDefinition
                        
                        ImageView {
                            verticalAlignment: VerticalAlignment.Bottom
                            leftMargin: 40; rightMargin: 40
                            preferredWidth: 50
                            
                            onCreationCompleted: {
                                scaler.play();
                            }
                            
                            animations: [
                                ParallelAnimation
                                {
                                    id: scaler
                                    
                                    TranslateTransition {
                                        fromY: 200
                                        toY: 0
                                        duration: 500
                                    }
                                    
                                    ScaleTransition {
                                        fromY: 0
                                        toY: 1
                                        duration: 500
                                    }   
                                }
                            ]
                        }
                    }
                ]
            }
            
            Divider {
                id: divider
                topMargin: 0; bottomMargin: 0
                verticalAlignment: VerticalAlignment.Bottom
            }
            
            ImageView {
                imageSource: "images/bottomDropShadow.png"
                topMargin: 0
                leftMargin: 0
                rightMargin: 0
                bottomMargin: 0
                
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
            }
        }
        
        ActivityIndicator {
            id: activity
            running: true
            visible: running
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            preferredHeight: 200
        }
    }
    
    function onUsed(stats) {
        timer.stats = stats;
    }
    
    onCreationCompleted: {
        loop.play();
        game.lifelineAskAudienceUsed.connect(onUsed);
        game.lifelineAskAudience();
        timer.start();
    }
    
    attachedObjects: [
        Timer {
            id: timer
            repeat: false
            running: false
            interval: 3000
            property variant stats
            
            onTriggered:
            {
                loop.sourceUrl = "asset:///audio/sfx01.mp3"
                loop.repeatMode = RepeatMode.None
                loop.play();
                activity.running = false;
                barParent.visible = true;
                
                for (var i = 0; i < 4; i++)
                {
                    var bar = barDefinition.createObject();
                    bar.imageSource = "images/bars/%1.png".arg(i);
                    bar.preferredHeight = stats[i].stat*2;
                    
                    var stat = statsDefinition.createObject();
                    stat.image.imageSource = bar.imageSource;
                    stat.choice = stats[i].value;
                    stat.value = stats[i].stat;
                    legend.add(stat);
                    
                    barContainer.add(bar);
                }
            }
        },
        
        MediaPlayer {
            id: loop
            sourceUrl: "asset:///audio/loop01.mp3"
            repeatMode: RepeatMode.Track
        }
    ]
}