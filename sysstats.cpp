/**
 * File containing functions to display system information
 * TODO
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// function to read and return the contents of a file
std::string read_file(const std::string& filename) {
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
    FILE* processPipe = popen("ps aux | wc -l", "r");
    if (!processPipe) {
        std::cerr << "popen failed for processes." << std::endl;
        return -1;
    }

    char processBuffer[128];
    if (fgets(processBuffer, sizeof(processBuffer), processPipe) != nullptr) {
        int num_proc = std::stoi(processBuffer);
        pclose(processPipe);
        return num_proc;
    } else {
        std::cerr << "Failed to read the output of 'ps'." << std::endl;
        pclose(processPipe);
        return -1;
    }
}

// function to display CPU usage
void cpu_usage() {
    std::string cpuUsage = read_file("/proc/loadavg");
    std::cout << "CPU Usage (load average): " << cpuUsage;
}

// function to extract CPU BogoMIPS and model name
void cpu_info() {
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    if (cpuInfoFile.is_open()) {
        std::string line;
        std::string model;
        double bogoMIPS = 0.0;

        while (std::getline(cpuInfoFile, line)) {
            if (line.find("BogoMIPS") == 0) {
                // extract BogoMIPS value
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    bogoMIPS = std::stod(line.substr(colonPos + 1));
                }
            } else if (line.find("Model") == 0) {
                // extract model name
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    model = line.substr(colonPos + 1);
                }
            }
        }

        std::cout << "Model: " << model << std::endl;
        std::cout << "BogoMIPS: " << bogoMIPS << std::endl;
    } else {
        std::cerr << "Failed to read /proc/cpuinfo." << std::endl;
    }
}

// function to display memory statistics
void mem_info() {
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        std::vector<std::string> desiredStats = {"MemTotal:", "MemFree:", "MemAvailable:", "Buffers:", "Cached:"};

        while (std::getline(meminfoFile, line)) {
            for (const std::string& stat : desiredStats) {
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
    std::string cpuTemp = read_file("/sys/class/thermal/thermal_zone0/temp");
    if (!cpuTemp.compare(0, 5, "Error")) {
        std::cout << "Error reading CPU temperature." << std::endl;
    } else {
        float temp = std::stof(cpuTemp) / 1000.0; // convert to Celsius
        std::cout << "CPU Temperature: " << temp << "°C" << std::endl;
    }
}

int main() {
    // display CPU model and other stats
    cpu_info();

    // get and display the number of running processes
    int num_proc = proc_info();
    if (num_proc != -1) {
        std::cout << "Number of running processes: " << num_proc << std::endl;
    }

    // display CPU usage, memory statistics, and CPU temperature
    cpu_usage();
    mem_info();
    cpu_temp();

    return 0;
}

