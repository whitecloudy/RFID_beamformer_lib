#include <iostream>
#include <string>
#include "lib/AD5371_ctrl/Phase_Attenuator_controller.hpp"

int main(int argc, char * argv[]){
    Phase_Attenuator_controller pa_control(0);

    int ant_quantity = std::stoi(argv[1]);

    for(int i = 0; i < ant_quantity; i++){
        int ant_num = 0;
        float phase = 0;
        float power = 0;

        std::cout << "Ant num : ";
        std::cin >> ant_num;
        std::cout << "Phase : ";
        std::cin >> phase;
        std::cout << "Power : ";
        std::cin >> power;
        
        std::cout << "Ant "<<ant_num<<" with Phase "<<phase<< " degree and Power "<<power<<"dB\n\n";

        pa_control.ant_on(ant_num);
        pa_control.phase_control(ant_num, power, phase);
    }

    char for_waiting[128];
    std::cin >> for_waiting;

    return 0;
}
