#include "Phase_Attenuator_controller.hpp"
#include "lib/hash-library/sha256.h"
#include <string>
#include <cmath>
#include <fstream>
#include <cstring>


enum VOUTNUM
{
  ANT1_phase = 23,
  ANT2_phase = 22,
  ANT3_phase = 21,
  ANT4_phase = 20,
  ANT5_phase = 19,
  ANT6_phase = 18,
  ANT7_phase = 17,
  ANT8_phase = 16,
  ANT9_phase = 7,
  ANT10_phase = 6,
  ANT11_phase = 5,
  ANT12_phase = 4,
  ANT13_phase = 3,
  ANT14_phase = 2,
  ANT15_phase = 1,
  ANT16_phase = 0,
  ANT1_attenuator = 31,
  ANT2_attenuator = 30,
  ANT3_attenuator = 29,
  ANT4_attenuator = 28,
  ANT5_attenuator = 27,
  ANT6_attenuator = 26,
  ANT7_attenuator = 25,
  ANT8_attenuator = 24,
  ANT9_attenuator = 39,
  ANT10_attenuator = 38,
  ANT11_attenuator = 37,
  ANT12_attenuator = 36,
  ANT13_attenuator = 35,
  ANT14_attenuator = 34,
  ANT15_attenuator = 33,
  ANT16_attenuator = 32
};

#define MISC_DIR (std::string("../misc/"))



const char PHASE[] = {ANT1_phase, ANT2_phase, ANT3_phase, ANT4_phase, ANT5_phase, ANT6_phase, ANT7_phase, ANT8_phase, ANT9_phase, ANT10_phase, ANT11_phase, ANT12_phase, ANT13_phase, ANT14_phase, ANT15_phase, ANT16_phase};
const char ATTENUATOR[] = {ANT1_attenuator, ANT2_attenuator, ANT3_attenuator, ANT4_attenuator, ANT5_attenuator, ANT6_attenuator, ANT7_attenuator, ANT8_attenuator, ANT9_attenuator, ANT10_attenuator, ANT11_attenuator, ANT12_attenuator, ANT13_attenuator, ANT14_attenuator, ANT15_attenuator, ANT16_attenuator};


//#define __DEBUG__

bool Phase_Attenuator_controller::read_cal_ref_file(std::vector<struct cal_ref>& cal_data, std::string filename)
{
  std::ifstream file(filename);

  if(file.fail())
  {
    file.close();
    return false;
  }else
  {
    //we close it immediately because we just opened only for file exists check
    file.close();

    io::CSVReader<4> csv_reader(filename);

    csv_reader.read_header(io::ignore_extra_column, "Phase(V)", "Attenuator(V)", "real", "imag");

    double ph_V, po_V, real, imag;

    while(csv_reader.read_row(ph_V, po_V, real, imag)){
      struct cal_ref v_data;
      v_data.signal = std::complex<double>(real,imag);
      v_data.phase = v_data.signal/std::abs(v_data.signal);
      v_data.power = std::norm(v_data.signal);
      v_data.ph_V = ph_V;
      v_data.po_V = po_V;

      cal_data.push_back(v_data);
    }

    return true;
  }
  return true;
}

bool Phase_Attenuator_controller::write_cal_ref_file(std::vector<struct cal_ref>& cal_data, std::string filename)
{
  std::ofstream file(filename);

  if(file.fail())
  {
    std::cerr<<"Saving Cache failed" <<std::endl;
    file.close();
    return false;
  }else
  {
    file << "Phase(V), Attenuator(V), real, imag"<<std::endl;
    for(unsigned int i = 0; i < cal_data.size() ; i++)
    {
      file << cal_data[i].ph_V << ", "<< cal_data[i].po_V << ", " << cal_data[i].signal.real() << ", " << cal_data[i].signal.imag() << std::endl;
    }
    return true;
  }
}

