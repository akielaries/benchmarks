/**
 * A Monte Carlo method is really a problem dealing with random distribution 
 * and random sampling related technique. In this case and within this file
 * we use a "Monte Carlo Method" from what is talked about in the video below
 * specifically when the author talks about predicting the number pi π using
 * random sampling by "dropping marbles" into a square and determining how
 * many are dropped within 1/4 of the square. The number of successes divided
 * by the number of total attempts will our prediction of pi π
 *
 * https://www.youtube.com/watch?v=7ESK5SaP-bc
 */
#include <iostream>
#include <random>
#include "montecarlo.hpp"

int monte_carlo() {
    int total_points = 10000000;
    int successes = 0;
    
    // initialize a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    
    for (int i = 0; i < total_points; i++) {
        // generate random points within the unit square
        double x = dis(gen);
        double y = dis(gen);
        
        // check if the point is inside the unit circle
        if (x * x + y * y <= 1.0) {
            successes++;
        }
    }
    
    // estimate pi
    double predicted_pi = 4.0 * static_cast<double>(successes) / total_points;
    
    std::cout << "Estimated value of pi: " << predicted_pi << std::endl;
    
    return 0;
}

