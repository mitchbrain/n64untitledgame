
#include "sys.h"


void gAppendHex4(u8 val);

u16 *gfx_buff;
u16 *g_disp_ptr;
u16 g_cur_pal;
u16 g_cons_ptr;
u8 g_last_x;
u8 g_last_y;

void gInit(u16 *gfx_mem) {

    gfx_buff = gfx_mem;
    gCleanScreen();
}

void gCleanScreen() {

    g_cur_pal = 0;
    gSetXY(G_BORDER_X, G_BORDER_Y);
    for (int i = 0; i < G_SCREEN_W * G_SCREEN_H; i++)gfx_buff[i] = PAL_B3;
    gSetPal(PAL_B1);
}

void gSetPal(u16 pal) {
    g_cur_pal = pal;
}

void gAppendString(u8 *str) {
    while (*str != 0)*g_disp_ptr++ = *str++ + g_cur_pal;
}

void gAppendChar(u8 chr) {

    *g_disp_ptr++ = chr + g_cur_pal;
}

void gAppendHex4(u8 val) {

    val += (val < 10 ? '0' : '7');
    *g_disp_ptr++ = val + g_cur_pal;
}

void gAppendHex8(u8 val) {

    gAppendHex4(val >> 4);
    gAppendHex4(val & 15);
}

void gAppendHex16(u16 val) {

    gAppendHex8(val >> 8);
    gAppendHex8(val);
}

void gAppendHex32(u32 val) {

    gAppendHex16(val >> 16);
    gAppendHex16(val);

}

void gSetXY(u8 x, u8 y) {

    g_cons_ptr = x + y * G_SCREEN_W;
    g_disp_ptr = &gfx_buff[g_cons_ptr];
    g_last_x = x;
    g_last_y = y;
}

void gConsPrint(u8 *str) {

    g_disp_ptr = &gfx_buff[g_cons_ptr];
    g_cons_ptr += G_SCREEN_W;
    g_last_y++;
    gAppendString(str);
}

