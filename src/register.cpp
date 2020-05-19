#include "register.hpp"

#include <cstdio>
#include "store.hpp"

#define REG_EVENT(type, callback) (register_for_event(type, callback, NULL))
#define REG_INDEXED_CHANNEL(name, index, type, target) register_for_channel(SCS_TELEMETRY_##name, index, SCS_VALUE_TYPE_##type, SCS_TELEMETRY_CHANNEL_FLAG_each_frame, telemetry_store_##type, target)
#define REG_CHANNEL(name, type, target) REG_INDEXED_CHANNEL(name, SCS_U32_NIL, type, target)

scs_telemetry_register_for_channel_t register_for_channel;
scs_telemetry_register_for_event_t register_for_event;

void register_event(const scs_event_t type, scs_telemetry_event_callback_t callback){
    REG_EVENT(type, callback);
}

// Common channel
void register_common(telemetry_common_s &telemetry_common){
    REG_CHANNEL(CHANNEL_local_scale, float, &telemetry_common.local_scale);
    REG_CHANNEL(CHANNEL_game_time, u32, &telemetry_common.game_time);
    REG_CHANNEL(CHANNEL_next_rest_stop, s32, &telemetry_common.next_rest_stop);
}

// Truck channel
void register_truck(telemetry_truck_s &telemetry_truck){
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
}

// Trailer channel
void register_trailer(telemetry_trailer_s &telemetry_trailer){
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
}