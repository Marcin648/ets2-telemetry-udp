#pragma once
#include <cstdint>

const uint32_t TELE_TRUCK_WHEEL_COUNT = 10;
const uint32_t TELE_TRAILER_WHEEL_COUNT = 18;
const uint32_t TELE_TRAILER_COUNT = 10;
const uint32_t TELE_STR_SIZE = 64;

enum TELE_PACKET{
    TELE_PACKET_COMMON = 0,
    TELE_PACKET_TRUCK = 1,
    TELE_PACKET_TRAILER = 2,

    TELE_PACKET_CONFIG_TRUCK = 100,
    TELE_PACKET_CONFIG_TRAILER = 101,
    TELE_PACKET_CONFIG_JOB = 102
};

#pragma pack(push, 1)
struct telemetry_fvector_t {
    float x;
    float y;
    float z;
};

struct telemetry_euler_t {
    float heading;
    float pitch;
    float roll;
};

struct telemetry_fplacement_t {
    telemetry_fvector_t position;
    telemetry_euler_t orientation;
};

struct telemetry_common_s {
    uint8_t game_paused;
    float local_scale;
    uint32_t game_time;
    int32_t next_rest_stop;
};

struct telemetry_truck_s {
    telemetry_fplacement_t world_placement;
    telemetry_fvector_t local_linear_velocity;
    telemetry_fvector_t local_angular_velocity;
    telemetry_fvector_t local_linear_acceleration;
    telemetry_fvector_t local_angular_acceleration;
    telemetry_fplacement_t cabin_offset;
    telemetry_fvector_t cabin_angular_velocity;
    telemetry_fvector_t cabin_angular_acceleration;
    telemetry_fplacement_t head_offset;
    float speed;
    float engine_rpm;
    int32_t engine_gear;
    int32_t displayed_gear;
    float input_steering;
    float input_throttle;
    float input_brake;
    float input_clutch;
    float effective_steering;
    float effective_throttle;
    float effective_brake;
    float effective_clutch;
    float cruise_control;
    uint32_t hshifter_slot;
    uint8_t parking_brake;
    uint8_t motor_brake;
    uint32_t retarder_level;
    float brake_air_pressure;
    uint8_t brake_air_pressure_warning;
    uint8_t brake_air_pressure_emergency;
    float brake_temperature;
    float fuel;
    uint8_t fuel_warning;
    float fuel_average_consumption;
    float fuel_range;
    float adblue;
    uint8_t adblue_warning;
    float adblue_average_consumption;
    float oil_pressure;
    uint8_t oil_pressure_warning;
    float oil_temperature;
    float water_temperature;
    uint8_t water_temperature_warning;
    float battery_voltage;
    uint8_t battery_voltage_warning;
    uint8_t electric_enabled;
    uint8_t engine_enabled;
    uint8_t lblinker;
    uint8_t rblinker;
    uint8_t light_lblinker;
    uint8_t light_rblinker;
    uint8_t light_parking;
    uint8_t light_low_beam;
    uint8_t light_high_beam;
    uint32_t light_aux_front;
    uint32_t light_aux_roof;
    uint8_t light_beacon;
    uint8_t light_brake;
    uint8_t light_reverse;
    uint8_t wipers;
    float dashboard_backlight;
    float wear_engine;
    float wear_transmission;
    float wear_cabin;
    float wear_chassis;
    float wear_wheels;
    float odometer;
    float navigation_distance;
    float navigation_time;
    float navigation_speed_limit;
    float wheel_susp_deflection[TELE_TRUCK_WHEEL_COUNT];
    uint8_t wheel_on_ground[TELE_TRUCK_WHEEL_COUNT];
    uint32_t wheel_substance[TELE_TRUCK_WHEEL_COUNT];
    float wheel_velocity[TELE_TRUCK_WHEEL_COUNT];
    float wheel_steering[TELE_TRUCK_WHEEL_COUNT];
    float wheel_rotation[TELE_TRUCK_WHEEL_COUNT];
    float wheel_lift[TELE_TRUCK_WHEEL_COUNT];
    float wheel_lift_offset[TELE_TRUCK_WHEEL_COUNT];
};


struct telemetry_trailer_s{
    uint8_t connected;
    telemetry_fplacement_t world_placement;
    telemetry_fvector_t local_linear_velocity;
    telemetry_fvector_t local_angular_velocity;
    telemetry_fvector_t local_linear_acceleration;
    telemetry_fvector_t local_angular_acceleration;

    float wear_chassis;

    float wheel_susp_deflection[TELE_TRAILER_WHEEL_COUNT];
    uint8_t wheel_on_ground[TELE_TRAILER_WHEEL_COUNT];
    uint32_t wheel_substance[TELE_TRAILER_WHEEL_COUNT];
    float wheel_velocity[TELE_TRAILER_WHEEL_COUNT];
    float wheel_steering[TELE_TRAILER_WHEEL_COUNT];
    float wheel_rotation[TELE_TRAILER_WHEEL_COUNT];
};

struct telemetry_config_truck_s{
    char brand_id[TELE_STR_SIZE];
    char brand[TELE_STR_SIZE];
    char id[TELE_STR_SIZE];
    char name[TELE_STR_SIZE];
    float fuel_capacity;
    float fuel_warning_factor;
    float adblue_capacity;
    float adblue_warning_factor;
    float air_pressure_warning;
    float air_pressure_emergency;
    float oil_pressure_warning;
    float water_temperature_warning;
    float battery_voltage_warning;
    float rpm_limit;
    uint32_t forward_gear_count;
    uint32_t reverse_gear_count;
    uint32_t retarder_step_count;
    telemetry_fvector_t cabin_position;
    telemetry_fvector_t head_position;
    telemetry_fvector_t hook_position;
    char license_plate[TELE_STR_SIZE];
    char license_plate_country[TELE_STR_SIZE];
    char license_plate_country_id[TELE_STR_SIZE];
    uint32_t wheel_count;
    telemetry_fvector_t wheel_position[TELE_TRUCK_WHEEL_COUNT];
};

struct telemetry_config_trailer_s{
    uint8_t index;
    char id[TELE_STR_SIZE];
    char cargo_accessory_id[TELE_STR_SIZE];
    telemetry_fvector_t hook_position;
    char brand_id[TELE_STR_SIZE];
    char brand[TELE_STR_SIZE];
    char name[TELE_STR_SIZE];
    char chain_type[TELE_STR_SIZE];
    char body_type[TELE_STR_SIZE];
    char license_plate[TELE_STR_SIZE];
    char license_plate_country[TELE_STR_SIZE];
    char license_plate_country_id[TELE_STR_SIZE];
    uint32_t wheel_count;
    telemetry_fvector_t wheel_position[TELE_TRAILER_WHEEL_COUNT];
};

struct telemetry_config_job_s{
    char cargo_id[TELE_STR_SIZE];
    char cargo[TELE_STR_SIZE];
    float cargo_mass;
    char destination_city_id[TELE_STR_SIZE];
    char destination_city[TELE_STR_SIZE];
    char source_city_id[TELE_STR_SIZE];
    char source_city[TELE_STR_SIZE];
    char destination_company_id[TELE_STR_SIZE];
    char destination_company[TELE_STR_SIZE];
    char source_company_id[TELE_STR_SIZE];
    char source_company[TELE_STR_SIZE];
    uint64_t income;
    uint32_t delivery_time;
    uint8_t is_cargo_loaded;
    char job_market[TELE_STR_SIZE];
    uint8_t special_job;
    uint32_t planned_distance_km;
};

#pragma pack(pop)
