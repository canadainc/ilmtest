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
        scaler.play();
    }
    
    function stop()
    {
        timer.stop();
        sound.stopClock();
    }
    
    function reset()
    {
        currentValue = toValue;
        state = ProgressIndicatorState.Progress;
        scaleX = 0;
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
                console.log("*** SDLFKJSDL", currentValue);

                if (currentValue == -1)
                {
                    stop();
                    expired();
                } else {
                    console.log("*** SDLFKJSDL3", currentValue);
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