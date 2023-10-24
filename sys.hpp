#ifndef SYSSTATS_HPP
#define SYSSTATS_HPP
#include <string>

/**
 * @brief reads the contents of a file into a string
 *
 * @param filename path to the file to be read
 * @return string containing the contents of the file if successful, or an error
 * message if the file cannot be opened or read
 */
std::string read_file(const std::string &filename);

/**
 * @brief get number of running processes on the system
 *
 * @return number of running processes or error code
 */
int proc_info();

/**
 * @brief get CPU usage using load average read from /proc/loadavg calculating
 * CPU and IO utilization over the last
 *
 * 1 / 5 / 10 min, number of running/total processes, last PID
 *  0.20 0.18 0.12 1/80 11206
 */
void cpu_usage();

/**
 * @brief get CPU info, BogoMIPS and Model
 */
void cpu_info();

/**
 * @brief get memory info and stats from /proc/meminfo
 */
void mem_info();

/**
 * @brief get current CPU temperature from /sys/class/thermal/thermal_zone0/temp
 */
void cpu_temp();

#endif
