
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

//#include <ControllerInput.h>

#define SENSITIVITY 2

#define DEBUG


unsigned short gButtons = 0;
struct controller_data gKeys;

volatile int gTicks;                    /* incremented every vblank */

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
    return gKeys.c[pad].x / 114;
}

float getAnalogY(int pad)
{
    return gKeys.c[pad].y / 114;
}

display_context_t lockVideo(int wait)
{
    display_context_t dc;

    if (wait)
        while (!(dc = display_lock()));
    else
        dc = display_lock();
    return dc;
}

void unlockVideo(display_context_t dc)
{
    if (dc)
        display_show(dc);
}

/* text functions */
void drawText(display_context_t dc, char *msg, int x, int y)
{
    if (dc)
        graphics_draw_text(dc, x, y, msg);
}

void printText(display_context_t dc, char *msg, int x, int y)
{
    if (dc)
        graphics_draw_text(dc, x*8, y*8, msg);
}

/* vblank callback */
void vblCallback(void)
{
    gTicks++;
}

void delay(int cnt)
{
    int then = gTicks + cnt;
    while (then > gTicks) ;
}

/* initialize console hardware */
void init_n64(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );

    register_VI_handler(vblCallback);

    controller_init();
}

void printDebug(display_context_t dc, char* msg, int x, int y)
{
	if (sizeof(msg) > 128)
		return;

	char temp[128];
	sprintf(temp, "Debug msg: %s", msg);
	printText(dc, temp, x, y);
}

/* main code entry point */
int main(void)
{
    display_context_t _dc;
	int box_x = 20, box_y = 20; 

#ifdef DEBUG
	char temp[128];
#endif

    init_n64();

    while (1)
    {
        _dc = lockVideo(1);
		graphics_fill_screen(_dc, graphics_make_color(0,0,0,255));

		getButtons(0);

		float y_input = getAnalogY(0);
		float x_input = getAnalogX(0);	

#ifdef DEBUG
		sprintf(temp, "Y input: %f", y_input);
		printDebug(_dc, temp, 0, 0);
		memset(temp, 0, sizeof(temp));

		sprintf(temp, "X input: %f", x_input);
		printDebug(_dc, temp, 0, 1);
		memset(temp, 0, sizeof(temp));
#endif

		if (y_input != 0)
		{
			box_y += -y_input * SENSITIVITY;
		}

		if (x_input != 0)
		{
			box_x += x_input * SENSITIVITY;
		}

		graphics_draw_box(_dc, box_x, box_y, 20, 20, graphics_make_color(255,0,0,255));
        unlockVideo(_dc);

    }

    return 0;
}
