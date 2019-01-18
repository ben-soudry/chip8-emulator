var Module = {
    onRuntimeInitialized: main
};

var emulationSpeed = 450; // Emulation Speed in Hz, rate to call emulateCycle
var clockSpeed = 60; // Clock Speed in Hz, rate to call tickClock


function main(){
    //var chip8 = new Module.Chip8("roms/games/Connect\ 4\ \[David\ Winter\].ch8");
    //var chip8 = new Module.Chip8("roms/games/Missile\ \[David\ Winter\].ch8");
    //var chip8 = new Module.Chip8("roms/games/Rush\ Hour\ \[Hap\,\ 2006\].ch8");
    //var chip8 = new Module.Chip8("roms/games/Space Invaders [David Winter].ch8");
    //var chip8 = new Module.Chip8("roms/games/Tetris\ \[Fran\ Dachille\,\ 1991\].ch8");
    var chip8 = new Module.Chip8("roms/games/Breakout (Brix hack) [David Winter, 1997].ch8");
    //chip8.printRom();
    //chip8.loadROM("roms/games/Connect\ 4\ \[David\ Winter\].ch8");


    var canvas = document.getElementById('Chip8 Display');
    var slider = document.getElementById('slider');
    var speedOutput = document.getElementById('output');     
    speedOutput.innerHTML = 450;
    slider.oninput = function() {
        speedOutput.innerHTML = 10*this.value;
    }
 
    var timeStep = (1/emulationSpeed)*1000;
    var clockTimeStep = (1/clockSpeed)*1000;

    var lastTimestamp = 0;

    //Start key listeners:
    var keyDownListener = keyListener(true);
    var keyUpListener = keyListener(false);

    document.addEventListener("keydown", keyDownListener, true);
    var timeStep = (1/emulationSpeed)*1000;
    document.addEventListener("keyup", keyUpListener, true);

    //Start main loop:
    requestAnimationFrame(mainLoop);
    //Start clock
    var clockCallback = setInterval(clockUpdate, clockTimeStep); 

    //Start Dropdown Listener:
    var romsDropdown = document.getElementById('romsDropdown');
    var romFileName = romsDropdown.value;
    var loadNewRom = false;
    romsDropdown.addEventListener('change', dropdownListener); 
    
    //Get audio:
    var audio = document.getElementById("audio");
 
    function mainLoop(timestamp) {
        var delta = timestamp - lastTimestamp;
        //console.log("Loop! Delta: " + delta);
        lastTimestamp = timestamp;
        var emulateTimeRemaining = delta;
        while(emulateTimeRemaining >= timeStep){
            chip8.emulateCycle();
            emulateTimeRemaining -= timeStep;
        }
        drawScreen(chip8, canvas);  
        //Update timestep
        emulationSpeed = speedOutput.innerHTML;
        timeStep = (1/emulationSpeed)*1000;
        
        //Play audio 
        if(chip8.isSoundOn()){
            audio.play();
        } else {
            audio.pause();
        }     

        //Load a new rom if dropdown changed
        if(loadNewRom){
            console.warn("Loading new ROM!");
            chip8.loadROM(romFileName);
            drawScreen(chip8, canvas);  
            loadNewRom = false;
        }
        //Loop again 
        requestAnimationFrame(mainLoop);
    }
    
    function clockUpdate(){
        console.warn("Clock Update");
        chip8.tickClock();
    }
    
    function dropdownListener() {
        //Load new ROM
        console.warn("dropdown listener!");
        romFileName = this.value;
        loadNewRom = true;
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



