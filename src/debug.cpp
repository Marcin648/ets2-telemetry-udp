#include "debug.hpp"

#include "log.hpp"

void debug_print_config(const scs_telemetry_configuration_t *info){
    #ifndef NDEBUG
    log(SCS_LOG_TYPE_message, "========== ID: %s ==========", info->id);
    for(const scs_named_value_t *current = info->attributes; current->name; current++){
        switch (current->value.type){
        case SCS_VALUE_TYPE_bool:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: bool INDEX: %u VALUE: %c",
                current->name,
                current->index,
                ((current->value.value_bool.value) ? '1' : '0')
            );
            break;

        case SCS_VALUE_TYPE_s32:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: s32 INDEX: %u VALUE: %l",
                current->name,
                current->index,
                current->value.value_s32.value
            );
            break;

        case SCS_VALUE_TYPE_u32:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: u32 INDEX: %u VALUE: %lu",
                current->name,
                current->index,
                current->value.value_u32.value
            );
            break;

        case SCS_VALUE_TYPE_u64:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: u64 INDEX: %u VALUE: %lu",
                current->name,
                current->index,
                current->value.value_u64.value
            );
            break;

        case SCS_VALUE_TYPE_float:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: float INDEX: %u VALUE: %f",
                current->name,
                current->index,
                current->value.value_float.value
            );
            break;

        case SCS_VALUE_TYPE_double:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: double INDEX: %u VALUE: %f",
                current->name,
                current->index,
                current->value.value_double.value
            );
            break;

        case SCS_VALUE_TYPE_fvector:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: fvector INDEX: %u VALUE: %f %f %f",
                current->name,
                current->index,
                current->value.value_fvector.x,
                current->value.value_fvector.y,
                current->value.value_fvector.z
            );
            break;

        case SCS_VALUE_TYPE_dvector:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: dvector INDEX: %u VALUE: %f %f %f",
                current->name,
                current->index,
                current->value.value_dvector.x,
                current->value.value_dvector.y,
                current->value.value_dvector.z
            );
            break;

        case SCS_VALUE_TYPE_euler:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: euler INDEX: %u VALUE: %f %f %f",
                current->name,
                current->index,
                current->value.value_euler.heading,
                current->value.value_euler.pitch,
                current->value.value_euler.roll
            );
            break;

        case SCS_VALUE_TYPE_fplacement:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: fplacement INDEX: %u",
                current->name,
                current->index
            );
            break;

        case SCS_VALUE_TYPE_dplacement:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: dplacement INDEX: %u",
                current->name,
                current->index
            );
            break;

        case SCS_VALUE_TYPE_string:
            log(SCS_LOG_TYPE_message,
                "NAME: %s TYPE: string INDEX: %u VALUE: %s",
                current->name,
                current->index,
                current->value.value_string.value
            );
            break;
        
        default:
            log(SCS_LOG_TYPE_message, "NAME: %s INDEX: %u VALUE: ???? (%d)", current->name, current->index, current->value.type);
            break;
        }
    }
    log(SCS_LOG_TYPE_message, "========== END ID: %s ==========", info->id);
    #endif
}
