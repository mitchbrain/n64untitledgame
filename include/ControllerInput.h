

#ifndef __CONTROLLER_INPUT_H_INCLUDE
#define __CONTROLLER_INPUT_H_INCLUDE

#include <libdragon.h>


unsigned short getButtons(int pad);

float getAnalogX(int pad);

float getAnalogY(int pad);

void ControllerInput();

void controllerTick();


#endif

