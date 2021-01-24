
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
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    dfs_init( DFS_DEFAULT_LOCATION );
    rdp_init();
    controller_init();
    timer_init();

    /* Read in single sprite */

    int fp = dfs_open("/face.sprite");
    sprite_t *face = malloc( dfs_size( fp ) );
    dfs_read( face, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    #ifdef DEBUG
        char temp[128];
    #endif
    
    int box_x = 20, box_y = 20;

    while(1) 
    {


        static display_context_t _dc = 0;

        /* Grab a render buffer */
        while( !(_dc = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( _dc, 0xFFFFFFFF );

        /* Set the text output color */
        graphics_set_color( 0x0, 0xFFFFFFFF );

        controller_scan();        
        getButtons(0);

		float y_input[4] = {getAnalogY(0), getAnalogY(1), getAnalogY(2), getAnalogY(3)};
		float x_input[4] = {getAnalogX(0), getAnalogX(1), getAnalogX(2), getAnalogX(3)};	


#ifdef DEBUG
		sprintf(temp, "Y0: %f, X0: %f", y_input[0], x_input[0]);
		printDebug(_dc, temp, 0, 0);
		memset(temp, 0, sizeof(temp));

		sprintf(temp, "Y1: %f, X1: %f", y_input[1], x_input[1]);
		printDebug(_dc, temp, 0, 1);
		memset(temp, 0, sizeof(temp));		
        
		sprintf(temp, "Y2: %f, X2: %f", y_input[2], x_input[2]);
		printDebug(_dc, temp, 0, 2);
		memset(temp, 0, sizeof(temp));

		sprintf(temp, "Y3: %f, X3: %f", y_input[3], x_input[3]);
		printDebug(_dc, temp, 0, 3);
		memset(temp, 0, sizeof(temp));
#endif

		if (y_input != 0)
		{
			box_y += -y_input[0] * SENSITIVITY;
		}

		if (x_input != 0)
		{
			box_x += x_input[0] * SENSITIVITY;
		}
        graphics_draw_sprite_trans( _dc, box_x, box_y, face );

        /* Force backbuffer flip */
        display_show(_dc);
    }
}
