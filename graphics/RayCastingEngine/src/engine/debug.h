#define DEBUG

#ifdef DEBUG
#define LOG_DEBUG(msg, ...) fprintf(stderr, "DEBUG: " msg "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(msg, ...)
#endif