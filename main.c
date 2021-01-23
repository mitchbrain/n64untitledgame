
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

/* hardware definitions */
// Pad buttons
#define A_BUTTON(a)     ((a) & 0x8000)
#define B_BUTTON(a)     ((a) & 0x4000)
#define Z_BUTTON(a)     ((a) & 0x2000)
#define START_BUTTON(a) ((a) & 0x1000)

// D-Pad
#define DU_BUTTON(a)    ((a) & 0x0800)
#define DD_BUTTON(a)    ((a) & 0x0400)
#define DL_BUTTON(a)    ((a) & 0x0200)
#define DR_BUTTON(a)    ((a) & 0x0100)

// Triggers
#define TL_BUTTON(a)    ((a) & 0x0020)
#define TR_BUTTON(a)    ((a) & 0x0010)

// Yellow C buttons
#define CU_BUTTON(a)    ((a) & 0x0008)
#define CD_BUTTON(a)    ((a) & 0x0004)
#define CL_BUTTON(a)    ((a) & 0x0002)
#define CR_BUTTON(a)    ((a) & 0x0001)

#define PAD_DEADZONE     5
#define PAD_ACCELERATION 10
#define PAD_CHECK_TIME   40


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

signed getAnalogX(int pad)
{
    return gKeys.c[pad].x;
}

signed getAnalogY(int pad)
{
    return gKeys.c[pad].y;
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
	char temp[128];

    init_n64();

    while (1)
    {
        _dc = lockVideo(1);
		graphics_fill_screen(_dc, graphics_make_color(0,0,0,255));

		getButtons(0);

		signed y_input = getAnalogY(0);
		signed x_input = getAnalogX(0);

		sprintf(temp, "Y input: %i", y_input);
		printDebug(_dc, temp, 0, 0);
		memset(temp, 0, sizeof(temp));

		sprintf(temp, "X input: %i", x_input);
		printDebug(_dc, temp, 0, 1);
		memset(temp, 0, sizeof(temp));

		if (y_input)
		{
			if (y_input > 0)
			{
				box_y -= 1;
			}
			else
			{
				box_y += 1;
			}
		}

		if (x_input)
		{
			if (x_input < 0)
			{
				box_x -= 1;
			}
			else
			{
				box_x += 1;
			}
		}

		graphics_draw_box(_dc, box_x, box_y, 20, 20, graphics_make_color(255,0,0,255));
        unlockVideo(_dc);

    }

    return 0;
}
