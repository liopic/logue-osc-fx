#include "usermodfx.h"

static float param_time, param_depth;
const int MAX_ITERATIONS = 4;

void MODFX_INIT(uint32_t platform, uint32_t api)
{
}

float overdrive(const float x, const float time)
{
  // Between 1 and MAX_ITERATIONS, given time=[0, 1]
  int iterations = 1 + time*0.99*MAX_ITERATIONS;

  const int isNegative = x<0;
  float overdriven = x;

  if(isNegative) {
    overdriven = -overdriven;
  }

  while(iterations--) {
    // y = -x^2 + 2x
    overdriven = 2*overdriven - (overdriven*overdriven);
  }

  if(isNegative) {
    overdriven = -overdriven;
  }

  return overdriven;
}

float mixer(const float x, const float time, const float depth)
{
  return x*(1-depth) + overdrive(x, time)*depth;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames)
{
  for (uint32_t i = 0; i < frames; i++)
  {
    float inL = main_xn[i * 2];
    float inR = main_xn[i * 2 + 1];
    float subInL = sub_xn[i * 2];
    float subInR = sub_xn[i * 2 + 1];

    main_yn[i * 2] = mixer(inL, param_time, param_depth);
    main_yn[i * 2 + 1] = mixer(inR, param_time, param_depth);

    sub_yn[i * 2] = mixer(subInL, param_time, param_depth);
    sub_yn[i * 2 + 1] = mixer(subInR, param_time, param_depth);
  }
}

void MODFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index)
  {
    case k_user_modfx_param_time:
      param_time = valf;
      break;
    case k_user_modfx_param_depth:
      param_depth = valf;
      break;
    default:
      break;
  }
}