bool Phase_Attenuator_controller::load_cache(int ant, std::string filename, int expected_preset_len)
{
  filename = MISC_DIR +"."+ filename;
  std::vector<struct cal_ref> cal_data;

  if(read_cal_ref_file(cal_data, filename))
  {
    int preset_length = (cal_data.size())/POWER_num;
    if(expected_preset_len != preset_length)
      return false;

    for(int power_idx= 0; power_idx < POWER_num; power_idx++)
    {
      V_preset[ant][power_idx].reserve(preset_length);
      for(int i = 0; i < preset_length; i++)
      {
        V_preset[ant][power_idx].push_back(cal_data[power_idx*preset_length + i]);
      }
    }
  }else
    return false;

  std::ifstream index_file(filename+"_index");

  if(index_file.fail())
    return false;
  else
  {
    for(int power_idx = 0; power_idx < POWER_num; power_idx++)
    {
      int index = 0;
      for(int phase_idx = 0; phase_idx < 360; phase_idx++)
      {
        index_file >> index;
        index_V_preset[ant][power_idx][phase_idx] = index;
      }
    }
  }

  index_file.close();

  return true;
}

bool Phase_Attenuator_controller::save_cache(int ant, std::string filename)
{
  filename = MISC_DIR +"."+ filename;

  int preset_length = V_preset[ant][0].size();
  std::vector<struct cal_ref> merged_data(preset_length * POWER_num);

  for(int i = 0; i < POWER_num; i++)
  {
    memcpy(merged_data.data() + preset_length*i, V_preset[ant][i].data(), sizeof(struct cal_ref)*V_preset[ant][i].size());
  }

  write_cal_ref_file(merged_data, filename);

  std::ofstream index_file(filename + "_index");

  if(index_file.fail())
    return false;
  else
  {
    for(int power_idx = 0; power_idx < POWER_num; power_idx++)
    {
      for(int phase_idx = 0; phase_idx < 360; phase_idx++)
      {
        index_file << index_V_preset[ant][power_idx][phase_idx] << std::endl;
      }
    }
  }

  index_file.close();

  return 0;
}



int Phase_Attenuator_controller::load_cal_data(void){
  //loading the calibration data

  std::cout << "Reading calibration data.....";
  for(int i = 0; i<ANT_num; i++){
    std::string filename = MISC_DIR + "calibration_data/ant" + std::to_string(i) + ".csv";
    std::vector<struct cal_ref> cal_data;

    read_cal_ref_file(cal_data, filename);

    SHA256 sha256;

    std::string file_hash(sha256(cal_data.data(), sizeof(struct cal_ref)*cal_data.size()));

    if(!(load_cache(i, file_hash, cal_data.size()/2)))
    {
      std::cerr << "No cache files" <<std::endl;
      fill_V_preset(i, cal_data);
      set_integer_index(i);
      save_cache(i, file_hash);
    }

  }

  std::cout << "Done"<<std::endl;

  return 0;
}

int Phase_Attenuator_controller::fill_V_preset(int ant_num, std::vector<struct cal_ref> cal_data)
{
  std::vector<struct cal_ref> phase_shifter, attenuator;

  int cal_len = cal_data.size()/2;

  for(int i = 0; i < cal_len; i++)
  {
    phase_shifter.push_back(cal_data[i]);
    cal_data[i + cal_len].signal /= phase_shifter[0].signal;
    attenuator.push_back(cal_data[i + cal_len]);
  }

  for(int power_idx = dB2idx(MIN_POWER); power_idx <= dB2idx(MAX_POWER); power_idx++)
  {
    double target_amp = idx2Amp(power_idx);

    V_preset[ant_num][power_idx].reserve(cal_len);

    for(int ph_idx = 0; ph_idx < cal_len; ph_idx++)
    {
      std::complex<double> phase_signal = phase_shifter[ph_idx].signal;

      int best_at_idx = 0;
      double best_amp = 99999999.0;

      int at_idx = 0;
      for(int at_idx = 0; at_idx < cal_len; at_idx++)
      {
        double cur_amp = std::abs(std::abs(phase_signal * attenuator[at_idx].signal) - target_amp);
        if(best_amp > cur_amp)
        {
          best_amp = cur_amp;
          best_at_idx = at_idx;
        }
      }


      struct cal_ref v_data;
      v_data.signal = phase_signal * attenuator[best_at_idx].signal;
      v_data.phase = v_data.signal/std::abs(v_data.signal);
      v_data.power = std::norm(v_data.signal);
      v_data.ph_V = phase_shifter[ph_idx].ph_V;
      v_data.po_V = attenuator[best_at_idx].po_V;
      V_preset[ant_num][power_idx].push_back(v_data);

    }
  }

  return 0;
}


