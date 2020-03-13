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

#define VERSION "v0.1.0"
#define TELEMETRY_VERSION 0
#define LOG_PREFIX "[TELE-UDP] "
#define BIND_PORT 23444

/*
    Telemetry
*/
struct __attribute__((packed)) telemetry_state_t
{
    uint32_t telemetry_version;
    uint8_t game_paused; // 0 - false; 1 - true
    int32_t game_time; // in minute
    int32_t next_rest_stop; // in minute

    uint8_t truck_engine_enabled; // 0 - false; 1 - true
    uint8_t truck_light_parking; // 0 - false; 1 - true
    uint8_t truck_light_low_beam; // 0 - false; 1 - true
    uint8_t truck_light_high_beam; // 0 - false; 1 - true
    uint32_t truck_light_aux_front; // 1 - dimmed state; 2 - full state
    uint32_t truck_light_aux_roof; // 1 - dimmed state; 2 - full state

    uint8_t truck_parking_brake; // 0 - false; 1 - true
    uint8_t truck_lblinker; // 0 - false; 1 - true
    uint8_t truck_rblinker; // 0 - false; 1 - true
    uint8_t truck_light_lblinker; // 0 - false; 1 - true
    uint8_t truck_light_rblinker; // 0 - false; 1 - true

    float truck_engine_rpm;
    float truck_speed; 
    float truck_fuel;

	//scs_value_dplacement_t world_placement;

} telemetry;

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
        #ifdef _WIN32
        closesocket(net_socket);
        #else
        close(net_socket);
        #endif
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
        #ifdef _WIN32
        closesocket(net_socket);
        #else
        close(net_socket);
        #endif
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
    
    log(SCS_LOG_TYPE_message, "Server star on port %h.", BIND_PORT);

    net_initialized = true;
    return true;
}

void net_close(){
    if(net_initialized){
        if(net_socket != -1){
            #ifdef _WIN32
            closesocket(net_socket);
            #else
            close(net_socket);
            #endif
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
/*SCSAPI_VOID telemetry_frame_start(const scs_event_t event, const void *const event_info, const scs_context_t context){

}
*/
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

SCSAPI_VOID telemetry_store_fplacement(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<scs_value_fplacement_t *>(context) = value->value_fplacement; 
}

SCSAPI_VOID telemetry_store_dplacement(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context){
    *static_cast<scs_value_dplacement_t *>(context) = value->value_dplacement; 
}

SCSAPI_VOID telemetry_game_start(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    telemetry.game_paused = 0;
}

SCSAPI_VOID telemetry_game_pause(const scs_event_t /*event*/, const void *const /*event_info*/, const scs_context_t /*context*/){
    telemetry.game_paused = 1;
}

SCSAPI_VOID telemetry_frame_start(const scs_event_t /*event*/, const void *const event_info, const scs_context_t /*context*/){
    if(net_initialized && net_socket != -1){
        log(SCS_LOG_TYPE_message,"Frame start: %u", rand());
    }
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
        sendto(net_socket, reinterpret_cast<char *>(&telemetry), sizeof(telemetry), 0, (sockaddr*)(&bind_addr), sizeof(bind_addr));
    }
}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t *const params){
    if (version != SCS_TELEMETRY_VERSION_CURRENT) {
		return SCS_RESULT_unsupported;
	}


    const scs_telemetry_init_params_v100_t *const version_params = static_cast<const scs_telemetry_init_params_v100_t *>(params);
    game_log = version_params->common.log;

    telemetry.telemetry_version = TELEMETRY_VERSION;

    #define REG_CHANNEL(name, index, type, target) version_params->register_for_channel(SCS_TELEMETRY_##name, index, SCS_VALUE_TYPE_##type, SCS_TELEMETRY_CHANNEL_FLAG_each_frame, telemetry_store_##type, target)
    #define REG_EVENT(type, callback) (version_params->register_for_event(type, callback, NULL))

    REG_EVENT(SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start);
    REG_EVENT(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end);
    REG_EVENT(SCS_TELEMETRY_EVENT_paused, telemetry_game_pause);
    REG_EVENT(SCS_TELEMETRY_EVENT_started, telemetry_game_start);
    

    REG_CHANNEL(CHANNEL_game_time, SCS_U32_NIL, s32, &telemetry.game_time);
    REG_CHANNEL(CHANNEL_next_rest_stop, SCS_U32_NIL, s32, &telemetry.next_rest_stop);
    REG_CHANNEL(TRUCK_CHANNEL_engine_enabled, SCS_U32_NIL, bool, &telemetry.truck_engine_enabled);
    REG_CHANNEL(TRUCK_CHANNEL_light_parking, SCS_U32_NIL, bool, &telemetry.truck_light_parking);
    REG_CHANNEL(TRUCK_CHANNEL_light_low_beam, SCS_U32_NIL, bool, &telemetry.truck_light_low_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_high_beam, SCS_U32_NIL, bool, &telemetry.truck_light_high_beam);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_front, SCS_U32_NIL, u32, &telemetry.truck_light_aux_front);
    REG_CHANNEL(TRUCK_CHANNEL_light_aux_roof, SCS_U32_NIL, u32, &telemetry.truck_light_aux_roof);
    REG_CHANNEL(TRUCK_CHANNEL_parking_brake, SCS_U32_NIL, bool, &telemetry.truck_parking_brake);
    REG_CHANNEL(TRUCK_CHANNEL_lblinker, SCS_U32_NIL, bool, &telemetry.truck_lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_rblinker, SCS_U32_NIL, bool, &telemetry.truck_rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_lblinker, SCS_U32_NIL, bool, &telemetry.truck_light_lblinker);
    REG_CHANNEL(TRUCK_CHANNEL_light_rblinker, SCS_U32_NIL, bool, &telemetry.truck_light_rblinker);
    REG_CHANNEL(TRUCK_CHANNEL_engine_rpm, SCS_U32_NIL, float, &telemetry.truck_engine_rpm);
    REG_CHANNEL(TRUCK_CHANNEL_speed, SCS_U32_NIL, float, &telemetry.truck_speed);
    REG_CHANNEL(TRUCK_CHANNEL_fuel, SCS_U32_NIL, float, &telemetry.truck_fuel);

    log(SCS_LOG_TYPE_message, "Telemetry UDP " VERSION);

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