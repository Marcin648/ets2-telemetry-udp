#include <iostream>
#include <cstdint>

#include "TelemetryClient.hpp"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 23444

int main(int, char**) {
    TelemetryClient telemetry = TelemetryClient();

    if(!telemetry.connect(SERVER_ADDRESS, SERVER_PORT)){
        return -1;
    }

    for(;;){
        telemetry.update();

        std::cout << "Time scale: " <<  telemetry.common.local_scale << std::endl;
        std::cout << "Truck brand: " <<  telemetry.config_truck.brand << std::endl;
        std::cout << "Truck: " <<  telemetry.config_truck.name << std::endl;
        for(size_t i = 0; i < TELE_TRAILER_COUNT; i++){
            if(telemetry.config_trailer[i].id[0] != '\0'){
                std::cout << "Trailer: " << i << ": " << telemetry.config_trailer[i].id << std::endl;
            }
        }
        std::cout << "Job cargo: " <<  telemetry.config_job.cargo << std::endl;
        std::cout << "Truck speed: " <<  telemetry.truck.speed * 3.6f << std::endl;
        std::cout << "Truck fuel: " <<  telemetry.truck.fuel << std::endl;
        std::cout << "Trailer connected: " <<  (int)telemetry.trailer.connected << std::endl;
    }

    telemetry.close();
    return 0;
}
