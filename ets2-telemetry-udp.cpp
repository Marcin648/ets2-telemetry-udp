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

#include <string>

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

telemetry_truck_s telemetry_truck;
telemetry_trailer_s telemetry_trailer;
telemetry_common_s telemetry_common;

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

    /*sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(BIND_PORT);
    
    int bind_success = bind(net_socket, (sockaddr*)&bind_addr, sizeof(bind_addr));
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

SCSAPI_VOID telemetry_store_bool(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint8_t *>(context) = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_s32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<int32_t *>(context) = value->value_s32.value; 
}

SCSAPI_VOID telemetry_store_u32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint32_t *>(context) = value->value_u32.value; 
}

SCSAPI_VOID telemetry_store_float(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<float *>(context) = value->value_float.value; 
}

SCSAPI_VOID telemetry_store_fvector(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    telemetry_fvector_t *fvector = static_cast<telemetry_fvector_t *>(context);
    fvector->x = value->value_fvector.x;
    fvector->y = value->value_fvector.y;
    fvector->z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_fplacement(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    telemetry_fplacement_t *fplacement = static_cast<telemetry_fplacement_t *>(context);
    fplacement->position.x = value->value_fplacement.position.x;
    fplacement->position.y = value->value_fplacement.position.y;
    fplacement->position.z = value->value_fplacement.position.z;

    fplacement->orientation.heading = value->value_fplacement.orientation.heading;
    fplacement->orientation.pitch = value->value_fplacement.orientation.pitch;
    fplacement->orientation.roll = value->value_fplacement.orientation.roll;
}

SCSAPI_VOID telemetry_store_dplacement(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
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
    telemetry_common.game_paused = 0;
}

SCSAPI_VOID telemetry_game_pause(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    telemetry_common.game_paused = 1;
}

SCSAPI_VOID telemetry_frame_start(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    ;
}

SCSAPI_VOID telemetry_frame_end(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    if(net_initialized && net_socket != -1){
        /*
        log(SCS_LOG_TYPE_message, "Frame end: %u", rand());
        struct sockaddr_in client_addr;
        int len;
        recvfrom(net_socket, NULL, 0, 0, (sockaddr*)(&client_addr), &len);
        sendto(net_socket, reinterpret_cast<char *>(&telemetry), sizeof(telemetry), 0, (sockaddr*)(&client_addr), len);
        */
        log(SCS_LOG_TYPE_message, "speed: %f km/s", telemetry_truck.speed * 3.6f);
        log(SCS_LOG_TYPE_message, "rpm: %f", telemetry_truck.engine_rpm);
        log(SCS_LOG_TYPE_message, "connected: %i", telemetry_trailer.connected);
        log(SCS_LOG_TYPE_message, "game time: %i", telemetry_common.game_time);
        std::string a = "";
        
        for(size_t i = 0; i < TELE_TRAILER_WHEEL_COUNT; i++){
            a += std::to_string(telemetry_trailer.wheel_velocity[i]) + ", ";
        }

        log(SCS_LOG_TYPE_message, "telemetry_trailer.wheel_velocity: %s", a.c_str());

        sockaddr_in bind_addr;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        bind_addr.sin_port = htons(BIND_PORT);
        sendto(net_socket, reinterpret_cast<char *>(&telemetry_truck), sizeof(telemetry_truck), 0, (sockaddr*)(&bind_addr), sizeof(bind_addr));
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
    
    // Common channel
    REG_CHANNEL(CHANNEL_local_scale, float, &telemetry_common.local_scale);
    REG_CHANNEL(CHANNEL_game_time, u32, &telemetry_common.game_time);
    REG_CHANNEL(CHANNEL_next_rest_stop, s32, &telemetry_common.next_rest_stop);

    // Truck channel
    REG_CHANNEL(TRUCK_CHANNEL_world_placement, dplacement, &telemetry_truck.world_placement);
    REG_CHANNEL(TRUCK_CHANNEL_local_linear_velocity, fvector, &telemetry_truck.local_linear_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_local_angular_velocity, fvector, &telemetry_truck.local_angular_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_local_linear_acceleration, fvector, &telemetry_truck.local_linear_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_local_angular_acceleration, fvector, &telemetry_truck.local_angular_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_offset, fplacement, &telemetry_truck.cabin_offset);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_angular_velocity, fvector, &telemetry_truck.cabin_angular_velocity);
    REG_CHANNEL(TRUCK_CHANNEL_cabin_angular_acceleration, fvector, &telemetry_truck.cabin_angular_acceleration);
    REG_CHANNEL(TRUCK_CHANNEL_head_offset, fplacement, &telemetry_truck.head_offset);
    REG_CHANNEL(TRUCK_CHANNEL_speed, float, &telemetry_truck.speed);
    REG_CHANNEL(TRUCK_CHANNEL_engine_rpm, float, &telemetry_truck.engine_rpm);
    REG_CHANNEL(TRUCK_CHANNEL_engine_gear, s32, &telemetry_truck.engine_gear);
    REG_CHANNEL(TRUCK_CHANNEL_displayed_gear, s32, &telemetry_truck.displayed_gear);
    REG_CHANNEL(TRUCK_CHANNEL_input_steering, float, &telemetry_truck.input_steering);
    REG_CHANNEL(TRUCK_CHANNEL_input_throttle, float, &telemetry_truck.input_throttle);
    REG_CHANNEL(TRUCK_CHANNEL_input_brake, float, &telemetry_truck.input_brake);
    REG_CHANNEL(TRUCK_CHANNEL_input_clutch, float, &telemetry_truck.input_clutch);
    REG_CHANNEL(TRUCK_CHANNEL_effective_steering, float, &telemetry_truck.effective_steering);
    REG_CHANNEL(TRUCK_CHANNEL_effective_throttle, float, &telemetry_truck.effective_throttle);
    REG_CHANNEL(TRUCK_CHANNEL_effective_brake, float, &telemetry_truck.effective_brake);
    REG_CHANNEL(TRUCK_CHANNEL_effective_clutch, float, &telemetry_truck.effective_clutch);
    REG_CHANNEL(TRUCK_CHANNEL_cruise_control, float, &telemetry_truck.cruise_control);
    REG_CHANNEL(TRUCK_CHANNEL_hshifter_slot, u32, &telemetry_truck.hshifter_slot);
    REG_CHANNEL(TRUCK_CHANNEL_parking_brake, bool, &telemetry_truck.parking_brake);
    REG_CHANNEL(TRUCK_CHANNEL_motor_brake, bool, &telemetry_truck.motor_brake);
    REG_CHANNEL(TRUCK_CHANNEL_retarder_level, u32, &telemetry_truck.retarder_level);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure, float, &telemetry_truck.brake_air_pressure);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure_warning, bool, &telemetry_truck.brake_air_pressure_warning);
    REG_CHANNEL(TRUCK_CHANNEL_brake_air_pressure_emergency, bool, &telemetry_truck.brake_air_pressure_emergency);
    REG_CHANNEL(TRUCK_CHANNEL_brake_temperature, float, &telemetry_truck.brake_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_fuel, float, &telemetry_truck.fuel);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_warning, bool, &telemetry_truck.fuel_warning);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_average_consumption, float, &telemetry_truck.fuel_average_consumption);
    REG_CHANNEL(TRUCK_CHANNEL_fuel_range, float, &telemetry_truck.fuel_range);
    REG_CHANNEL(TRUCK_CHANNEL_adblue, float, &telemetry_truck.adblue);
    REG_CHANNEL(TRUCK_CHANNEL_adblue_warning, bool, &telemetry_truck.adblue_warning);
    REG_CHANNEL(TRUCK_CHANNEL_adblue_average_consumption, float, &telemetry_truck.adblue_average_consumption);
    REG_CHANNEL(TRUCK_CHANNEL_oil_pressure, float, &telemetry_truck.oil_pressure);
    REG_CHANNEL(TRUCK_CHANNEL_oil_pressure_warning, bool, &telemetry_truck.oil_pressure_warning);
    REG_CHANNEL(TRUCK_CHANNEL_oil_temperature, float, &telemetry_truck.oil_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_water_temperature, float, &telemetry_truck.water_temperature);
    REG_CHANNEL(TRUCK_CHANNEL_water_temperature_warning, bool, &telemetry_truck.water_temperature_warning);
    REG_CHANNEL(TRUCK_CHANNEL_battery_voltage, float, &telemetry_truck.battery_voltage);
    REG_CHANNEL(TRUCK_CHANNEL_battery_voltage_warning, bool, &telemetry_truck.battery_voltage_warning);
    REG_CHANNEL(TRUCK_CHANNEL_electric_enabled, bool, &telemetry_truck.electric_enabled);
    REG_CHANNEL(TRUCK_CHANNEL_engine_enabled, bool, &telemetry_truck.engine_enabled);
    REG_CHANNEL(TRUCK_CHANNEL_lblinker, bool, &telemetry_truck.lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_rblinker, bool, &telemetry_truck.rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_lblinker, bool, &telemetry_truck.light_lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_rblinker, bool, &telemetry_truck.light_rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_parking, bool, &telemetry_truck.light_parking);
    REG_CHANNEL(TRUCK_CHANNEL_light_low_beam, bool, &telemetry_truck.light_low_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_high_beam, bool, &telemetry_truck.light_high_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_front, u32, &telemetry_truck.light_aux_front);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_roof, u32, &telemetry_truck.light_aux_roof);
    REG_CHANNEL(TRUCK_CHANNEL_light_beacon, bool, &telemetry_truck.light_beacon);
    REG_CHANNEL(TRUCK_CHANNEL_light_brake, bool, &telemetry_truck.light_brake);
    REG_CHANNEL(TRUCK_CHANNEL_light_reverse, bool, &telemetry_truck.light_reverse);
    REG_CHANNEL(TRUCK_CHANNEL_wipers, bool, &telemetry_truck.wipers);
    REG_CHANNEL(TRUCK_CHANNEL_dashboard_backlight, float, &telemetry_truck.dashboard_backlight);
    REG_CHANNEL(TRUCK_CHANNEL_wear_engine, float, &telemetry_truck.wear_engine);
    REG_CHANNEL(TRUCK_CHANNEL_wear_transmission, float, &telemetry_truck.wear_transmission);
    REG_CHANNEL(TRUCK_CHANNEL_wear_cabin, float, &telemetry_truck.wear_cabin);
    REG_CHANNEL(TRUCK_CHANNEL_wear_chassis, float, &telemetry_truck.wear_chassis);
    REG_CHANNEL(TRUCK_CHANNEL_wear_wheels, float, &telemetry_truck.wear_wheels);
    REG_CHANNEL(TRUCK_CHANNEL_odometer, float, &telemetry_truck.odometer);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_distance, float, &telemetry_truck.navigation_distance);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_time, float, &telemetry_truck.navigation_time);
    REG_CHANNEL(TRUCK_CHANNEL_navigation_speed_limit, float, &telemetry_truck.navigation_speed_limit);

    for(size_t i = 0; i < TELE_TRUCK_WHEEL_COUNT; i++){
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_susp_deflection, i, float, &telemetry_truck.wheel_susp_deflection[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_on_ground, i, bool, &telemetry_truck.wheel_on_ground[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_substance, i, u32, &telemetry_truck.wheel_substance[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_velocity, i, float, &telemetry_truck.wheel_velocity[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_steering, i, float, &telemetry_truck.wheel_steering[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_rotation, i, float, &telemetry_truck.wheel_rotation[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_lift, i, float, &telemetry_truck.wheel_lift[i]);
        REG_INDEXED_CHANNEL(TRUCK_CHANNEL_wheel_lift_offset, i, float, &telemetry_truck.wheel_lift_offset[i]);
    }

    // Trailer channel
    REG_CHANNEL(TRAILER_CHANNEL_connected, bool, &telemetry_trailer.connected);
    REG_CHANNEL(TRAILER_CHANNEL_world_placement, fplacement, &telemetry_trailer.world_placement);
    REG_CHANNEL(TRAILER_CHANNEL_local_linear_velocity, fvector, &telemetry_trailer.local_linear_velocity);
    REG_CHANNEL(TRAILER_CHANNEL_local_angular_velocity, fvector, &telemetry_trailer.local_angular_velocity);
    REG_CHANNEL(TRAILER_CHANNEL_local_linear_acceleration, fvector, &telemetry_trailer.local_linear_acceleration);
    REG_CHANNEL(TRAILER_CHANNEL_local_angular_acceleration, fvector, &telemetry_trailer.local_angular_acceleration);
    REG_CHANNEL(TRAILER_CHANNEL_wear_chassis, float, &telemetry_trailer.wear_chassis);

    for(size_t i = 0; i < TELE_TRAILER_WHEEL_COUNT; i++){
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_susp_deflection, i, float, &telemetry_trailer.wheel_susp_deflection[i]);
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_on_ground, i, bool, &telemetry_trailer.wheel_on_ground[i]);
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_substance, i, u32, &telemetry_trailer.wheel_substance[i]);
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_velocity, i, float, &telemetry_trailer.wheel_velocity[i]);
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_steering, i, float, &telemetry_trailer.wheel_steering[i]);
        REG_INDEXED_CHANNEL(TRAILER_CHANNEL_wheel_rotation, i, float, &telemetry_trailer.wheel_rotation[i]);
    }

    log(SCS_LOG_TYPE_message, "Telemetry UDP " VERSION);
    log(SCS_LOG_TYPE_message, "Common telemetry size: %zi", sizeof(telemetry_common_s));
    log(SCS_LOG_TYPE_message, "Truck telemetry size: %zi", sizeof(telemetry_truck_s));
    log(SCS_LOG_TYPE_message, "Trailer telemetry size: %zi", sizeof(telemetry_trailer_s));
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
