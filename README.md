# Dev-Driver_LED-Driver-App
## Description
    Simple led driver build-in on BeagleBone Black, using an application to control on, off, blink led and read led's state
## Connection   
    Led -> GPIO27 (GPIO0_27) or P8_17
## Using 
    Step 1: make file .ko, using cross-compile from x86 host
`make`  
    
    Step 2: make app.exe
`make app`

    Step 3: copy from host to board
`scp app x.ko debian@192.168.x.x:/home/debian`

    Step 4: load driver to kernel to use
`insmod x.ko`   
`./app`

## Demo video
[Link demo](https://drive.google.com/file/d/1bBa7PgEBdsPr1ZUbgaRVO9RaOMSRz0UP/view?usp=sharing)