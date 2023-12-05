#include <iostream>
#include <cuda_runtime_api.h>

int _ConvertSMVer2Cores(int major, int minor) {
    // Defines for GPU Architecture types (using the SM version to determine the # of cores per SM)
    struct SMInfo {
        int sm;  // 0xMm (hex), M = SM Major version, and m = SM minor version
        int cores;
    };

    SMInfo gpuArchCoresPerSM[] = {
        {0x10, 8},  // Tesla Generation (SM 1.0) G80 class
        {0x11, 8},  // Tesla Generation (SM 1.1) G8x class
        {0x12, 8},  // Tesla Generation (SM 1.2) G9x class
        {0x13, 8},  // Tesla Generation (SM 1.3) GT200 class
        {0x20, 32}, // Fermi Generation (SM 2.0) GF100 class
        {0x21, 48}, // Fermi Generation (SM 2.1) GF10x class
        {0x30, 192},// Kepler Generation (SM 3.0) GK10x class
        {0x32, 192},// Kepler Generation (SM 3.2) GK20A class
        {0x35, 192},// Kepler Generation (SM 3.5) GK11x class
        {0x37, 192},// Kepler Generation (SM 3.7) GK21x class
        {0x50, 128},// Maxwell Generation (SM 5.0) GM10x class
        {0x52, 128},// Maxwell Generation (SM 5.2) GM20x class
        {0x53, 128},// Maxwell Generation (SM 5.3) GM20x class
        {0x60, 64}, // Pascal Generation (SM 6.0) GP100 class
        {0x61, 128},// Pascal Generation (SM 6.1) GP10x class
        {0x62, 128},// Pascal Generation (SM 6.2) GP10x class
        {0x70, 64}, // Volta Generation (SM 7.0) GV100 class
        {0x72, 64}, // Volta Generation (SM 7.2) GV10B class
        {0x75, 64}, // Turing Generation (SM 7.5) TU10x class
        {0x80, 64}, // Ampere Generation (SM 8.0) GA100 class
        {0x86, 64}, // Ampere Generation (SM 8.6) GA10x class
        // Add future architectures here
        {-1, -1}    // Indicates unknown SM
    };

    int index = 0;
    while (gpuArchCoresPerSM[index].sm != -1) {
        if (gpuArchCoresPerSM[index].sm == ((major << 4) + minor)) {
            return gpuArchCoresPerSM[index].cores;
        }
        index++;
    }

    return -1; // Invalid architecture
}

// Structure to store GPU information
struct GPUInfo {
    std::string name;
    int driverVersion;
    int runtimeVersion;
    int major;
    int minor;
    float totalGlobalMemMB;
    int multiProcessorCount;
    int cudaCores;
    float maxClockRateGHz;
    size_t totalConstMem;
    size_t sharedMemPerBlock;
    int maxThreadsPerMultiProcessor;
    int maxThreadsPerBlock;
    int maxThreadsDim[3];
    int maxGridSize[3];
};

// Function to get GPU information
void getGPUInfo(GPUInfo &gpuInfo) {
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0) {
        std::cerr << "No CUDA-compatible GPU found." << std::endl;
        return;
    }

    // Assuming you want information about the first GPU
    int dev = 0;
    cudaSetDevice(dev);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, dev);

    // Store GPU information in the struct
    gpuInfo.name = deviceProp.name;
    cudaDriverGetVersion(&gpuInfo.driverVersion);
    cudaRuntimeGetVersion(&gpuInfo.runtimeVersion);
    gpuInfo.major = deviceProp.major;
    gpuInfo.minor = deviceProp.minor;
    gpuInfo.totalGlobalMemMB = static_cast<float>(deviceProp.totalGlobalMem / 1048576.0f);
    gpuInfo.multiProcessorCount = deviceProp.multiProcessorCount;
    gpuInfo.cudaCores = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
    gpuInfo.maxClockRateGHz = deviceProp.clockRate * 1e-6f;
    gpuInfo.totalConstMem = deviceProp.totalConstMem;
    gpuInfo.sharedMemPerBlock = deviceProp.sharedMemPerBlock;
    gpuInfo.maxThreadsPerMultiProcessor = deviceProp.maxThreadsPerMultiProcessor;
    gpuInfo.maxThreadsPerBlock = deviceProp.maxThreadsPerBlock;
    gpuInfo.maxThreadsDim[0] = deviceProp.maxThreadsDim[0];
    gpuInfo.maxThreadsDim[1] = deviceProp.maxThreadsDim[1];
    gpuInfo.maxThreadsDim[2] = deviceProp.maxThreadsDim[2];
    gpuInfo.maxGridSize[0] = deviceProp.maxGridSize[0];
    gpuInfo.maxGridSize[1] = deviceProp.maxGridSize[1];
    gpuInfo.maxGridSize[2] = deviceProp.maxGridSize[2];
}

// Function to print GPU information
void printGPUInfo(const GPUInfo &gpuInfo) {
    std::cout << "\nDevice: \"" << gpuInfo.name << "\"" << std::endl;

    std::cout << "  CUDA Driver Version / Runtime Version          "
              << gpuInfo.driverVersion / 1000 << "." << (gpuInfo.driverVersion % 100) / 10
              << " / " << gpuInfo.runtimeVersion / 1000 << "."
              << (gpuInfo.runtimeVersion % 100) / 10 << std::endl;

    std::cout << "  CUDA Capability Major/Minor version number:    "
              << gpuInfo.major << "." << gpuInfo.minor << std::endl;

    std::cout << "  Total amount of global memory:                 "
              << gpuInfo.totalGlobalMemMB << " MBytes" << std::endl;

    std::cout << "  ( " << gpuInfo.multiProcessorCount << ") Multiprocessors, ("
              << gpuInfo.multiProcessorCount * gpuInfo.cudaCores
              << ") CUDA Cores/MP:     " << gpuInfo.cudaCores
              << " CUDA Cores" << std::endl;

    std::cout << "  GPU Max Clock rate:                            "
              << gpuInfo.maxClockRateGHz << " GHz" << std::endl;

    std::cout << "  Total amount of constant memory:               "
              << gpuInfo.totalConstMem << " bytes" << std::endl;

    std::cout << "  Total amount of shared memory per block:       "
              << gpuInfo.sharedMemPerBlock << " bytes" << std::endl;

    std::cout << "  Maximum number of threads per multiprocessor:  "
              << gpuInfo.maxThreadsPerMultiProcessor << std::endl;

    std::cout << "  Maximum number of threads per block:           "
              << gpuInfo.maxThreadsPerBlock << std::endl;

    std::cout << "  Max dimension size of a thread block (x,y,z): ("
              << gpuInfo.maxThreadsDim[0] << ", " << gpuInfo.maxThreadsDim[1] << ", "
              << gpuInfo.maxThreadsDim[2] << ")" << std::endl;

    std::cout << "  Max dimension size of a grid size    (x,y,z): ("
              << gpuInfo.maxGridSize[0] << ", " << gpuInfo.maxGridSize[1] << ", "
              << gpuInfo.maxGridSize[2] << ")" << std::endl;
}

int main() {
    GPUInfo gpuInfo;
    getGPUInfo(gpuInfo);
    printGPUInfo(gpuInfo);
    return 0;
}

