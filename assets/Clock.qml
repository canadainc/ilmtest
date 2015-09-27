import QtQuick 1.0
import bb.cascades 1.0

Container
{
    id: clock
    property int currentValue
    signal ready();
    signal expired();
    horizontalAlignment: HorizontalAlignment.Fill
    scaleX: 0
    background: bg.imagePaint
    
    function elapsed() {
        return progress.toValue-currentValue;
    }
    
    function start()
    {
        if ( !scaler.isPlaying() && !scaler.isStarted() ) {
            scaler.play();
        }
    }
    
    function stop()
    {
        scaler.stop();
        timer.stop();
        sound.stopClock();
        progress.state = ProgressIndicatorState.Complete;
    }
    
    function reset()
    {
        currentValue = progress.toValue;
        progress.state = ProgressIndicatorState.Progress;
        scaleX = 0;
    }
    
    ProgressControl
    {
        id: busy
        asset: "images/progress/spinner_clock.png"
        delegateActive: timer.running
        scaleY: scaleX
        loadingText: currentValue.toString()
    }
    
    ProgressIndicator
    {
        id: progress
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Bottom
        fromValue: 0
        toValue: 30;
        value: currentValue
    }
    
    animations: [
        ScaleTransition
        {
            id: scaler
            fromX: 0
            toX: 1
            duration: 1000
            
            onEnded: {
                sound.playClock();
                
                ready();
                timer.start();
                
                progress.translationY = 1;
            }
        }
    ]
    
    attachedObjects: [
        Timer {
            id: timer
            repeat: true
            running: false
            interval: 1000
            
            onTriggered: {
                currentValue -= 1;
                busy.scaleX = currentValue % 2 == 0 ? busy.scaleX*0.9 : 1;
                
                if (currentValue == -1)
                {
                    stop();
                    expired();
                } else {
                    if (currentValue < 10) {
                        progress.state = ProgressIndicatorState.Error;
                    } else if (currentValue < 20) {
                        progress.state = ProgressIndicatorState.Pause;
                    }
                }
            }
        },
        
        ImagePaintDefinition {
            id: bg
            //repeatPattern: RepeatPattern.X
            imageSource: "images/backgrounds/exam_bg.jpg"
        }
    ]
}