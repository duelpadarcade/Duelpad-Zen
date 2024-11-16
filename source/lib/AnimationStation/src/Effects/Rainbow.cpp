#include "Rainbow.hpp"

Rainbow::Rainbow(PixelMatrix &matrix) : Animation(matrix) {
}

void Rainbow::Animate(RGB (&frame)[100]) {
  if (!time_reached(this->nextRunTime)) {
    return;
  }

  UpdateTime();
  UpdatePresses(frame);

  for (auto &col : matrix->pixels) {
    for (auto &pixel : col) {
      if (pixel.index == NO_PIXEL.index)
        continue;

      // Count down the timer
      DecrementFadeCounter(pixel.index);

      RGB color = RGB::wheel(this->currentFrame);
      for (auto &pos : pixel.positions)
        frame[pos] = BlendColor(hitColor[pixel.index], color, times[pixel.index]);
    }
  }

  if (reverse) {
    currentFrame--;

    if (currentFrame < 0) {
      currentFrame = 1;
      reverse = false;
    }
  } else {
    currentFrame++;

    if (currentFrame > 255) {
      currentFrame = 254;
      reverse = true;
    }
  }

  this->nextRunTime = make_timeout_time_ms(AnimationStation::options.rainbowCycleTime);
}


void Rainbow::ParameterUp() {
  if (AnimationStation::options.rainbowCycleTime < RAINBOW_CYCLE_MAX) {
    AnimationStation::options.rainbowCycleTime = AnimationStation::options.rainbowCycleTime + RAINBOW_CYCLE_INCREMENT;
  } else {
    AnimationStation::options.rainbowCycleTime = INT16_MAX;
  }
}

void Rainbow::ParameterDown() {
  if (AnimationStation::options.rainbowCycleTime > RAINBOW_CYCLE_MIN) {
    AnimationStation::options.rainbowCycleTime = AnimationStation::options.rainbowCycleTime - RAINBOW_CYCLE_INCREMENT;
  } else {
    AnimationStation::options.rainbowCycleTime = 1;
  }
}
