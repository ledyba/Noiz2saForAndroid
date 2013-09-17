/*
 * $Id: screen.c,v 1.3 2003/02/09 07:34:16 kenta Exp $
 *
 * Copyright 2002 Kenta Cho. All rights reserved.
 */

/**
 * SDL screen handler.
 *
 * @version $Revision: 1.3 $
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "noiz2sa.h"
#include "screen.h"
#include "clrtbl.h"
#include "vector.h"
#include "degutil.h"
#include "letterrender.h"
#include "attractmanager.h"

#include "java.h"
#include "log.h"

int windowMode = 0;
int brightness = DEFAULT_BRIGHTNESS;

static SDL_Window* window;
static SDL_Surface* windowSurface;
static SDL_Surface *video, *layer, *lpanel, *rpanel;
static LayerBit **smokeBuf;
static LayerBit *pbuf;
LayerBit *l1buf, *l2buf;
LayerBit *buf;
LayerBit *lpbuf, *rpbuf;
static SDL_Rect screenRect, layerRect, layerClearRect;
static SDL_Rect lpanelRect, rpanelRect, panelClearRect;
static int pitch, ppitch;

// Handle BMP images.
#define SPRITE_NUM 7

static SDL_Surface *sprite[SPRITE_NUM];
static char *spriteFile[SPRITE_NUM] = {
  "title_n.amr", "title_o.amr", "title_i.amr", "title_z.amr", "title_2.amr", 
  "title_s.amr", "title_a.amr",
};

const Uint8* keys;

static void loadSprites() {
  SDL_Surface *img;
  int i;
  char name[32];
  color[0].r = 100; color[0].g = 0; color[0].b = 0;
  SDL_SetPaletteColors(video->format->palette, color, 0, 1);
  for ( i=0 ; i<SPRITE_NUM ; i++ ) {
    strcpy(name, "images/");
    strcat(name, spriteFile[i]);
    img = SDL_LoadBMP(name);
    if ( img == NULL ) {
      fprintf(stderr, "Unable to load: %s\n", name);
      SDL_Quit();
      exit(1);
    }
    sprite[i] = SDL_ConvertSurface(img, video->format, 0);
    SDL_SetColorKey(sprite[i], SDL_TRUE, 0);
    fprintf(stdout, "Sprite loaded. : %s\n", name);
  }
  color[0].r = color[0].g = color[0].b = 255;
  SDL_SetPaletteColors(video->format->palette, color, 0, 1);
  fprintf(stdout, "All sprites loaded\n");
}

void drawSprite(int n, int x, int y) {
  SDL_Rect pos;
  pos.x = x; pos.y = y;
  pos.w = sprite[n]->w;
  pos.h = sprite[n]->h;
  SDL_BlitSurface(sprite[n], NULL, video, &pos);
}

// Initialize palletes.
static void initPalette() {
  int i;
  for ( i=0 ; i<256 ; i++ ) {
    color[i].r = color[i].r*brightness/256;
    color[i].g = color[i].g*brightness/256;
    color[i].b = color[i].b*brightness/256;
  }
  SDL_SetPaletteColors(video->format->palette, color, 0, 256);
  SDL_SetPaletteColors(layer->format->palette, color, 0, 256);
  SDL_SetPaletteColors(lpanel->format->palette, color, 0, 256);
  SDL_SetPaletteColors(rpanel->format->palette, color, 0, 256);
  SDL_SetColorKey(lpanel, SDL_TRUE, SDL_MapRGB(lpanel->format, 255, 255, 255));
  SDL_SetColorKey(rpanel, SDL_TRUE, SDL_MapRGB(rpanel->format, 255, 255, 255));
}

static int lyrSize;

static void makeSmokeBuf() {
  int x, y, mx, my;
  lyrSize = sizeof(LayerBit)*pitch*LAYER_HEIGHT;
  if ( NULL == (smokeBuf = (LayerBit**)malloc(sizeof(LayerBit*)*pitch*LAYER_HEIGHT)) ) {
    fprintf(stderr, "Couldn't malloc smokeBuf.");
    exit(1);
  }
  if ( NULL == (pbuf  = (LayerBit*)malloc(lyrSize+sizeof(LayerBit))) ||
       NULL == (l1buf = (LayerBit*)malloc(lyrSize+sizeof(LayerBit))) ||
       NULL == (l2buf = (LayerBit*)malloc(lyrSize+sizeof(LayerBit))) ) {
    fprintf(stderr, "Couldn't malloc buffer.");
    exit(1);
  }
  pbuf[pitch*LAYER_HEIGHT] = 0;
  for ( y=0 ; y<LAYER_HEIGHT ; y++ ) {
    for ( x=0 ; x<LAYER_WIDTH ; x++ ) {
      mx = x + sctbl[(x*8)&(DIV-1)]/128;
      my = y + sctbl[(y*8)&(DIV-1)]/128;
      if ( mx < 0 || mx >= LAYER_WIDTH || my < 0 || my >= LAYER_HEIGHT ) {
	smokeBuf[x+y*pitch] = &(pbuf[pitch*LAYER_HEIGHT]);
      } else {	
	smokeBuf[x+y*pitch] = &(pbuf[mx+my*pitch]);
      }
    }
  }
}

SDL_Rect displayRect;

void initSDL(int window_) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	Uint8 const videoBpp = BPP;
	Uint32 const videoFlags = (window_) ? 0 : SDL_WINDOW_FULLSCREEN;

	if (SDL_GetDisplayBounds(0, &displayRect) != 0) {
		LOGE("Unable to get display rect: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	LOGD("Display: %dx%dx%dx%d\n", displayRect.x, displayRect.y, displayRect.w,
			displayRect.h);

	if ((window = SDL_CreateWindow(CAPTION, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, videoFlags))
			== NULL) {
		LOGE("Unable to create SDL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	if ((windowSurface = SDL_GetWindowSurface(window)) == NULL) {
		LOGE("Unable to create SDL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	if ((video = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, videoBpp,
			0, 0, 0, 0)) == NULL) {
		LOGE("Unable to create SDL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	screenRect.x = screenRect.y = 0;
	screenRect.w = SCREEN_WIDTH;
	screenRect.h = SCREEN_HEIGHT;
	SDL_PixelFormat* const pfrm = video->format;
	if ( NULL
			== (layer = SDL_CreateRGBSurface(0, LAYER_WIDTH, LAYER_HEIGHT,
					videoBpp, 0, 0, 0, 0))
			||
			NULL
					== (lpanel = SDL_CreateRGBSurface(0, PANEL_WIDTH,
							PANEL_HEIGHT, videoBpp, 0, 0, 0, 0))
			||
			NULL
					== (rpanel = SDL_CreateRGBSurface(0, PANEL_WIDTH,
							PANEL_HEIGHT, videoBpp, 0, 0, 0, 0))) {
		LOGE("Couldn't create surface: %s\n", SDL_GetError());
		exit(1);
	}
	layerRect.x = (SCREEN_WIDTH - LAYER_WIDTH) / 2;
	layerRect.y = (SCREEN_HEIGHT - LAYER_HEIGHT) / 2;
	layerRect.w = LAYER_WIDTH;
	layerRect.h = LAYER_HEIGHT;
	layerClearRect.x = layerClearRect.y = 0;
	layerClearRect.w = LAYER_WIDTH;
	layerClearRect.h = LAYER_HEIGHT;
	lpanelRect.x = 0;
	lpanelRect.y = 0; //(SCREEN_HEIGHT-PANEL_HEIGHT)/2;
	rpanelRect.x = SCREEN_WIDTH - PANEL_WIDTH;
	rpanelRect.y = 0; //(SCREEN_HEIGHT-PANEL_HEIGHT)/2;
	lpanelRect.w = rpanelRect.w = PANEL_WIDTH;
	lpanelRect.h = PANEL_HEIGHT;
	rpanelRect.h = PANEL_HEIGHT * 2;
	panelClearRect.x = panelClearRect.y = 0;
	panelClearRect.w = PANEL_WIDTH;
	panelClearRect.h = PANEL_HEIGHT * 2;

	pitch = layer->pitch / (videoBpp / 8);
	buf = (LayerBit*) layer->pixels;
	ppitch = lpanel->pitch / (videoBpp / 8);
	lpbuf = (LayerBit*) lpanel->pixels;
	rpbuf = (LayerBit*) rpanel->pixels;

	initPalette();
	makeSmokeBuf();
	clearLPanel();
	clearRPanel();

	loadSprites();

	// SDL_WM_SetCaption(CAPTION, NULL);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetWindowGrab(window, 1);
	//SDL_WM_GrabInput(SDL_GRAB_ON);
}

void closeSDL() {
	SDL_ShowCursor(SDL_ENABLE);
}

void blendScreen() {
  int i;
  for ( i = lyrSize-1 ; i >= 0 ; i-- ) {
    buf[i] = colorAlp[l1buf[i]][l2buf[i]];
  }
}

void flipScreen() {
  SDL_BlitSurface(layer, NULL, video, &layerRect);
  if( status != TITLE ){
    SDL_BlitSurface(lpanel, NULL, video, &lpanelRect);
    SDL_BlitSurface(rpanel, NULL, video, &rpanelRect);
  }else {
    drawTitle();
  }
  SDL_BlitSurface(video, NULL, windowSurface, &screenRect);
  SDL_UpdateWindowSurface( window );
}

void clearScreen() {
  SDL_FillRect(layer, &layerClearRect, 0);
}

void clearLPanel() {
  SDL_FillRect(lpanel, NULL, 0);
}

void clearRPanel() {
  SDL_FillRect(rpanel, NULL, 0);
}

void smokeScreen() {
  int i;
  memcpy(pbuf, l2buf, lyrSize);
  for ( i = lyrSize-1 ; i >= 0 ; i-- ) {
    l1buf[i] = colorDfs[l1buf[i]];
    l2buf[i] = colorDfs[*(smokeBuf[i])];
  }
}


void drawLine(int x1, int y1, int x2, int y2, LayerBit color, int width, LayerBit *buf) {
  int lx, ly, ax, ay, x, y, ptr, i, j;
  int xMax, yMax;

  lx = absN(x2 - x1);
  ly = absN(y2 - y1);
  if ( lx < ly ) {
    x1 -= width>>1; x2 -= width>>1;
  } else {
    y1 -= width>>1; y2 -= width>>1;
  }
  xMax = LAYER_WIDTH-width-1; yMax = LAYER_HEIGHT-width-1;

  if ( x1 < 0 ) {
    if ( x2 < 0 ) return;
    y1 = (y1-y2)*x2/(x2-x1)+y2;
    x1 = 0;
  } else if ( x2 < 0 ) {
    y2 = (y2-y1)*x1/(x1-x2)+y1;
    x2 = 0;
  }
  if ( x1 > xMax ) {
    if ( x2 > xMax ) return;
    y1 = (y1-y2)*(x2-xMax)/(x2-x1)+y2;
    x1 = xMax;
  } else if ( x2 > xMax ) {
    y2 = (y2-y1)*(x1-xMax)/(x1-x2)+y1;
    x2 = xMax;
  }
  if ( y1 < 0 ) {
    if ( y2 < 0 ) return;
    x1 = (x1-x2)*y2/(y2-y1)+x2;
    y1 = 0;
  } else if ( y2 < 0 ) {
    x2 = (x2-x1)*y1/(y1-y2)+x1;
    y2 = 0;
  }
  if ( y1 > yMax ) {
    if ( y2 > yMax ) return;
    x1 = (x1-x2)*(y2-yMax)/(y2-y1)+x2;
    y1 = yMax;
  } else if ( y2 > yMax ) {
    x2 = (x2-x1)*(y1-yMax)/(y1-y2)+x1;
    y2 = yMax;
  }

  lx = abs(x2 - x1);
  ly = abs(y2 - y1);

  if ( lx < ly ) {
    if ( ly == 0 ) ly++;
    ax = ((x2 - x1)<<8) / ly;
    ay = ((y2 - y1)>>8) | 1;
    x  = x1<<8;
    y  = y1;
    for ( i=ly ; i>0 ; i--, x+=ax, y+=ay ){
      ptr = y*pitch + (x>>8);
      for ( j=width ; j>0 ; j--, ptr++ ) {
	buf[ptr] = color;
      }
    }
  } else {
    if ( lx == 0 ) lx++;
    ay = ((y2 - y1)<<8) / lx;
    ax = ((x2 - x1)>>8) | 1;
    x  = x1;
    y  = y1<<8;
    for ( i=lx ; i>0 ; i--, x+=ax, y+=ay ) {
      ptr = (y>>8)*pitch + x;
      for ( j=width ; j>0 ; j--, ptr+=pitch ) {
	buf[ptr] = color;
      }
    }
  }
}

void drawThickLine(int x1, int y1, int x2, int y2, 
		   LayerBit color1, LayerBit color2, int width) {
  int lx, ly, ax, ay, x, y, ptr, i, j;
  int xMax, yMax;
  int width1, width2;

  lx = abs(x2 - x1);
  ly = abs(y2 - y1);
  if ( lx < ly ) {
    x1 -= width>>1; x2 -= width>>1;
  } else {
    y1 -= width>>1; y2 -= width>>1;
  }
  xMax = LAYER_WIDTH-width; yMax = LAYER_HEIGHT-width;

  if ( x1 < 0 ) {
    if ( x2 < 0 ) return;
    y1 = (y1-y2)*x2/(x2-x1)+y2;
    x1 = 0;
  } else if ( x2 < 0 ) {
    y2 = (y2-y1)*x1/(x1-x2)+y1;
    x2 = 0;
  }
  if ( x1 > xMax ) {
    if ( x2 > xMax ) return;
    y1 = (y1-y2)*(x2-xMax)/(x2-x1)+y2;
    x1 = xMax;
  } else if ( x2 > xMax ) {
    y2 = (y2-y1)*(x1-xMax)/(x1-x2)+y1;
    x2 = xMax;
  }
  if ( y1 < 0 ) {
    if ( y2 < 0 ) return;
    x1 = (x1-x2)*y2/(y2-y1)+x2;
    y1 = 0;
  } else if ( y2 < 0 ) {
    x2 = (x2-x1)*y1/(y1-y2)+x1;
    y2 = 0;
  }
  if ( y1 > yMax ) {
    if ( y2 > yMax ) return;
    x1 = (x1-x2)*(y2-yMax)/(y2-y1)+x2;
    y1 = yMax;
  } else if ( y2 > yMax ) {
    x2 = (x2-x1)*(y1-yMax)/(y1-y2)+x1;
    y2 = yMax;
  }

  lx = abs(x2 - x1);
  ly = abs(y2 - y1);
  width1 = width - 2;

  if ( lx < ly ) {
    if ( ly == 0 ) ly++;
    ax = ((x2 - x1)<<8) / ly;
    ay = ((y2 - y1)>>8) | 1;
    x  = x1<<8;
    y  = y1;
    ptr = y*pitch + (x>>8) + 1;
    memset(&(buf[ptr]), color2, width1);
    x += ax; y += ay;
    for ( i = ly-1 ; i > 1 ; i--, x+=ax, y+=ay ){
      ptr = y*pitch + (x>>8);
      buf[ptr] = color2; ptr++;
      memset(&(buf[ptr]), color1, width1); ptr += width1;
      buf[ptr] = color2;
    }
    ptr = y*pitch + (x>>8) + 1;
    memset(&(buf[ptr]), color2, width1);
  } else {
    if ( lx == 0 ) lx++;
    ay = ((y2 - y1)<<8) / lx;
    ax = ((x2 - x1)>>8) | 1;
    x  = x1;
    y  = y1<<8;
    ptr = ((y>>8)+1)*pitch + x;
    for ( j=width1 ; j>0 ; j--, ptr+=pitch ) {
      buf[ptr] = color2;
    }
    x += ax; y += ay;
    for ( i=lx-1 ; i>1 ; i--, x+=ax, y+=ay ) {
      ptr = (y>>8)*pitch + x;
      buf[ptr] = color2; ptr += pitch;
      for ( j=width1 ; j>0 ; j--, ptr+=pitch ) {
	buf[ptr] = color1;
      }
      buf[ptr] = color2;
    }
    ptr = ((y>>8)+1)*pitch + x;
    for ( j=width1 ; j>0 ; j--, ptr+=pitch ) {
      buf[ptr] = color2;
    }
  }
}

void drawBox(int x, int y, int width, int height, 
	     LayerBit color1, LayerBit color2, LayerBit *buf) {
  int i, j;
  LayerBit cl;
  int ptr;

  x -= width>>1; y -= height>>1;
  if ( x < 0 ) {
    width += x; x = 0;
  }
  if ( x+width >= LAYER_WIDTH ) {
    width = LAYER_WIDTH-x;
  }
  if ( width <= 1 ) return;
  if ( y < 0 ) {
    height += y; y = 0;
  }
  if ( y+height > LAYER_HEIGHT ) {
    height = LAYER_HEIGHT-y;
  }
  if ( height <= 1 ) return;

  ptr = x + y*LAYER_WIDTH;
  memset(&(buf[ptr]), color2, width);
  y++;
  for ( i=0 ; i<height-2 ; i++, y++ ) {
    ptr = x + y*LAYER_WIDTH;
    buf[ptr] = color2; ptr++;
    memset(&(buf[ptr]), color1, width-2);
    ptr += width-2;
    buf[ptr] = color2;
  }
  ptr = x + y*LAYER_WIDTH;
  memset(&(buf[ptr]), color2, width);
}

void drawBoxPanel(int x, int y, int width, int height, 
		  LayerBit color1, LayerBit color2, LayerBit *buf) {
  int i, j;
  LayerBit cl;
  int ptr;

  x -= width>>1; y -= height>>1;
  if ( x < 0 ) {
    width += x; x = 0;
  }
  if ( x+width >= PANEL_WIDTH ) {
    width = PANEL_WIDTH-x;
  }
  if ( width <= 1 ) return;
  if ( y < 0 ) {
    height += y; y = 0;
  }
  if ( y+height > PANEL_HEIGHT ) {
    height = PANEL_HEIGHT-y;
  }
  if ( height <= 1 ) return;

  ptr = x + y*PANEL_WIDTH;
  memset(&(buf[ptr]), color2, width);
  y++;
  for ( i=0 ; i<height-2 ; i++, y++ ) {
    ptr = x + y*PANEL_WIDTH;
    buf[ptr] = color2; ptr++;
    memset(&(buf[ptr]), color1, width-2);
    ptr += width-2;
    buf[ptr] = color2;
  }
  ptr = x + y*PANEL_WIDTH;
  memset(&(buf[ptr]), color2, width);
}

// Draw the numbers.
int drawNum(int n, int x ,int y, int s, int c1, int c2) {
  int n2 = n;
  int factor = 1;
  while(n2 >= 10){
	 n2 /= 10;
	 factor *= 10;
  }

  for ( ; ; ) {
    drawLetter(n/factor, x, y, s, 2, c1, c2, lpbuf);
    x += s*1.7f;
    n %= factor;
    factor /= 10;
    if ( factor < 1 ) break;
  }
  return y;
}

int drawNumRight(int n, int x ,int y, int s, int c1, int c2) {
  int d, nd, drawn = 0;
  for ( d = 100000000 ; d > 0 ; d /= 10 ) {
    nd = (int)(n/d);
    if ( nd > 0 || drawn ) {
      n -= d*nd;
      drawLetter(nd%10, x, y, s, 2, c1, c2, rpbuf);
      x += s*1.7f;
      drawn = 1;
    }
  }
  if ( !drawn ) {
    drawLetter(0, x, y, s, 2, c1, c2, rpbuf);
    x += s*1.7f;
  }
  return x;
}

int drawNumCenter(int n, int x ,int y, int s, int c1, int c2) {
  for ( ; ; ) {
    drawLetterBuf(n%10, x, y, s, 2, c1, c2, buf, 0);
    x -= s*1.7f;
    n /= 10;
    if ( n <= 0 ) break;
  }
  return y;
}


#define JOYSTICK_AXIS 16384

int getPadState() {
  const float x = JOYPAD_getX();
  const float y = JOYPAD_getY();
  int pad = 0;
  if ( x > JOYSTICK_AXIS ) {
    pad |= PAD_RIGHT;
  }
  if ( x < -JOYSTICK_AXIS ) {
    pad |= PAD_LEFT;
  }
  if ( y > JOYSTICK_AXIS ) {
    pad |= PAD_DOWN;
  }
  if ( y < -JOYSTICK_AXIS ) {
    pad |= PAD_UP;
  }
  return pad;
}

int buttonReversed = 0;

int getButtonState() {
	int btn = 0;
	const int btn1 = JOYPAD_getA();
	const int btn2 = JOYPAD_getB();
	if (JOYPAD_getA()) {
		btn |= (buttonReversed) ? PAD_BUTTON2 : PAD_BUTTON1;
	}
	if (JOYPAD_getB()) {
		btn |= (buttonReversed) ? PAD_BUTTON1 : PAD_BUTTON2;
	}
	return btn;
}
