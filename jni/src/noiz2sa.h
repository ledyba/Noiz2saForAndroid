/*
 * $Id: noiz2sa.h,v 1.4 2003/02/09 07:34:16 kenta Exp $
 *
 * Copyright 2002 Kenta Cho. All rights reserved.
 */

/**
 * Noiz2sa header file.
 *
 * @version $Revision: 1.4 $
 */
#define randN(N) (rand()%(N))
#define randNS(N) (rand()%(N<<1)-N)
#define randNS2(N) ((rand()%(N)-(N>>1)) + (rand()%(N)-(N>>1)))
#define absN(a) ((a) < 0 ? - (a) : (a))

#define INTERVAL_BASE 16

#define CAPTION "Noiz2sa"
#define VERSION_NUM 50

#define NOT_EXIST -999999

enum GameMode {
	TITLE=0,
	IN_GAME=1,
	GAMEOVER=2,
	STAGE_CLEAR=3,
	PAUSE=4
};

extern enum GameMode status;
extern int interval;
extern int tick;

void quitLast();
void initTitleStage(int stg);
void initTitle();
void initGame(int stg);
void initGameover();
void initStageClear();

