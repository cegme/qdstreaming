#ifndef UTIL_H
#define UTIL_H

#include <cstring>
#include <errno.h>
#include <time.h>

#define MAX(a,b) ( ((a) > (b)) ? (a) : (b) )
#define MIN(a,b) ( ((a) < (b)) ? (a) : (b) )

// This only works for C
static const char *  currentTime () {
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  char * thetime = asctime(timeinfo);
  thetime[strlen(thetime)-1] = '\0';
  return (const char *) thetime;
  //return asctime(timeinfo);
}

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define DATE_STRING currentTime()

#define logInfo(M) std::cerr << DATE_STRING << "[INFO] (" << __FILE__ << ":" << __LINE__ << ") | " << M  << "\n"

#define log_info(M, ...) fprintf(stderr, "%s [INFO] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_trace(M, ...) fprintf(stderr, "%s [trace] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(M, ...) fprintf(stderr, "%s [DEBUG] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_err(M, ...) fprintf(stderr, "%s [ERROR] (%s:%d: errno: %s) | " M "\n", DATE_STRING, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "%s [WARN] (%s:%d: errno: %s) | " M "\n", DATE_STRING, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define check_mem(A) check((A), "Out of memory.")

#define log_timer(tic, toc, M, ...) fprintf(stderr, "%s [INFO] (%s:%d) | " M " (%lf msecs) \n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__, 1000.0*(toc-tic)/CLOCKS_PER_SEC)



#endif  // UTIL_H

