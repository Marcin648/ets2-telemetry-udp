#pragma once
#include <cstdint>

struct __attribute__((packed)) telemetry_fvector_t {
    float x;
    float y;
    float z;
};

struct __attribute__((packed)) telemetry_euler_t {
    float heading;
    float pitch;
    float roll;
};

struct __attribute__((packed)) telemetry_fplacement_t {
    telemetry_fvector_t position;
    telemetry_euler_t orientation;
};

struct __attribute__((packed)) telemetry_common_s {
    uint8_t game_paused;
    float local_scale;
    uint32_t game_time;
    int32_t next_rest_stop;
};

struct __attribute__((packed)) telemetry_truck_s {
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
    uint8_t hshifter_selector[4]; // TODO index??
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
    float wheel_susp_deflection[4]; // TODO index??
    uint8_t wheel_on_ground[4]; // TODO index??
    uint32_t wheel_substance[4]; // TODO index??
    float wheel_velocity[4]; // TODO index??
    float wheel_steering[4]; // TODO index??
    float wheel_rotation[4]; // TODO index??
    float wheel_lift[4]; // TODO index??
    float wheel_lift_offset[4]; // TODO index??
};


struct __attribute__((packed)) telemetry_trailer_s{
    uint8_t connected;
    telemetry_fplacement_t world_placement;
    telemetry_fvector_t local_linear_velocity;
    telemetry_fvector_t local_angular_velocity;
    telemetry_fvector_t local_linear_acceleration;
    telemetry_fvector_t local_angular_acceleration;

    float wear_chassis;

    float wheel_susp_deflection[4]; // TODO index??
    uint8_t wheel_on_ground[4]; // TODO index??
    uint32_t wheel_substance[4]; // TODO index??
    float wheel_velocity[4]; // TODO index??
    float wheel_steering[4]; // TODO index??
    float wheel_rotation[4]; // TODO index??
};