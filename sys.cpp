/**
 * File containing functions to display system information
 * TODO
 */
#include "sys.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// function to read and return the contents of a file
std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "Error: Unable to read " + filename;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// function to get the number of running processes
int proc_info() {
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
void cpu_usage() {
    std::string cpu_usage = read_file("/proc/loadavg");
    std::cout << "CPU Usage (load average): " << cpu_usage;
}

// function to extract CPU BogoMIPS and model name
void cpu_info() {
    std::ifstream cpu_info_file("/proc/cpuinfo");
    if (cpu_info_file.is_open()) {
        std::string line;
        std::string model;
        double bogoMIPS = 0.0;

        while (std::getline(cpu_info_file, line)) {
            if (line.find("BogoMIPS") == 0) {
                // extract BogoMIPS value
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    bogoMIPS = std::stod(line.substr(colon_pos + 1));
                }
            } 
            else if (line.find("Model") == 0) {
                // extract model name
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    model = line.substr(colon_pos + 1);
                }
            }
        }
        // TODO: get info from lscpu!!

        std::cout << "Model: " << model << std::endl;
        std::cout << "BogoMIPS: " << bogoMIPS << std::endl;
    } else {
        std::cerr << "Failed to read /proc/cpuinfo." << std::endl;
    }
}

// function to display memory statistics
void mem_info() {
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
void cpu_temp() {
    // TODO initialize starting temp here?
    std::string cpu_temp = read_file("/sys/class/thermal/thermal_zone0/temp");
    if (!cpu_temp.compare(0, 5, "Error")) {
        std::cout << "Error reading CPU temperature." << std::endl;
    } else {
        float temp_c = std::stof(cpu_temp) / 1000.0; // convert to Celsius
        float temp_f = (temp_c * 9 / 5) + 32;       // convert to Fahrenheit
        std::cout << "CPU Temperature: " << temp_c << "°C"
                  << " (" << temp_f << "°F)" << std::endl;
    }
}

// TODO: this function should run for some amount of time for the CPU to 
// literally cooloff before the next run, the base CPU temp should be determined
// at the very start of our benchmark and this function should run for ~30s? and
// get the average temperature to determine the baseline...
// void cpu_idle() {}