int preset_finder(std::vector<struct cal_ref> V_preset, float phase){
  int vec_len = V_preset.size();  
  std::complex<double> phase_com = std::polar(1.0f, Deg2Rad(phase));

  int min_idx = 0;
  double min_value = 999999.9;

  for(int i = 0; i< vec_len; i++){
    if(std::abs(V_preset[i].phase - phase_com) < min_value){
      min_idx = i;
      min_value = std::abs(V_preset[i].phase - phase_com);
    }
  }

  return min_idx;
}

int preset_finder(std::vector<struct cal_ref> V_preset, int start, int end, float phase){
  return preset_finder(V_preset, phase);
}


int Phase_Attenuator_controller::find_matched_preset(int ant, int power, float phase){
  return preset_finder(V_preset[ant][power], phase);
}


int Phase_Attenuator_controller::set_integer_index(int ant){
  for(int power_idx = 0; power_idx < POWER_num; power_idx++){
    for(int i = 0; i < 360; i++){
      index_V_preset[ant][power_idx][i] = find_matched_preset(ant, power_idx ,i);
    }
  }

  return 0;
}


int Phase_Attenuator_controller::voltage_index_search(int ant, int power_idx, int phase){
  while(phase < 0)
    phase += 360;

  return index_V_preset[ant][power_idx][phase%360];
}

int Phase_Attenuator_controller::voltage_index_search(int ant, int power_idx ,float phase){
  while(phase > 360)
    phase -= 360.0;
  while(phase < 0)
    phase += 360.0;

  return find_matched_preset(ant, power_idx, phase);
}

int Phase_Attenuator_controller::phase_setup(int ant, int power_idx, int index){
  ant_power_setting[ant] = power_idx;
  int at_result = V.voltage_modify(ATTENUATOR[ant], V_preset[ant][power_idx][index].po_V);
  int ph_result = V.voltage_modify(PHASE[ant], V_preset[ant][power_idx][index].ph_V);

  return at_result&&ph_result;
}

int Phase_Attenuator_controller::phase_control(int ant, float power, int phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  ant_power_setting[ant] = dB2idx(power);
  return phase_control(ant, phase);
}

int Phase_Attenuator_controller::phase_control(int ant, float power, float phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  ant_power_setting[ant] = dB2idx(power);
  return phase_control(ant, phase);
}

int Phase_Attenuator_controller::phase_control(int ant, int phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  int index = voltage_index_search(ant, ant_power_setting[ant], phase);
  return phase_setup(ant, ant_power_setting[ant], index);
}

int Phase_Attenuator_controller::phase_control(int ant, float phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }  
  int index = voltage_index_search(ant, ant_power_setting[ant], phase);
  return phase_setup(ant, ant_power_setting[ant], index);
}

int Phase_Attenuator_controller::data_apply(void){
  return V.data_apply();
}

int Phase_Attenuator_controller::ant_off(int ant_num){
  ant_power_setting[ant_num] = PoffIDX;
  return V.voltage_modify(ATTENUATOR[ant_num], 0);
}

int Phase_Attenuator_controller::ant_on(int ant_num, float power){
  ant_power_setting[ant_num] = dB2idx(power);
  return phase_control(ant_num, power, 0);
}



Phase_Attenuator_controller::Phase_Attenuator_controller(void){
  init();
}

Phase_Attenuator_controller::Phase_Attenuator_controller(int phase){
  init();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}

Phase_Attenuator_controller::Phase_Attenuator_controller(float phase){
  init();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}

int Phase_Attenuator_controller::init(void){
  if(load_cal_data())
    std::cout<<"Error : Loading calibration data failed"<<std::endl;

  std::fill_n(ant_power_setting, ANT_num, DEFAULT_POWER_idx);
  std::cout << ant_power_setting[0]<<std::endl;

  return 0;
}
