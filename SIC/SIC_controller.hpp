#ifndef __SIC_CONTROLLER__
#define __SIC_CONTROLLER__

#include <iostream>
#include <complex>

#define SIC_REF_POWER (-10.0)
#define SIC_REF_PHASE 0

class SIC_controller{
  private:
    std::complex<float> SIC_cha;
    std::complex<float> weight_cur;
    std::complex<float> amp_SI;

    std::complex<float> target_power = std::complex<float>(0.0, 0.0);

  public:
    SIC_controller(std::complex<float>);
    int setCurrentAmp(std::complex<float>);
    std::complex<float> getWeight(void);
    float getPhase(void);
    float getPower(void);
    int setPhase(float);
    int setPower(float);    
    int setTargetPower(std::complex<float>);

};

#endif
