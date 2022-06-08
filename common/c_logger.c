#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <c_logger.h>

#if defined(WIN32)

#include <windows.h>

#else

#endif

#if defined(ANDROID)
#include <android/log.h>
#else
#define __android_log_vprint(prio, tag, fmt, ap) vfprintf(stderr, fmt, ap)
#endif

static struct {
    void *udata;
    log_LockFn lock;
    FILE *fp;
    int level;
    int quiet;
#if defined(WIN32)
    HANDLE h;
    CONSOLE_SCREEN_BUFFER_INFO ci;
#endif
} L;


static const char *level_names[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOG_USE_COLOR
#if defined(WIN32)
enum ConsoleColor {
    BLACK = 0,
    DARKBLUE = FOREGROUND_BLUE,
    DARKGREEN = FOREGROUND_GREEN,
    DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
    DARKRED = FOREGROUND_RED,
    DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
    DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    GRAY = FOREGROUND_INTENSITY,
    BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
    MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};
static const WORD level_colors[] = {
        WHITE, GRAY, DARKGREEN, DARKYELLOW, DARKRED, RED
};
#else
static const char *level_colors[] = {
        "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif
#endif


static void lock(void) {
    if (L.lock) {
        L.lock(L.udata, 1);
    }
}


static void unlock(void) {
    if (L.lock) {
        L.lock(L.udata, 0);
    }
}

void log_init(int level, FILE *fp, log_LockFn fn, void *user_data) {
#if defined(WIN32)
    L.h = GetStdHandle(STD_ERROR_HANDLE);
    GetConsoleScreenBufferInfo(L.h, &L.ci);
#endif
    log_set_level(level);
    log_set_fp(fp);
    log_set_lock(fn);
    log_set_udata(user_data);
}

void log_set_udata(void *udata) {
    L.udata = udata;
}


void log_set_lock(log_LockFn fn) {
    L.lock = fn;
}


void log_set_fp(FILE *fp) {
    L.fp = fp;
}


void log_set_level(int level) {
    L.level = level;
}


void log_set_quiet(int enable) {
    L.quiet = enable ? 1 : 0;
}


void log_log(int level, const char *file, int line, const char *fmt, ...) {
    if (level < L.level || level < 0) {
        return;
    }

    /* Acquire lock */
    lock();

    /* Get current time */
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    /* Log to stderr */
    if (!L.quiet) {
        va_list args;
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
#ifdef LOG_USE_COLOR
#if defined(WIN32)

        SetConsoleTextAttribute(L.h, level_colors[level]);
        /*if (level >= LOG_WARN) {
            fprintf(stderr, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
        } else {*/
            fprintf(stderr, "%s %-5s: ", buf, level_names[level]);
        /*}*/
        SetConsoleTextAttribute(L.h, L.ci.wAttributes);
#else
        /*if (level >= LOG_WARN) {
            fprintf(stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                    buf, level_colors[level], level_names[level], file, line);
        } else {*/
            fprintf(stderr, "%s %s%-5s\x1b[0m: ",
                    buf, level_colors[level], level_names[level]);
        /*}*/
#endif
#else
        fprintf(stderr, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
#endif
        va_start(args, fmt);
//        vfprintf(stderr, fmt, args);
        __android_log_vprint(ANDROID_LOG_DEBUG, "c_logger", fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    /* Log to file */
#if defined(ENABLE_FILE_LOGGING)
    if (L.fp) {
        va_list args;
        char buf[64];
        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
        /*if (level >= LOG_WARN) {
            fprintf(L.fp, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
        } else {*/
            fprintf(L.fp, "%s %-5s: ", buf, level_names[level]);
        /*}*/
        va_start(args, fmt);
        vfprintf(L.fp, fmt, args);
        va_end(args);
        fprintf(L.fp, "\n");
        fflush(L.fp);

        if (ftell(L.fp) > LOG_FILE_LIMIT) {
            char filename[sizeof(buf) + 10];

            filename[sprintf(filename, "%s.log", buf)] = '\0';
            L.fp = freopen(filename, "w+", L.fp);
        }
    }
#endif

    /* Release lock */
    unlock();
}