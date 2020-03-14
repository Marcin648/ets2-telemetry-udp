#include <cstdint>

struct __attribute__((packed)) telemetry_fvector_t {
    float x;
    float y;
    float z;
};

struct __attribute__((packed)) telemetry_euler_t {
    scs_float_t heading;
    scs_float_t pitch;
    scs_float_t roll;
};

struct __attribute__((packed)) telemetry_fplacement_t {
    telemetry_fvector_t position;
    telemetry_euler_t orientation;
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
    bool hshifter_selector[4]; // TODO index??
    bool parking_brake;
    bool motor_brake;
    uint32_t retarder_level;
    float brake_air_pressure;
    bool brake_air_pressure_warning;
    bool brake_air_pressure_emergency;
    float brake_temperature;
    float fuel;
    bool fuel_warning;
    float fuel_average_consumption;
    float fuel_range;
    float adblue;
    bool adblue_warning;
    float adblue_average_consumption;
    float oil_pressure;
    bool oil_pressure_warning;
    float oil_temperature;
    float water_temperature;
    bool water_temperature_warning;
    float battery_voltage;
    bool battery_voltage_warning;
    bool electric_enabled;
    bool engine_enabled;
    bool lblinker;
    bool rblinker;
    bool light_lblinker;
    bool light_rblinker;
    bool light_parking;
    bool light_low_beam;
    bool light_high_beam;
    uint32_t light_aux_front;
    uint32_t light_aux_roof;
    bool light_beacon;
    bool light_brake;
    bool light_reverse;
    bool wipers;
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
    bool wheel_on_ground[4]; // TODO index??
    uint32_t wheel_substance[4]; // TODO index??
    float wheel_velocity[4]; // TODO index??
    float wheel_steering[4]; // TODO index??
    float wheel_rotation[4]; // TODO index??
    float wheel_lift[4]; // TODO index??
    float wheel_lift_offset[4]; // TODO index??
};

/*
struct __attribute__((packed)) telemetry_truck_s {
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

};
*/