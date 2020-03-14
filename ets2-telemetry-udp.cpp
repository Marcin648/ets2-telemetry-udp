#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <list>
#include <scssdk_telemetry.h>
#include <eurotrucks2/scssdk_eut2.h>
#include <eurotrucks2/scssdk_telemetry_eut2.h>
#include <amtrucks/scssdk_ats.h>
#include <amtrucks/scssdk_telemetry_ats.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "telemetry.hpp"

#define VERSION "v0.1.0"
#define TELEMETRY_VERSION 0
#define LOG_PREFIX "[TELE-UDP] "
#define BIND_PORT 23444

/*
    Telemetry
*/

telemetry_truck_s tele_truck;

/*
    Log
*/
scs_log_t game_log = NULL;

void log(scs_log_type_t type, const char* format, ...){
	if(game_log == NULL){
        return;
    }
    char buffer[512] = LOG_PREFIX; 
    size_t buffer_offset = sizeof(LOG_PREFIX)-1;

	va_list args;
	va_start(args, format);
	vsnprintf(buffer+buffer_offset, sizeof(buffer)-buffer_offset, format, args);
	va_end(args);

    game_log(type, buffer);
}

/*
    Network
*/
int net_socket = -1;
bool net_initialized = false;

#ifndef _WIN32
#define closesocket(socket) close(socket)
#endif

bool net_init(){
    if(net_initialized){
        return true;
    }

    #ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(2, 0);
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0){
        log(SCS_LOG_TYPE_error, "WinSock2 failed to initialized!");
        return false;
    }
    #endif

    if(net_socket != -1){
        closesocket(net_socket);
        net_socket = -1;
    }

    net_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(net_socket == -1){
        #ifdef _WIN32
            log(SCS_LOG_TYPE_error, "Failed to create socket. (%s)", WSAGetLastError());
        #else
            log(SCS_LOG_TYPE_error, "Failed to create socket.");
        #endif
        return false;
    }

    int non_blocking_success = -1;
    #ifdef WIN32
    unsigned long non_blocking = 1;
    non_blocking_success = ioctlsocket(net_socket, FIONBIO, &non_blocking);
    #else
    non_blocking_success = fcntl(net_socket, F_SETFL, O_NONBLOCK);
    #endif

    if(non_blocking_success != 0){
        log(SCS_LOG_TYPE_error, "Failed to set socket non blocking.");
        closesocket(net_socket);
        net_socket = -1;
        return false;
    }

    sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(BIND_PORT);
    
    /*int bind_success = bind(net_socket, (sockaddr*)&bind_addr, sizeof(bind_addr));
    if(bind_success == -1){
        #ifdef _WIN32
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket. (%s)", WSAGetLastError());
        #else
            log(SCS_LOG_TYPE_error, "Failed to assign address to socket.");
        #endif
        closesocket(net_socket);
        net_socket = -1;
        return false;
    }*/
    
    log(SCS_LOG_TYPE_message, "Server start on port %i.", BIND_PORT);

    net_initialized = true;
    return true;
}

void net_close(){
    if(net_initialized){
        if(net_socket != -1){
            closesocket(net_socket);
            net_socket = -1;
        }
        #ifdef _WIN32
        WSACleanup();
        #endif
        net_initialized = false;
        log(SCS_LOG_TYPE_message, "Socket closed.");
    }
}

/*
    SCS API
*/

// STORING

