#include "lib/montecarlo.hpp"
#include "lib/primes.hpp"
#include "lib/sys.hpp"
#include "lib/threadpool.hpp"
#include <chrono>
#include <iostream>
#include <vector>

void naive_primes(std::vector<uint32_t> nums) {
    // TODO at the end of each benchmark run we should log memory
    std::cout << "Miller-Rabin sequential...\n";
    // TIME START
    std::chrono::steady_clock::time_point start_time =
        std::chrono::steady_clock::now();

    for (uint32_t n : nums) {
        if (miller_rabin(n, 120000)) {
            std::cout << n << " is PRIME...\n";
        } else {
            std::cout << n << " is COMPOSITE...\n";
        }
    }

    // TIME END
    std::chrono::steady_clock::time_point end_time =
        std::chrono::steady_clock::now();

    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_time - start_time)
                     .count()
              << " ms" << std::endl;
}

void threadpool_primes(std::vector<uint32_t> nums) {
    // declares a threadpool with 4 threads
    ThreadPool *pool = new ThreadPool(4);

    std::vector<std::future<bool>> miller_results;

    std::chrono::steady_clock::time_point start_time =
        std::chrono::steady_clock::now();
    for (auto n : nums) {
        miller_results.emplace_back(
            pool->enqueue([n]() { return miller_rabin(n, 120000); }));
    }

    // Print the results
    std::cout << "\nResults:\n";
    std::cout << "Miller-Rabin with ThreadPool" << std::endl;
    for (size_t i = 0; i < miller_results.size(); i++) {
        bool is_prime = miller_results[i].get();
        std::cout << nums[i] << " is " << (is_prime ? "PRIME" : "COMPOSITE")
                  << "\n";
    }
    delete pool;

    std::chrono::steady_clock::time_point end_time =
        std::chrono::steady_clock::now();

    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_time - start_time)
                     .count()
              << " ms" << std::endl;
}

void threadpool_primes_dispatch(std::vector<uint32_t> nums) {
    std::vector<std::future<bool>> miller_results;

    ThreadPool *pool = new ThreadPool(4);

    std::chrono::steady_clock::time_point start_time =
        std::chrono::steady_clock::now();

    for (auto n : nums) {
        // enqueue the function call to the thread pool using the
        // ThreadDispatch.dispatch() function
        miller_results.emplace_back(
            ThreadDispatch().dispatch(*pool, miller_rabin, n, 120000));
    }

    for (size_t i = 0; i < miller_results.size(); i++) {
        bool is_prime = miller_results[i].get();
        std::cout << nums[i] << " is " << (is_prime ? "PRIME" : "COMPOSITE")
                  << "\n";
    }
    delete pool;

    std::chrono::steady_clock::time_point end_time =
        std::chrono::steady_clock::now();

    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_time - start_time)
                     .count()
              << " ms" << std::endl;
}

int main() {
    std::cout << "Starting benchmark...\n\n";

    System sys;

    // display CPU information and number of running processes from `ps`
    sys.cpu_info();
    sys.proc_info();

    sys.cpu_usage();
    sys.mem_info();
    sys.cpu_temp();
    // TODO get idle temperature at the start of this benchmark to determine
    // the "idle starting" temperature. store in struct?
    // when would it get called?

    std::cout << "Starting with primality testing using the Miller-Rabin "
                 "algorithm...\n";

    std::vector<uint32_t> nums = {
        1000000007, // A large 32-bit integer PRIME
        2147483647, // The largest 32-bit signed integer PRIME
        97,         // A PRIME number
        123456789,  // Another large 32-bit integer
        19,         // A PRIME number
        42,         // Just a random number
        31,         // A PRIME number
        987654321,  // Yet another large 32-bit integer
        37,         // A PRIME number
        123,        // Just another number
        17,         // A PRIME number
        999999999,  // And another large 32-bit integer
        23,         // A PRIME number
        777777777,  // Large 32-bit integer
        13,         // A PRIME number
        234567890,  // Large 32-bit integer
        11,         // A PRIME number
        987654321,  // Repeating value for demonstration
        7,          // A PRIME number
        8675309,    // Another large 32-bit integer
        709,        // A PRIME number
        5381,       // A PRIME number
        52711,      // A PRIME number
        167449,     // A PRIME number
        648391,     // A PRIME number
        1128889,    // A PRIME number
        2269733,    // A PRIME number
        3042161,    // A PRIME number
        4535189,    // A PRIME number
        7474967,    // A PRIME number
        9737333,    // A PRIME number
        14161729,   // A PRIME number
        17624813,   // A PRIME number
        19734581,   // A PRIME number
        23391799,   // A PRIME number
        29499439,   // A PRIME number
        37139213    // A PRIME number
    };

    naive_primes(nums);
    threadpool_primes(nums);

    // TODO where/when should these be called? should these functions update
    // values located within a struct? think about this...
    sys.cpu_usage();
    sys.mem_info();
    sys.cpu_temp();

    threadpool_primes_dispatch(nums);

    sys.cpu_usage();
    sys.mem_info();
    sys.cpu_temp();

    // monte_carlo();

    return 0;
}
