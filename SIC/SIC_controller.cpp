#include "SIC_controller.hpp"
#include <cmath>
#include <algorithm>
#include <complex>
#include <global/global.hpp>

#define _MINdB  (MIN_POWER)
#define _MAXdB  (MAX_POWER)

//#define _SIC_DEBUG_

SIC_controller::SIC_controller(std::complex<float> input_ref){
  this->weight_cur = std::polar((float)dB2Amp(SIC_REF_POWER), Deg2Rad(SIC_REF_PHASE)); 
  this->SIC_cha = input_ref/this->weight_cur;
}

int SIC_controller::setCurrentAmp(std::complex<float> amp_Rx){
  amp_SI = amp_Rx - SIC_cha * weight_cur;
  weight_cur = (target_power - amp_SI)/SIC_cha;
  
#ifdef  _SIC_DEBUG_
  std::cout<<"ANT amp : "<<amp_Ant<<std::endl;
  std::cout<<"SIC channel : "<<SIC_cha<<std::endl;
  std::cout<<"weight_cur : "<<weight_cur<<std::endl;
  std::cout<<"new dB : "<<Amp2dB(std::abs(weight_cur))<<std::endl;
  std::cout<<"phase : "<<Rad2Deg(std::arg(weight_cur))<<std::endl;
  std::cout<<"Expected : "<<amp_Ant + weight_cur*SIC_cha<<std::endl;
#endif

  return 0;
}

std::complex<float> SIC_controller::getWeight(void){
  return weight_cur;
}

float SIC_controller::getPower(void){
  return Amp2dB(std::abs(weight_cur));
}

float SIC_controller::getPhase(void){
  return Rad2Deg(std::arg(weight_cur));
}

int SIC_controller::setPower(float power_a){
  this->weight_cur = std::polar((float)dB2Amp(power_a), std::arg(weight_cur));
  return 0;
}

int SIC_controller::setPhase(float phase_a){
  this->weight_cur = std::polar(std::abs(weight_cur), Deg2Rad(phase_a));
  return Rad2Deg(std::arg(weight_cur));
}

int SIC_controller::setTargetPower(std::complex<float> target_power)
{
  this->target_power = target_power;
  return 0;
}
