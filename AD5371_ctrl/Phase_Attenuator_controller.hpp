#ifndef __PHASE_ATTENUATOR_CONTROLLER__
#define __PHASE_ATTENUATOR_CONTROLLER__

#include <iostream>
#include "Vout_controller.hpp"
#include "csv/csv.h"
#include <complex>
#include <vector>

#define ANT_num   (16)
#define MIN_POWER (-22.0)
#define MAX_POWER (-2.0)
#define POWER_res (0.25)
#define POWER_num ((unsigned int)((MAX_POWER - MIN_POWER)/POWER_res) + 1)
#define dB2idx(_dB)  (unsigned int)(((_dB) - MIN_POWER)/POWER_res)
#define idx2dB(_idx) (double)(_idx*POWER_res + MIN_POWER)
#define PoffIDX    (-1)
#define DEFAULT_POWER (MAX_POWER)
#define DEFAULT_POWER_idx (dB2idx(DEFAULT_POWER))


struct cal_ref{
  std::complex<double> signal;
  std::complex<double> phase;
  double power;
  double ph_V;
  double po_V;
};


class Phase_Attenuator_controller{
  private:
    //Voltage control reference data
    std::vector<struct cal_ref> V_preset[ANT_num][POWER_num];
    int ant_power_setting[ANT_num];
    int index_V_preset[ANT_num][POWER_num][360];

  private:
    bool read_cal_ref_file(std::vector<struct cal_ref>&, std::string);
    bool write_cal_ref_file(std::vector<struct cal_ref>&, std::string);


    Vout_controller V;
    int load_cal_data(void);
    int fill_V_preset(int, std::vector<struct cal_ref>);
    int find_matched_preset(int, int, float);
    int set_integer_index(int);

    int voltage_index_search(int, int, int);
    int voltage_index_search(int, int, float);
    int phase_setup(int, int, int);
    int init(void);

    bool load_cache(int, std::string, int);
    bool save_cache(int, std::string);


  public:
    Phase_Attenuator_controller(void);
    Phase_Attenuator_controller(int);
    Phase_Attenuator_controller(float);
    int phase_control(int, int);
    int phase_control(int, float);
    int phase_control(int, float, int);
    int phase_control(int, float, float);

    int ant_off(int);
    int ant_on(int, float power = DEFAULT_POWER);
    int data_apply();
    void print_integer_index(void);
};

#endif
