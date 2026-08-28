#ifndef NETWORK_PROGRAMMING_COMMON_UTILS_H
#define NETWORK_PROGRAMMING_COMMON_UTILS_H

typedef struct MousePosition {
    int x;
    int y;
} MOUSE_POSITION, *PMOUSE_POSITION;

#ifdef _WIN32

#include <windows.h>

typedef DWORD ERROR_CODE_TYPE;

#ifndef ERROR_TIMEOUT
 #define ERROR_TIMEOUT WSAETIMEDOUT
#endif

#elif __linux__

#include <errno.h>
#include <time.h>
#include <unistd.h>

typedef int ERROR_CODE_TYPE;

#define ERROR_TIMEOUT ETIMEDOUT

#else
#error "Unsupported platform"
#endif

unsigned long get_tick_count();

ERROR_CODE_TYPE get_last_error();

int current_thread_sleep(unsigned int millis);

#ifdef _MSC_VER

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#endif // _MSC_VER

typedef struct CommandOptions {
    char  host[256];
    short port;
} COMMAND_OPTIONS, *PCOMMAND_OPTIONS;

bool parse_cmd(int argc, char* argv[], PCOMMAND_OPTIONS);

bool valid_connection_opts(PCOMMAND_OPTIONS);

void common_usage(const char*);

void get_mouse_pos(PMOUSE_POSITION);
void set_mouse_pos(PMOUSE_POSITION);

#endif //NETWORK_PROGRAMMING_COMMON_UTILS_H
