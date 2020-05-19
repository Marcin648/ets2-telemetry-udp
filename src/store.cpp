#include "store.hpp"

#include <cstring>
#include <string>
#include "telemetry.hpp"

// STORING FUNCTIONS

SCSAPI_VOID telemetry_store_bool(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint8_t *>(context) = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_s32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<int32_t *>(context) = value->value_s32.value; 
}

SCSAPI_VOID telemetry_store_u32(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint32_t *>(context) = value->value_u32.value; 
}

SCSAPI_VOID telemetry_store_s64(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<int64_t *>(context) = value->value_s64.value; 
}

SCSAPI_VOID telemetry_store_u64(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    *static_cast<uint64_t *>(context) = value->value_u64.value; 
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

SCSAPI_VOID telemetry_store_string(const scs_string_t /*name*/, const scs_u32_t /*index*/, const scs_value_t *const value, const scs_context_t context){
    char *string = static_cast<char *>(context);
    strncpy(string, value->value_string.value, TELE_STR_SIZE-1);
}

// CONFIG STORE


#define STORE_CONFIG_ATTRIBUTE(type, value, target) telemetry_store_##type("", SCS_U32_NIL, value, target)

void config_store_truck(const scs_telemetry_configuration_t *config, telemetry_config_truck_s &telemetry_config_truck){
    memset(&telemetry_config_truck, 0, sizeof(telemetry_config_truck_s));
    for(auto *attr = config->attributes; attr->name; attr++){
        std::string name(attr->name);
        if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.brand_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.brand); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_name){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.name); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_fuel_capacity){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.fuel_capacity); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_fuel_warning_factor){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.fuel_warning_factor); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_adblue_capacity){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.adblue_capacity); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_adblue_warning_factor){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.adblue_warning_factor); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_air_pressure_warning){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.air_pressure_warning); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_air_pressure_emergency){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.air_pressure_emergency); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_oil_pressure_warning){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.oil_pressure_warning); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_water_temperature_warning){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.water_temperature_warning); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_battery_voltage_warning){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.battery_voltage_warning); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_rpm_limit){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_truck.rpm_limit); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_forward_gear_count){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_truck.forward_gear_count); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_reverse_gear_count){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_truck.reverse_gear_count); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_retarder_step_count){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_truck.retarder_step_count); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_cabin_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_truck.cabin_position); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_head_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_truck.head_position); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_hook_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_truck.hook_position); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.license_plate); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.license_plate_country); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_truck.license_plate_country_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_count){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_truck.wheel_count); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_truck.wheel_position[attr->index]); }
    }
}

void config_store_trailer(const scs_telemetry_configuration_t *config, telemetry_config_trailer_s &telemetry_config_trailer){
    memset(&telemetry_config_trailer, 0, sizeof(telemetry_config_trailer_s));
    for(auto *attr = config->attributes; attr->name; attr++){
        std::string name(attr->name);
        if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_accessory_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.cargo_accessory_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_hook_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_trailer.hook_position); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.brand_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.brand); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_name){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.name); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_chain_type){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.chain_type); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_body_type){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.body_type); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.license_plate); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.license_plate_country); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_trailer.license_plate_country_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_count){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_trailer.wheel_count); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_position){ STORE_CONFIG_ATTRIBUTE(fvector, &attr->value, &telemetry_config_trailer.wheel_position[attr->index]); }
    }
}

void config_store_job(const scs_telemetry_configuration_t *config, telemetry_config_job_s &telemetry_config_job){
    memset(&telemetry_config_job, 0, sizeof(telemetry_config_job_s));
    for(auto *attr = config->attributes; attr->name; attr++){
        std::string name(attr->name);
        if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.cargo_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.cargo); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_mass){ STORE_CONFIG_ATTRIBUTE(float, &attr->value, &telemetry_config_job.cargo_mass); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_city_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.destination_city_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_city){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.destination_city); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_city_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.source_city_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_city){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.source_city); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_company_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.destination_company_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_company){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.destination_company); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_company_id){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.source_company_id); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_company){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.source_company); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_income){ STORE_CONFIG_ATTRIBUTE(u64, &attr->value, &telemetry_config_job.income); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_delivery_time){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_job.delivery_time); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_is_cargo_loaded){ STORE_CONFIG_ATTRIBUTE(bool, &attr->value, &telemetry_config_job.is_cargo_loaded); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_job_market){ STORE_CONFIG_ATTRIBUTE(string, &attr->value, &telemetry_config_job.job_market); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_special_job){ STORE_CONFIG_ATTRIBUTE(bool, &attr->value, &telemetry_config_job.special_job); }
        else if(name == SCS_TELEMETRY_CONFIG_ATTRIBUTE_planned_distance_km){ STORE_CONFIG_ATTRIBUTE(u32, &attr->value, &telemetry_config_job.planned_distance_km); }
    }
}