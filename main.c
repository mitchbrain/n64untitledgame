
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <libdragon.h>

//#include <ControllerInput.h>

#define SENSITIVITY 2

#define DEBUG


unsigned short gButtons = 0;
struct controller_data gKeys;

volatile int gTicks;                    /* incremented every vblank */

typedef struct SpriteTransform SpriteTransform;

struct SpriteTransform 
{
    // XY position
    float X;
    float Y;

    // Rotation position
    float rX;
    float rY;

    // Scale
    float sX;
    float sY;

    sprite_t * obj;
    display_context_t* _dc;
};

void translateSprite(SpriteTransform* object, SpriteTransform amount)
{
    // Move object to new location
    object->X = ((object->X * cos(amount.rX)) - (object->Y * sin(amount.rX))) + amount.X; 
    object->Y = ((object->X * sin(amount.rY)) + (object->Y * cos(amount.rY))) - amount.Y;

    // Rotate object
    //object->X = (object->X * cos(amount.rX)) - (object->Y * sin(amount.rX));
    //object->Y = (object->X * sin(amount.rY)) + (object->Y * cos(amount.rY));

    // Render at new cordinates
    graphics_draw_sprite_trans(*object->_dc, object->X, object->Y, object->obj );
    //rdp_draw_sprite_scaled();
    rsp_init();
}

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
	//int box_x = 20, box_y = 20; 

#ifdef DEBUG
	char temp[128];
#endif
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    dfs_init( DFS_DEFAULT_LOCATION );
    rdp_init();
    controller_init();
    timer_init();

    SpriteTransform movment = {0.0, 0.0,
                               0.0, 0.0,
                               0.0, 0.0 };

    /* Read in single sprite */

    int fp = dfs_open("/face.sprite");
    sprite_t *face = malloc( dfs_size( fp ) );
    dfs_read( face, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    SpriteTransform faceTrasform = {0.0, 0.0,
                                    0.0, 0.0,
                                    0.0, 0.0,
                                    face };
    faceTrasform._dc = &_dc;

    float y_input[4] = {0,0,0,0};
    float x_input[4] = {0,0,0,0};	

    //const float gravity_const = 1;

    while(1) 
    {
        _dc = lockVideo(1);
		graphics_fill_screen(_dc, graphics_make_color(0,0,0,255));

		getButtons(0);

		movment.Y = getAnalogY(0);
		movment.X = getAnalogX(0);

        if (gKeys.c[0].A)
        {
            movment.rX = 1;
        }

        if (gKeys.c[0].B)
        {
            movment.rY = 1;
        }

        translateSprite(&faceTrasform, movment);

		y_input[1] = getAnalogY(1);
		x_input[1] = getAnalogX(1);	

		y_input[2] = getAnalogY(2);
		x_input[2] = getAnalogX(2);	

		y_input[3] = getAnalogY(3);
		x_input[3] = getAnalogX(3);	

        graphics_draw_line(_dc,0, (240/2), 340, (240/2), graphics_make_color(255, 0, 0, 255));

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

      //  for (int i = 0; i !=1; i++)
      //  {
      //      if (y_input[i] != 0)
      //      {
      //          box_y += -y_input[i] * SENSITIVITY;
      //      }

      //      if (x_input != 0)
      //      {
      //          box_x += x_input[i] * SENSITIVITY;
      //      }
      //  }
        
        // If we have not hit the ground then apply velocity
      //  if (box_y <= (120 - (face->height)))
      //  {
      //      box_y += gravity_const;
      //  }


        unlockVideo(_dc);

        //graphics_draw_sprite_trans( _dc, box_x, box_y, face );

        /* Force backbuffer flip */
        display_show(_dc);
    }
}
