/*
 * $Id: noiz2sa.c,v 1.8 2003/02/12 13:55:13 kenta Exp $
 *
 * Copyright 2002 Kenta Cho. All rights reserved.
 */

/**
 * Noiz2sa main routine.
 *
 * @version $Revision: 1.8 $
 */
#include "SDL.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "noiz2sa.h"
#include "screen.h"
#include "vector.h"
#include "ship.h"
#include "shot.h"
#include "frag.h"
#include "bonus.h"
#include "foe_mtd.h"
#include "brgmng_mtd.h"
#include "background.h"
#include "degutil.h"
#include "soundmanager.h"
#include "attractmanager.h"

#include "java.h"
#include "log.h"

static int noSound = 0;

// Initialize and load preference.
static void initFirst() {
  loadPreference();
  srand(SDL_GetTicks());
  initBarragemanager();
  initAttractManager();
}

// Quit and save preference.
void quitLast() {
  if ( !noSound ) closeSound();
  savePreference();
  closeBarragemanager();
  closeSDL();
  SDL_Quit();
}

enum GameMode status;

static float stagePrm[STAGE_NUM+ENDLESS_STAGE_NUM+2][3] = {
  {13, 0.5f, 0.12f}, {2, 1.8f, 0.15f}, {3, 3.2f, 0.1f}, {90, 6.0f, 0.3f}, {5, 5.0f, 0.6f},
  {6, 10.0f, 0.6f}, {7, 5.0f, 2.2f}, {98, 12.0f, 1.5f}, {9, 10.0f, 2.0f}, {79, 21.0f, 1.5f},
  {-3, 5.0f, 0.7f}, {-1, 10.0f, 1.2f}, {-4, 15.0f, 1.8f}, {-2, 16.0f, 1.8f},
  {0, -1.0f, 0.0f},{0, -1.0f, 0.0f}
};

void initTitleStage(int stg) {
  initFoes();
  initBarrages(stagePrm[stg][0], stagePrm[stg][1], stagePrm[stg][2]);
}

void initTitle() {
  int stg;
  status = TITLE;

  stg = initTitleAtr();
  initShip();
  initShots();
  initFrags();
  initBonuses();
  initBackground();
  setStageBackground(1);

  initTitleStage(stg);
}

void initGame(int stg) {
  status = IN_GAME;

  initShip();
  initShots();
  initFoes();
  initFrags();
  initBonuses();
  initBackground();

  initBarrages(stagePrm[stg][0], stagePrm[stg][1], stagePrm[stg][2]);
  initGameState(stg);
  if ( stg < STAGE_NUM ) {
    setStageBackground(stg%5+1);
    playMusic(stg%5+1);
  } else {
    if ( !insane ) {
      setStageBackground(0);
      playMusic(0);
    } else {
      setStageBackground(6);
      playMusic(6);
    }
  }
}

void initGameover() {
  status = GAMEOVER;
  initGameoverAtr();
}

void initStageClear() {
  status = STAGE_CLEAR;
  initStageClearAtr();
}

static void move() {
  switch ( status ) {
  case TITLE:
    moveTitleMenu();
    moveBackground();
    addBullets();
    moveFoes();
    break;
  case IN_GAME:
    moveBackground();
    addBullets();
    moveShots();
    moveShip();
    moveFoes();
    moveFrags();
    moveBonuses();
    break;
  case GAMEOVER:
    moveGameover();
    moveBackground();
    addBullets();
    moveShots();
    moveFoes();
    moveFrags();
    break;
  case STAGE_CLEAR:
    moveStageClear();
    moveBackground();
    moveShots();
    moveShip();
    moveFrags();
    moveBonuses();
    break;
  case PAUSE:
    movePause();
    break;
  }
}

static void draw() {
  switch ( status ) {
  case TITLE:
    // Draw background.
    drawBackground();
    drawFoes();
    drawBulletsWake();
    blendScreen();
    // Draw forground.
    drawBullets();
    drawScore();
    drawTitleMenu();
    break;
  case IN_GAME:
    // Draw background.
    drawBackground();
    drawBonuses();
    drawFoes();
    drawBulletsWake();
    drawFrags();
    blendScreen();
    // Draw forground.
    drawShots();
    drawShip();
    drawBullets();
    drawScore();
    break;
  case GAMEOVER:
    // Draw background.
    drawBackground();
    drawFoes();
    drawBulletsWake();
    drawFrags();
    blendScreen();
    // Draw forground.
    drawShots();
    drawBullets();
    drawScore();
    drawGameover();
    break;
  case STAGE_CLEAR:
    // Draw background.
    drawBackground();
    drawBonuses();
    drawFrags();
    blendScreen();
    // Draw forground.
    drawShots();
    drawShip();
    drawScore();
    drawStageClear();
    break;
  case PAUSE:
    // Draw background.
    drawBackground();
    drawBonuses();
    drawFoes();
    drawBulletsWake();
    drawFrags();
    blendScreen();
    // Draw forground.
    drawShots();
    drawShip();
    drawBullets();
    drawScore();
    drawPause();
    break;
  }
}


static int accframe = 0;

