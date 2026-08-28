#include "common_utils.h"
#include "string.h"
#include "stdio.h"

unsigned long get_tick_count()
{
#ifdef _WIN32
    return GetTickCount();
#elif __linux__
    struct timespec ts;
    unsigned long theTick = 0U;
    clock_gettime(CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#else
#error "Unsupported platform"
#endif
}

ERROR_CODE_TYPE get_last_error() {
#ifdef _WIN32
    return GetLastError();
#elif __linux__
    return errno;
#else
#error "Unsupported platform"
#endif
}

int current_thread_sleep(unsigned int millis) {
#ifdef _WIN32
    Sleep(millis);
    return 0;
#elif __linux__
    return usleep(millis * 1000);
#else
#error "Unsupported platform"
#endif
}

bool parse_cmd(int argc, char* argv[], PCOMMAND_OPTIONS p_options)
{
    if (argc < 2) {
        return false;
    }

    char all_args[256];
    memset(all_args, 0, sizeof(all_args));

    for (int i = 1; i < argc; ++i) {
        strcat(all_args, argv[i]);
        strcat(all_args, " ");
    }


    struct FormatOptions {
        char format[16];
        int require_cn;
        int order;
    };

    FormatOptions format_opts[] = {
            {"-h %s -p %d", 2, 0},
            {"-p %d -h %s", 2, 1},
            {"-h %s:%d", 2, 0},
            {"-p %d", 1, 0}

    };

    const int opt_counts = sizeof(format_opts)/sizeof(FormatOptions);

    int ret = 0;
    int done = -1;
    for (int i = 0; done != 0 && i < opt_counts; ++i) {
        FormatOptions format_opt = format_opts[i];
        memset(p_options, 0, sizeof(COMMAND_OPTIONS));

        if (format_opt.require_cn == 1) {
            ret = sscanf(all_args, format_opt.format, &(p_options->port));
        }
        else {
            if (format_opt.order == 1) {
                ret = sscanf(all_args, format_opt.format, &(p_options->port), p_options->host);
            }
            else if (format_opt.order == 0) {
                ret = sscanf(all_args, format_opt.format, p_options->host, &(p_options->port));
            }
        }

        done = ret - format_opt.require_cn;
    }

    return done == 0;

}

bool valid_connection_opts(PCOMMAND_OPTIONS p_opts) {
    return p_opts && p_opts->port > 0 && strlen(p_opts->host) > 0;
}

void common_usage(const char* str) {
    printf("Usage of %s: \n", str);
    printf("\t\t: -h <host> -p <port>\n");
    printf("\t\t: -p <port> -h <host> \n");
    printf("\t\t: -h <host>:<port> \n");
    printf("\t\t: -p <port> \n");
}

void get_mouse_pos(PMOUSE_POSITION p_mouse_pos) {
#ifdef _WIN32
    if (!p_mouse_pos) {
        return;
    }

    POINT point;
    GetCursorPos(&point);
    p_mouse_pos->x = point.x;
    p_mouse_pos->y = point.y;
#elif __linux__
#endif
}

void set_mouse_pos(PMOUSE_POSITION p_mouse_pos) {
#ifdef _WIN32
    if (!p_mouse_pos) {
        return;
    }
    SetCursorPos(p_mouse_pos->x, p_mouse_pos->y);
#elif __linux__
#endif
}
