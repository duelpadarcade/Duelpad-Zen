#ifndef _RAINBOW_H_
#define _RAINBOW_H_

#include "../Animation.hpp"
#include "hardware/clocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../AnimationStation.hpp"

// clamp rainbowCycleTime to [1 ... INT16_MAX]
#define RAINBOW_CYCLE_INCREMENT   10
#define RAINBOW_CYCLE_MAX         INT16_MAX - RAINBOW_CYCLE_INCREMENT
#define RAINBOW_CYCLE_MIN         1         + RAINBOW_CYCLE_INCREMENT
class Rainbow : public Animation {
public:
  Rainbow(PixelMatrix &matrix);
  ~Rainbow() {};

  void Animate(RGB (&frame)[100]);
  void ParameterUp();
  void ParameterDown();

protected:
  int currentFrame = 0;
  bool reverse = false;
  absolute_time_t nextRunTime = nil_time;
};

#endif
