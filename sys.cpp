/**
 * File containing functions to display system information
 * TODO: figure out logic behind collecting information (to variables) and
 * dumping them en masse to a file. this should be done logically with some
 * sort of interval / condition on collecting data to log i.e.:
 *  - before benchmark test
 *  - during test (how often?, pause/resume timers before/after check_mem call?)
 *  - after test
 */
#include "lib/sys.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include "sys/types.h"
#include "sys/sysinfo.h"


// function to read and return the contents of a file
std::string System::read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "Error: Unable to read " + filename;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// function to get the number of running processes
int System::proc_info() {
    FILE *proc_pipe = popen("ps aux | wc -l", "r");
    if (!proc_pipe) {
        std::cerr << "popen failed for processes." << std::endl;
        return -1;
    }

    char proc_buffer[128];
    if (fgets(proc_buffer, sizeof(proc_buffer), proc_pipe) != nullptr) {
        int num_proc = std::stoi(proc_buffer);
        pclose(proc_pipe);
        std::cout << "Number of running processes: " << num_proc << std::endl;
        return num_proc;
    } else {
        std::cerr << "Failed to read the output of 'ps'." << std::endl;
        pclose(proc_pipe);
        return -1;
    }
}

// function to display CPU usage
void System::cpu_usage() {
    std::string cpu_usage = read_file("/proc/loadavg");
    std::cout << "CPU Usage (load average): " << cpu_usage;
}

// function to extract CPU BogoMIPS and model name
void System::cpu_info() {
    std::string lscpu_output;
    std::string model;
    double bogoMIPS = 0.0;
    int numCPUs = 0;

    // run the lscpu command and capture its output
    FILE *pipe = popen("lscpu", "r");
    if (pipe) {
        char buffer[128];
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != nullptr) {
                lscpu_output += buffer;
            }
        }
        pclose(pipe);

        // parse lscpu output to extract information
        size_t pos = 0;
        while (pos < lscpu_output.size()) {
            size_t newline_pos = lscpu_output.find('\n', pos);
            if (newline_pos == std::string::npos) {
                newline_pos = lscpu_output.size();
            }
            std::string line = lscpu_output.substr(pos, newline_pos - pos);

            // convert the line to lowercase for case-insensitive matching
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);

            if (line.find("model name") != std::string::npos) {
                // extract model name
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    model = line.substr(colon_pos + 1);
                    model = model.substr(model.find_first_not_of(" \t"),
                                         model.find_last_not_of(" \t") + 1);
                }
            } else if (line.find("bogomips") != std::string::npos) {
                // extract BogoMIPS value
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    bogoMIPS = std::stod(line.substr(colon_pos + 1));
                }
            } else if (line.find("cpu(s):") != std::string::npos) {
                // extract the number of CPU(s)
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    numCPUs = std::stoi(line.substr(colon_pos + 1));
                }
            }

            pos = newline_pos + 1;
        }

        // print the extracted information
        std::cout << "Model: " << model << std::endl;
        std::cout << "BogoMIPS: " << bogoMIPS << std::endl;
        std::cout << "Number of CPU(s): " << numCPUs << std::endl;
    } else {
        std::cerr << "Failed to run lscpu command." << std::endl;
    }
}
// function to display memory statistics
void System::mem_info() {
    // TODO: save the memory info to variables for a mass dump into file with
    // timestamp as KEY
    std::ifstream meminfo_file("/proc/meminfo");
    if (meminfo_file.is_open()) {
        std::string line;
        std::vector<std::string> fetched_stats =
            {"MemTotal:", "MemFree:", "MemAvailable:", "Buffers:", "Cached:"};

        while (std::getline(meminfo_file, line)) {
            for (const std::string &stat : fetched_stats) {
                if (line.find(stat) == 0) {
                    std::cout << line << std::endl;
                }
            }
        }
    } else {
        std::cerr << "Failed to read /proc/meminfo." << std::endl;
    }
}

