#include "log.hpp"

#include <cstdio>
#include <cstdarg>
#include "defines.hpp"

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