static void usage(char *argv0) {
  fprintf(stderr, "Usage: %s [-nosound] [-window] [-reverse] [-brightness n] [-nowait] [-accframe]\n", argv0);
}

static void parseArgs(int argc, char *argv[]) {
  int i;
  for ( i=1 ; i<argc ; i++ ) {
    if ( strcmp(argv[i], "-nosound") == 0 ) {
      noSound = 1;
    } else if ( strcmp(argv[i], "-window") == 0 ) {
      windowMode = 1;
    } else if ( strcmp(argv[i], "-reverse") == 0 ) {
      buttonReversed = 1;
    } else if ( (strcmp(argv[i], "-brightness") == 0) && argv[i+1] ) {
      i++;
      brightness = (int)atoi(argv[i]);
      if ( brightness < 0 || brightness > 256 ) {
	brightness = DEFAULT_BRIGHTNESS;
      }
    } else if ( strcmp(argv[i], "-nowait") == 0 ) {
      nowait = 1;
    } else if ( strcmp(argv[i], "-accframe") == 0 ) {
      accframe = 1;
    } else {
      usage(argv[0]);
      exit(1);
    }
  }
}

int interval = INTERVAL_BASE;
int tick = 0;
static int pPrsd = 1;

static float winScale = -1;
static void useScale()
{
	if(winScale < 0){
		const float r = Window_getScale();
		LOGD("Window scale: %f", r);
		winScale = r;
	}
}

int main(int argc, char *argv[]) {
	int done = 0;
	long prvTickCount = 0;
	int i;
	int btn;
	SDL_Event event;
	long nowTick;
	int frame;
	int sleep=0;

	parseArgs(argc, argv);

	initDegutil();
	initSDL(windowMode);
  
	if ( !noSound ) initSound();
	initFirst();
	initTitle();

	while ( !done ) {
		while(SDL_PollEvent(&event)){
			switch(event.type){
			case SDL_QUIT:
			case SDL_APP_TERMINATING:
				LOGD("terminated");
				done = 1;
				break;
			case SDL_APP_WILLENTERBACKGROUND:
				LOGD("to background.");
				Mix_Pause(-1);
				Mix_PauseMusic();
				sleep=1;
				//done = 1;//FIXME: なぜかうまくquitが降ってこない。
				break;
			case SDL_APP_WILLENTERFOREGROUND:
				LOGD("to foreground.");
				Mix_Resume(-1);
				Mix_ResumeMusic();
				sleep=0;
				break;
			case  SDL_FINGERMOTION:
				useScale();
				onTapMoveAttr(
					event.tfinger.touchId,
					event.tfinger.fingerId,
					event.tfinger.x/winScale * SCREEN_WIDTH,
					event.tfinger.y/winScale * SCREEN_HEIGHT,
					event.tfinger.dx/winScale * SCREEN_WIDTH,
					event.tfinger.dy/winScale * SCREEN_HEIGHT);
				break;
			case SDL_FINGERDOWN:
				useScale();
				onTapDownAttr(
					event.tfinger.touchId,
					event.tfinger.fingerId,
					event.tfinger.x/winScale * SCREEN_WIDTH,
					event.tfinger.y/winScale * SCREEN_HEIGHT,
					event.tfinger.dx/winScale * SCREEN_WIDTH,
					event.tfinger.dy/winScale * SCREEN_HEIGHT);
				break;
			case SDL_FINGERUP:
				useScale();
				onTapUpAttr(
					event.tfinger.touchId,
					event.tfinger.fingerId,
					event.tfinger.x/winScale * SCREEN_WIDTH,
					event.tfinger.y/winScale * SCREEN_HEIGHT,
					event.tfinger.dx/winScale * SCREEN_WIDTH,
					event.tfinger.dy/winScale * SCREEN_HEIGHT);
				break;
			}
		}
		keys = SDL_GetKeyboardState(NULL);
		if(sleep) {
			SDL_Delay(50);
			prvTickCount = SDL_GetTicks();
			continue;
		}
		if ( keys[SDL_SCANCODE_ESCAPE] == SDL_PRESSED ) {
			LOGD("Escape pressed.");
			done = 1;
		}
		if ( keys[SDL_SCANCODE_P] == SDL_PRESSED ) {
			if ( !pPrsd ) {
				if ( status == IN_GAME ) {
					status = PAUSE;
				} else if ( status == PAUSE ) {
					status = IN_GAME;
				}
			}
			pPrsd = 1;
		} else {
			pPrsd = 0;
		}

		nowTick = SDL_GetTicks();
		frame = (int)(nowTick-prvTickCount) / interval;
		if ( frame <= 0 ) {
			frame = 1;
			SDL_Delay(prvTickCount+interval-nowTick);
			if ( accframe ) {
				prvTickCount = SDL_GetTicks();
			} else {
				prvTickCount += interval;
			}
		} else if ( frame > 5 ) {
			frame = 5;
			prvTickCount = nowTick;
		} else {
			prvTickCount += frame*interval;
		}
		for ( i=0 ; i<frame ; i++ ) {
			move();
			tick++;
		}
		smokeScreen();
		draw();
		flipScreen();
	}
	quitLast();
	LOGD("Good bye...");
}
