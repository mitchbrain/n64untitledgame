#include "ControllerInput.h"


unsigned short gButtons = 0;
struct controller_data gKeys;

/* input - do getButtons() first, then getAnalogX() and/or getAnalogY() */
unsigned short getButtons(int pad)
{
    // Read current controller status
    controller_scan();
    gKeys = get_keys_pressed();
    return (unsigned short)(gKeys.c[0].data >> 16);
}

float getAnalogX(int pad)
{

    return (float)gKeys.c[pad].x/114;
}

float getAnalogY(int pad)
{
    return (float)gKeys.c[pad].y/114;
}

void ControllerInput()
{
    return;
}

void controllerTick(){

}