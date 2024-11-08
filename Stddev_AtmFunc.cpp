#include <iostream>
#include <atomic>
#include <vector>
#include <cmath>
#include <random>


std::atomic<double> runningSum(0.0);
std::atomic<int> count(0);

void atomicAdd(std::atomic<double>& target, double value) {
    double current = target.load();
    while (!target.compare_exchange_weak(current, current + value));
}

double calculateMean(double newValue) {
    atomicAdd(runningSum, newValue);              
    count.fetch_add(1, std::memory_order_relaxed);        
    return runningSum.load() / count.load();            
}

// Function to calculate running standard deviation
double calculateRunningStdDev(const std::vector<double>& prices) {
    double mean = runningSum.load() / count.load();
    double varianceSum = 0.0;
    
    for (double price : prices) {
        varianceSum += (price - mean) * (price - mean);
    }
    
    return std::sqrt(varianceSum / count.load());
}

// Simulate stock prices with random walks
std::vector<double> simulatePriceUpdates(int numUpdates, double initialPrice = 100.0) {
    std::vector<double> prices;
    prices.push_back(initialPrice);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0, 1); // Normal distribution for price changes

    for (int i = 1; i < numUpdates; ++i) {
        // Simulate price as a random walk
        double change = dist(gen) * 0.5; // Adjust 0.5 to increase or decrease volatility
        double newPrice = prices.back() + change;
        prices.push_back(newPrice);
        
        // Update running average and count
        calculateMean(newPrice);
    }

    return prices;
}

int main() {
    int numUpdates = 1000;
    double initialPrice = 100.0;
    
    std::vector<double> prices = simulatePriceUpdates(numUpdates, initialPrice);

    double mean = runningSum.load() / count.load();
    double stddev = calculateRunningStdDev(prices);

    std::cout << "Final Average Price: " << mean << std::endl;
    std::cout << "Final Standard Deviation of Prices: " << stddev << std::endl;

    return 0;
}
