import bb.cascades 1.2

QtObject
{
    function randomInt(min,max) {
        return Math.floor(Math.random()*(max-min+1)+min);
    }
}