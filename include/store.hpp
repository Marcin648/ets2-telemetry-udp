#pragma once

#include <scssdk_telemetry.h>
#include <eurotrucks2/scssdk_eut2.h>
#include <eurotrucks2/scssdk_telemetry_eut2.h>
#include <amtrucks/scssdk_ats.h>
#include <amtrucks/scssdk_telemetry_ats.h>
#include "telemetry.hpp"
// STORING FUNCTIONS

SCSAPI_VOID telemetry_store_bool(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_s32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_u32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_s64(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_u64(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_float(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_fvector(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_fplacement(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_dplacement(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);
SCSAPI_VOID telemetry_store_string(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context);

// CONFIG STORE

void config_store_truck(const scs_telemetry_configuration_t *config, telemetry_config_truck_s &telemetry_config_truck);
void config_store_trailer(const scs_telemetry_configuration_t *config, telemetry_config_trailer_s &telemetry_config_trailer);
void config_store_job(const scs_telemetry_configuration_t *config, telemetry_config_job_s &telemetry_config_job);