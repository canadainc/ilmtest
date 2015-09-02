import QtQuick 1.0
import bb.cascades 1.0

ProgressIndicator
{
    id: clock
    property int currentValue
    signal ready();
    signal expired();
    fromValue: 0
    toValue: 30;
    scaleX: 0
    value: currentValue
    
    function start() {
        console.log("*** START SCALERS");
        scaler.play();
    }
    
    function stop()
    {
        timer.stop();
        sound.stopClock();
        state = ProgressIndicatorState.Complete;
    }
    
    function reset()
    {
        currentValue = toValue;
        state = ProgressIndicatorState.Progress;
        scaleX = 0;
        console.log("*** RESETTED!", currentValue);
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
                console.log("**", currentValue);

                if (currentValue == -1)
                {
                    stop();
                    expired();
                } else {
                    if (currentValue < 10) {
	                    clock.state = ProgressIndicatorState.Error;
	                } else if (currentValue < 20) {
	                    clock.state = ProgressIndicatorState.Pause;
	                }
                }
            }
        }
    ]
}