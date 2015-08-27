import bb.cascades 1.0
import QtQuick 1.0
import bb.multimedia 1.0

ProgressIndicator
{
    id: clock
    property int currentValue
    signal expired();
    signal ready();
    fromValue: 0
    toValue: 30;
    
    onCreationCompleted: {
        tick.prepare();
    }
    
    function start() {
        timer.start();
    }
    
    function stop() {
        timer.stop();
        tick.stop();
    }
    
    function reset() {
        currentValue = toValue;
        value = currentValue;
        state = ProgressIndicatorState.Progress;
    }

    attachedObjects: [
        Timer {
            id: timer
            repeat: true
            running: false
            interval: 1000

			onTriggered: {
                currentValue -= 1;

                if (currentValue == -1) {
                    stop();
                    expired();
                } else {
                    if (tick.mediaState != MediaState.Started) {
                        tick.play();
                    }

                    clock.value = currentValue;

                    if (currentValue < 10) {
	                    clock.state = ProgressIndicatorState.Error;
	                } else if (currentValue < 20) {
	                    clock.state = ProgressIndicatorState.Pause;
	                }
                }
            }
        },
        
        MediaPlayer {
            id: tick
            sourceUrl: "asset:///audio/clock.mp3"
            repeatMode: RepeatMode.Track
            
            onDurationChanged: {
                ready();
            }
        }
    ]
}