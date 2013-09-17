/*
 * $Id: ship.h,v 1.1.1.1 2002/11/03 11:08:24 kenta Exp $
 *
 * Copyright 2002 Kenta Cho. All rights reserved.
 */

/**
 * Player data.
 *
 * @version $Revision: 1.1.1.1 $
 */
#include "vector.h"

typedef struct {
  Vector pos;
  int cnt, shotCnt;
  int speed;
  int invCnt;

  int tapCount;
} Ship;

extern Ship ship;

extern void initShip();
extern void moveShip();
extern void drawShip();
extern void destroyShip();
extern int getPlayerDeg(int x, int y);

extern void moveShipByTap(int const fingerId, float const dx, float const dy);
extern void startShipShotByTap(int const fingerId);
extern void endShipShotByTap(int const fingerId);
