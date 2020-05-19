#pragma once

#include <scssdk_telemetry.h>
#include <eurotrucks2/scssdk_eut2.h>
#include <eurotrucks2/scssdk_telemetry_eut2.h>
#include <amtrucks/scssdk_ats.h>
#include <amtrucks/scssdk_telemetry_ats.h>

#include "telemetry.hpp"

extern scs_telemetry_register_for_channel_t register_for_channel;
extern scs_telemetry_register_for_event_t register_for_event;


void register_event(const scs_event_t type, scs_telemetry_event_callback_t callback);

void register_common(telemetry_common_s &telemetry_common);
void register_truck(telemetry_truck_s &telemetry_truck);
void register_trailer(telemetry_trailer_s &telemetry_trailer);
