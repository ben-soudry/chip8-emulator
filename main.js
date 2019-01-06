var Module = {
    onRuntimeInitialized: main
};


function main(){
    var chip8 = new Module.Chip8("roms/games/Connect\ 4\ \[David\ Winter\].ch8");
    //var chip8 = new Module.Chip8("roms/games/Missile\ \[David\ Winter\].ch8");
    console.log('isSoundOn result: ' + chip8.isSoundOn());
    
    var canvas = document.getElementById('Chip8 Display');
    
    var clockSpeed = 100; // Clock Speed in Hz, rate to call emulateCycle
    var timeStep = (1/clockSpeed)*1000;
    var lastTimestamp = 0;

    //Start key listeners:
    var keyDownListener = keyListener(true);
    var keyUpListener = keyListener(false);

    document.addEventListener("keydown", keyDownListener, true);
    document.addEventListener("keyup", keyUpListener, true);
    //document.addEventListener("keydown", function(event){console.warn("***KeyDown!!")});    

    //Start main loop:
    requestAnimationFrame(mainLoop);
    
    function mainLoop(timestamp) {
        var delta = timestamp - lastTimestamp;
        //console.log("Loop! Delta: " + delta);
        lastTimestamp = timestamp;
        while(delta >= timeStep){
            //console.log("Update!?!?");
            chip8.emulateCycle();
            delta -= timeStep;
        }
        drawScreen(chip8, canvas);  
        requestAnimationFrame(mainLoop);
    }
            
    function keyListener(isKeyDown) {
        //Returns the correct key listener for keyup, keydown events,
        //depending on what is specified by isKeyDown,
        return function(event){
            console.log("Listener Ran!");
            if (event.defaultPrevented) {
                return; // Do nothing if the event was already processed
            } 
            switch(event.code) {
                case "Digit1":
                    console.warn("****One is pressed!");
                    chip8.setKey(0x1, isKeyDown);
                    break; 
                case "Digit2":
                    chip8.setKey(0x2, isKeyDown);
                    break;
                case "Digit3":
                    chip8.setKey(0x3, isKeyDown);
                    break;
                case "Digit4":
                    chip8.setKey(0xC, isKeyDown);
                    break;
                case "KeyQ":
                    chip8.setKey(0x4, isKeyDown);
                    break;
                case "KeyW":
                    chip8.setKey(0x5, isKeyDown);
                    break;
                case "KeyE":
                    chip8.setKey(0x6, isKeyDown);
                    break;
                case "KeyR":
                    chip8.setKey(0xD, isKeyDown);
                    break;
                case "KeyA":
                    chip8.setKey(0x7, isKeyDown);
                    break;
                case "KeyS":
                    chip8.setKey(0x8, isKeyDown);
                    break;
                case "KeyD":
                    chip8.setKey(0x9, isKeyDown);
                    break;
                case "KeyF":
                    chip8.setKey(0xE, isKeyDown);
                    break;
                case "KeyZ":
                    chip8.setKey(0xA, isKeyDown);
                    break;
                case "KeyX":
                    chip8.setKey(0x0, isKeyDown);
                    break;
                case "KeyC":
                    chip8.setKey(0xB, isKeyDown);
                    break;
                case "KeyV":
                    chip8.setKey(0xF, isKeyDown);
                    break;
                default:
                    return
                event.preventDefault();
            }
        }
    }
}


function drawScreen(chip8, canvas){
    if(canvas.getContext) {
        var ctx = canvas.getContext('2d');
        //Chip8 display is 128x64 pixels:
        const displayWidth = 64;
        const displayHeight = 32;

        pixelWidth = canvas.width / displayWidth;
        pixelHeight = canvas.height / displayHeight;

        for(col = 0; col < displayWidth; col++){
            for(row = 0; row < displayHeight; row++){
                if(chip8.displayOnAt(row, col)){
                    ctx.fillStyle = 'rgb(0, 200, 0)';
                } else {
                    ctx.fillStyle = 'rgb(0, 0, 0)';
                }
                ctx.fillRect(col*pixelWidth, row*pixelHeight, pixelWidth, pixelHeight);
            }
        }
    }
}