// function to display CPU temperature
double System::cpu_temp() {
    // TODO initialize starting temp here?
    std::string cpu_temp = read_file("/sys/class/thermal/thermal_zone0/temp");
    if (!cpu_temp.compare(0, 5, "Error")) {
        std::cout << "Error reading CPU temperature." << std::endl;
    } else {
        float temp_c = std::stof(cpu_temp) / 1000.0; // convert to Celsius
        float temp_f = (temp_c * 9 / 5) + 32;        // convert to Fahrenheit
        // std::cout << "CPU Temperature: " << temp_c << "°C"
        //          << " (" << temp_f << "°F)" << std::endl;
        return temp_c;
    }
    return -1;
}

// TODO: this function should run for some amount of time for the CPU to
// literally cooloff before the next run, the base CPU temp should be determined
// at the very start of our benchmark and this function should run for ~30s? and
// get the average temperature to determine the baseline...
double System::cpu_idle_temp() {
    std::cout << "Determining idle CPU temperature...\n";

    int duration = 20;

    std::vector<double> temperatures;
    auto time_start = std::chrono::steady_clock::now();
    while (true) {
        auto time_now = std::chrono::steady_clock::now();
        auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                                time_now - time_start)
                                .count();
        if (time_elapsed >= duration) {
            break;
        }
        double temperature = cpu_temp();
        temperatures.push_back(temperature);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    double sum = 0.0;
    for (const double &temp : temperatures) {
        sum += temp;
    }
    double idle_temp = sum / temperatures.size();

    // set class var
    System::cpu_temp_idle = idle_temp;

    return idle_temp;
}

void System::cpu_idle(double idle_temp) {
    while (true) {
        double temp_now = cpu_temp();

        if (temp_now <= idle_temp) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void mem_stats() {
    struct sysinfo memInfo;

    sysinfo(&memInfo);
    
    long long totalVirtualMem = (memInfo.totalram + memInfo.totalswap) * memInfo.mem_unit;
    long long virtualMemUsed = (memInfo.totalram - memInfo.freeram + memInfo.totalswap - memInfo.freeswap) * memInfo.mem_unit;
    
    long long totalPhysMem = memInfo.totalram * memInfo.mem_unit;
    long long physMemUsed = (memInfo.totalram - memInfo.freeram) * memInfo.mem_unit;

    std::cout << "VIRTUAL MEM AVAIL: " << totalVirtualMem / 1000 << " KB" << std::endl;
    std::cout << "VIRTUAL MEM USED: " << virtualMemUsed / 1000 << " KB" << std::endl;

    std::cout << "PHYSICAL MEM AVAIL: " << totalPhysMem / 1000 << " KB" << std::endl;
    std::cout << "PHYSICAL MEM USED: " << physMemUsed / 1000 << " KB" << std::endl;
    std::cout << "PHYSICAL MEM FREE: " << (totalPhysMem - physMemUsed) / 1000 << " KB" << std::endl;
}

double cpu_stats() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return -1.0;
    }

    std::string line;
    std::getline(file, line);

    std::istringstream iss(line);
    std::string cpuLabel;
    iss >> cpuLabel;

    if (cpuLabel != "cpu") {
        std::cerr << "Failed to parse /proc/stat" << std::endl;
        return -1.0;
    }

    std::vector<long long> values;
    long long value;
    while (iss >> value) {
        values.push_back(value);
    }

    if (values.size() < 7) {
        std::cerr << "Failed to parse /proc/stat" << std::endl;
        return -1.0;
    }

    long long idle = values[3];
    long long total = 0;
    for (int i = 0; i < 7; i++) {
        total += values[i];
    }

    // Calculate CPU usage as a percentage
    double usage = 100.0 * (1.0 - static_cast<double>(idle) / total);

    std::cout << "CPU USAGE: " << usage << std::endl;

    return usage;
}

int main() {

    mem_stats();
    cpu_stats();
    return 0;
}
