#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <functional>
#include <map>
#include <scssdk_telemetry.h>
#include <eurotrucks2/scssdk_eut2.h>
#include <eurotrucks2/scssdk_telemetry_eut2.h>
#include <amtrucks/scssdk_ats.h>
#include <amtrucks/scssdk_telemetry_ats.h>

#include <string>

#include "defines.hpp"
#include "log.hpp"
#include "network.hpp"
#include "store.hpp"
#include "register.hpp"
#include "debug.hpp"
#include "telemetry.hpp"

/*
    Telemetry
*/

telemetry_truck_s telemetry_truck;
telemetry_trailer_s telemetry_trailer;
telemetry_common_s telemetry_common;

telemetry_config_truck_s telemetry_config_truck;
telemetry_config_trailer_s telemetry_config_trailer;
telemetry_config_job_s telemetry_config_job;

/*
    SCS API
*/

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
    // if(net_initialized && net_socket != -1){
        /*
        log(SCS_LOG_TYPE_message, "Frame end: %u", rand());
        struct sockaddr_in client_addr;
        int len;
        recvfrom(net_socket, NULL, 0, 0, (sockaddr*)(&client_addr), &len);
        sendto(net_socket, reinterpret_cast<char *>(&telemetry), sizeof(telemetry), 0, (sockaddr*)(&client_addr), len);
        */
        log(SCS_LOG_TYPE_message, "speed: %f km/s", telemetry_truck.speed * 3.6f);
        //log(SCS_LOG_TYPE_message, "rpm: %f", telemetry_truck.engine_rpm);
        //log(SCS_LOG_TYPE_message, "connected: %i", telemetry_trailer.connected);
        log(SCS_LOG_TYPE_message, "game time: %i", telemetry_common.game_time);
        log(SCS_LOG_TYPE_message, "brand name: %s", telemetry_config_truck.brand);

        // sockaddr_in bind_addr;
        // bind_addr.sin_family = AF_INET;
        // bind_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        // bind_addr.sin_port = htons(BIND_PORT);
        // sendto(net_socket, reinterpret_cast<char *>(&telemetry_truck), sizeof(telemetry_truck), 0, (sockaddr*)(&bind_addr), sizeof(bind_addr));
    // }
}

SCSAPI_VOID telemetry_configuration(const scs_event_t /*event*/, const void *const event_info, const scs_context_t /*context*/){
    const scs_telemetry_configuration_t *info = static_cast<const scs_telemetry_configuration_t *>(event_info);
    std::string id(info->id);
    if(id == SCS_TELEMETRY_CONFIG_truck){ config_store_truck(info, telemetry_config_truck); }
    else if(id == SCS_TELEMETRY_CONFIG_trailer) { config_store_trailer(info, telemetry_config_trailer); }
    else if(id == SCS_TELEMETRY_CONFIG_job) { config_store_job(info, telemetry_config_job); }

    debug_print_config(info);

}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t *const params){
    if (version != SCS_TELEMETRY_VERSION_CURRENT) {
		return SCS_RESULT_unsupported;
	}


    const scs_telemetry_init_params_v100_t *const version_params = static_cast<const scs_telemetry_init_params_v100_t *>(params);
    game_log = version_params->common.log;
    register_for_event = version_params->register_for_event;
    register_for_channel = version_params->register_for_channel;
    

    register_event(SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start);
    register_event(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end);
    register_event(SCS_TELEMETRY_EVENT_paused, telemetry_game_pause);
    register_event(SCS_TELEMETRY_EVENT_started, telemetry_game_start);
    register_event(SCS_TELEMETRY_EVENT_configuration, telemetry_configuration);
    
    register_common(telemetry_common);
    register_truck(telemetry_truck);
    register_trailer(telemetry_trailer);

    log(SCS_LOG_TYPE_message, "Telemetry UDP " VERSION);
    log(SCS_LOG_TYPE_message, "Common telemetry size: %zi", sizeof(telemetry_common_s));
    log(SCS_LOG_TYPE_message, "Truck telemetry size: %zi", sizeof(telemetry_truck_s));
    log(SCS_LOG_TYPE_message, "Trailer telemetry size: %zi", sizeof(telemetry_trailer_s));
    // if(!net_initialized){
    //     if(!net_init()){
    //         log(SCS_LOG_TYPE_error, "Failed to initialize network.");
    //         return SCS_RESULT_generic_error;
    //     }
    // }

    return SCS_RESULT_ok;
}

SCSAPI_VOID scs_telemetry_shutdown(void)
{
  net_close();
}