SCSAPI_VOID telemetry_store_bool(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint8_t *>(context) = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_s32(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<int32_t *>(context) = value->value_s32.value; 
}

SCSAPI_VOID telemetry_store_u32(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint32_t *>(context) = value->value_u32.value; 
}

SCSAPI_VOID telemetry_store_float(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<float *>(context) = value->value_float.value; 
}

SCSAPI_VOID telemetry_store_fvector(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    telemetry_fvector_t *fvector = static_cast<telemetry_fvector_t *>(context);
    fvector->x = value->value_fvector.x;
    fvector->y = value->value_fvector.y;
    fvector->z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_fplacement(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    telemetry_fplacement_t *fplacement = static_cast<telemetry_fplacement_t *>(context);
    fplacement->position.x = value->value_fplacement.position.x;
    fplacement->position.y = value->value_fplacement.position.y;
    fplacement->position.z = value->value_fplacement.position.z;

    fplacement->orientation.heading = value->value_fplacement.orientation.heading;
    fplacement->orientation.pitch = value->value_fplacement.orientation.pitch;
    fplacement->orientation.roll = value->value_fplacement.orientation.roll;
}

SCSAPI_VOID telemetry_store_dplacement(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    telemetry_fplacement_t *fplacement = static_cast<telemetry_fplacement_t *>(context);
    fplacement->position.x = value->value_dplacement.position.x;
    fplacement->position.y = value->value_dplacement.position.y;
    fplacement->position.z = value->value_dplacement.position.z;

    fplacement->orientation.heading = value->value_dplacement.orientation.heading;
    fplacement->orientation.pitch = value->value_dplacement.orientation.pitch;
    fplacement->orientation.roll = value->value_dplacement.orientation.roll;
}

// API

SCSAPI_VOID telemetry_game_start(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    //telemetry.game_paused = 0;
}

SCSAPI_VOID telemetry_game_pause(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    //telemetry.game_paused = 1;
}

SCSAPI_VOID telemetry_frame_start(const scs_event_t /*event*/, const void *const event_info, const scs_context_t /*context*/){
    ;
}

SCSAPI_VOID telemetry_frame_end(const scs_event_t /*event*/, const void *const event_info, const scs_context_t /*context*/){
    if(net_initialized && net_socket != -1){
        /*
        log(SCS_LOG_TYPE_message, "Frame end: %u", rand());
        struct sockaddr_in client_addr;
        int len;
        recvfrom(net_socket, NULL, 0, 0, (sockaddr*)(&client_addr), &len);
        sendto(net_socket, reinterpret_cast<char *>(&telemetry), sizeof(telemetry), 0, (sockaddr*)(&client_addr), len);
        */
        sockaddr_in bind_addr;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        bind_addr.sin_port = htons(BIND_PORT);
        sendto(net_socket, reinterpret_cast<char *>(&tele_truck), sizeof(tele_truck), 0, (sockaddr*)(&bind_addr), sizeof(bind_addr));
    }
}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t *const params){
    if (version != SCS_TELEMETRY_VERSION_CURRENT) {
		return SCS_RESULT_unsupported;
	}


    const scs_telemetry_init_params_v100_t *const version_params = static_cast<const scs_telemetry_init_params_v100_t *>(params);
    game_log = version_params->common.log;

    #define REG_INDEXED_CHANNEL(name, index, type, target) version_params->register_for_channel(SCS_TELEMETRY_##name, index, SCS_VALUE_TYPE_##type, SCS_TELEMETRY_CHANNEL_FLAG_each_frame, telemetry_store_##type, target)
    #define REG_CHANNEL(name, type, target) REG_INDEXED_CHANNEL(name, SCS_U32_NIL, type, target)
    #define REG_EVENT(type, callback) (version_params->register_for_event(type, callback, NULL))

    REG_EVENT(SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start);
    REG_EVENT(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end);
    REG_EVENT(SCS_TELEMETRY_EVENT_paused, telemetry_game_pause);
    REG_EVENT(SCS_TELEMETRY_EVENT_started, telemetry_game_start);

    REG_CHANNEL(TRUCK_CHANNEL_world_placement, dplacement, &tele_truck.world_placement);
    REG_CHANNEL(TRUCK_CHANNEL_local_linear_velocity, fvector, &tele_truck.local_linear_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_local_angular_velocity, fvector, &tele_truck.local_angular_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_local_linear_acceleration, fvector, &tele_truck.local_linear_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_local_angular_acceleration, fvector, &tele_truck.local_angular_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_offset, fplacement, &tele_truck.cabin_offset);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_angular_velocity, fvector, &tele_truck.cabin_angular_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_angular_acceleration, fvector, &tele_truck.cabin_angular_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_head_offset, fplacement, &tele_truck.head_offset);
    REG_CHANNEL(TRUCK_CHANNEL_speed, float, &tele_truck.speed);
    REG_CHANNEL(TRUCK_CHANNEL_engine_rpm, float, &tele_truck.engine_rpm);
    REG_CHANNEL(TRUCK_CHANNEL_engine_gear, s32, &tele_truck.engine_gear);
    REG_CHANNEL(TRUCK_CHANNEL_displayed_gear, s32, &tele_truck.displayed_gear);
    REG_CHANNEL(TRUCK_CHANNEL_input_steering, float, &tele_truck.input_steering);
    REG_CHANNEL(TRUCK_CHANNEL_input_throttle, float, &tele_truck.input_throttle);
    REG_CHANNEL(TRUCK_CHANNEL_input_brake, float, &tele_truck.input_brake);
    REG_CHANNEL(TRUCK_CHANNEL_input_clutch, float, &tele_truck.input_clutch);
    REG_CHANNEL(TRUCK_CHANNEL_effective_steering, float, &tele_truck.effective_steering);
    REG_CHANNEL(TRUCK_CHANNEL_effective_throttle, float, &tele_truck.effective_throttle);
    REG_CHANNEL(TRUCK_CHANNEL_effective_brake, float, &tele_truck.effective_brake);
    REG_CHANNEL(TRUCK_CHANNEL_effective_clutch, float, &tele_truck.effective_clutch);
    REG_CHANNEL(TRUCK_CHANNEL_cruise_control, float, &tele_truck.cruise_control);
    REG_CHANNEL(TRUCK_CHANNEL_hshifter_slot, u32, &tele_truck.hshifter_slot);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_hshifter_selector, 0, indexed_bool, &tele_truck.hshifter_selector);
    REG_CHANNEL(TRUCK_CHANNEL_parking_brake, bool, &tele_truck.parking_brake);
    REG_CHANNEL(TRUCK_CHANNEL_motor_brake, bool, &tele_truck.motor_brake);
    REG_CHANNEL(TRUCK_CHANNEL_retarder_level, u32, &tele_truck.retarder_level);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure, float, &tele_truck.brake_air_pressure);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure_warning, bool, &tele_truck.brake_air_pressure_warning);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure_emergency, bool, &tele_truck.brake_air_pressure_emergency);
    REG_CHANNEL(TRUCK_CHANNEL_brake_temperature, float, &tele_truck.brake_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_fuel, float, &tele_truck.fuel);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_warning, bool, &tele_truck.fuel_warning);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_average_consumption, float, &tele_truck.fuel_average_consumption);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_range, float, &tele_truck.fuel_range);
    REG_CHANNEL(TRUCK_CHANNEL_adblue, float, &tele_truck.adblue);
    REG_CHANNEL(TRUCK_CHANNEL_adblue_warning, bool, &tele_truck.adblue_warning);
    REG_CHANNEL(TRUCK_CHANNEL_adblue_average_consumption, float, &tele_truck.adblue_average_consumption);
    REG_CHANNEL(TRUCK_CHANNEL_oil_pressure, float, &tele_truck.oil_pressure);
    REG_CHANNEL(TRUCK_CHANNEL_oil_pressure_warning, bool, &tele_truck.oil_pressure_warning);
    REG_CHANNEL(TRUCK_CHANNEL_oil_temperature, float, &tele_truck.oil_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_water_temperature, float, &tele_truck.water_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_water_temperature_warning, bool, &tele_truck.water_temperature_warning);
    REG_CHANNEL(TRUCK_CHANNEL_battery_voltage, float, &tele_truck.battery_voltage);
    REG_CHANNEL(TRUCK_CHANNEL_battery_voltage_warning, bool, &tele_truck.battery_voltage_warning);
    REG_CHANNEL(TRUCK_CHANNEL_electric_enabled, bool, &tele_truck.electric_enabled);
    REG_CHANNEL(TRUCK_CHANNEL_engine_enabled, bool, &tele_truck.engine_enabled);
    REG_CHANNEL(TRUCK_CHANNEL_lblinker, bool, &tele_truck.lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_rblinker, bool, &tele_truck.rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_lblinker, bool, &tele_truck.light_lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_rblinker, bool, &tele_truck.light_rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_parking, bool, &tele_truck.light_parking);
    REG_CHANNEL(TRUCK_CHANNEL_light_low_beam, bool, &tele_truck.light_low_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_high_beam, bool, &tele_truck.light_high_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_front, u32, &tele_truck.light_aux_front);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_roof, u32, &tele_truck.light_aux_roof);
    REG_CHANNEL(TRUCK_CHANNEL_light_beacon, bool, &tele_truck.light_beacon);
    REG_CHANNEL(TRUCK_CHANNEL_light_brake, bool, &tele_truck.light_brake);
    REG_CHANNEL(TRUCK_CHANNEL_light_reverse, bool, &tele_truck.light_reverse);
    REG_CHANNEL(TRUCK_CHANNEL_wipers, bool, &tele_truck.wipers);
    REG_CHANNEL(TRUCK_CHANNEL_dashboard_backlight, float, &tele_truck.dashboard_backlight);
    REG_CHANNEL(TRUCK_CHANNEL_wear_engine, float, &tele_truck.wear_engine);
    REG_CHANNEL(TRUCK_CHANNEL_wear_transmission, float, &tele_truck.wear_transmission);
    REG_CHANNEL(TRUCK_CHANNEL_wear_cabin, float, &tele_truck.wear_cabin);
    REG_CHANNEL(TRUCK_CHANNEL_wear_chassis, float, &tele_truck.wear_chassis);
    REG_CHANNEL(TRUCK_CHANNEL_wear_wheels, float, &tele_truck.wear_wheels);
    REG_CHANNEL(TRUCK_CHANNEL_odometer, float, &tele_truck.odometer);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_distance, float, &tele_truck.navigation_distance);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_time, float, &tele_truck.navigation_time);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_speed_limit, float, &tele_truck.navigation_speed_limit);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_susp_deflection, 0, indexed_float, &tele_truck.wheel_susp_deflection);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_on_ground, 0, indexed_bool, &tele_truck.wheel_on_ground);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_substance, 0, indexed_u32, &tele_truck.wheel_substance);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_velocity, 0, indexed_float, &tele_truck.wheel_velocity);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_steering, 0, indexed_float, &tele_truck.wheel_steering);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_rotation, 0, indexed_float, &tele_truck.wheel_rotation);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_lift, 0, indexed_float, &tele_truck.wheel_lift);
    //REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_lift_offset, 0, indexed_float, &tele_truck.wheel_lift_offset);

    log(SCS_LOG_TYPE_message, "Telemetry UDP " VERSION);
    log(SCS_LOG_TYPE_message, "Truck telemetry size: %i", sizeof(telemetry_truck_s));
    if(!net_initialized){
        if(!net_init()){
            log(SCS_LOG_TYPE_error, "Failed to initialize network.");
            return SCS_RESULT_generic_error;
        }
    }

    return SCS_RESULT_ok;
}

SCSAPI_VOID scs_telemetry_shutdown(void)
{
  net_close();
}