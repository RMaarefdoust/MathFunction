
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <vector>
#include <fstream>
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>
#include <universal/number/bfloat/bfloat.hpp>
#include <universal/number/fixpnt/fixpnt.hpp>

// Template function to compute the absolute value
template<typename Real>
Real fabs_custom(Real value) {
    return std::fabs(value);
}

template<>
sw::universal::posit<16, 2> fabs_custom(sw::universal::posit<16, 2> value) {
    return sw::universal::fabs(value);
}

template<>
sw::universal::posit<32, 2> fabs_custom(sw::universal::posit<32, 2> value) {
    return sw::universal::fabs(value);
}

// Template function to compute the square root
template<typename Real>
Real sqrt_custom(Real value) {
    return std::sqrt(value);
}

template<>
sw::universal::posit<16, 2> sqrt_custom(sw::universal::posit<16, 2> value) {
    return sw::universal::sqrt(value);
}

template<>
sw::universal::posit<32, 2> sqrt_custom(sw::universal::posit<32, 2> value) {
    return sw::universal::sqrt(value);
}

// Template function to compute the square root using Heron's method (original version)
template<typename Real>
Real heronSqrtOriginal(Real number, Real tolerance = 1e-7, int maxIterations = 1000) 
{	
	using ::fabs;
    if (number < 0) {
        std::cerr << "Error: Negative input to sqrt function." << std::endl;
        return -1;
    }

    Real guess = number * 0.5;
    Real difference, nextGuess;
    int iterations = 0;

    do {
        nextGuess = (guess + number / guess) * 0.5;

        difference = fabs(nextGuess - guess);
        guess = nextGuess;
        iterations++;
    } while (difference > tolerance && iterations < maxIterations);

    return nextGuess;
}

// Template function to compute the square root using Heron's method (improved version)
template<typename Real>
Real heronSqrtImproved(Real number, Real tolerance = 1e-7, int maxIterations = 1000) {
    if (number < 0) {
        std::cerr << "Error: Negative input to sqrt function." << std::endl;
        return -1;
    }

    if (number == static_cast<Real>(0)) {
        return 0;
    }

    Real guess = number * 0.5;
    Real difference, nextGuess;
    int iterations = 0;

    do {
        Real guessSquared = guess * guess;
        Real a = (number - guessSquared) / (2.0 * guess);
        Real guessPlusA = guess + a;
        nextGuess = guessPlusA - (a * a) / (2.0 * guessPlusA);
        difference = fabs_custom(nextGuess - guess);
        guess = nextGuess;
        iterations++;
    } while (difference > tolerance * (nextGuess + guess) && iterations < maxIterations);

    return nextGuess;
}

// Template function to run comparisons between the original, improved, and system sqrt functions
template<typename Real>
void runComparisons(const std::string& typeName) {
    std::vector<Real> numbers;
    for (int i = -100; i <= 100; ++i) {
        Real number = std::pow(10.0, i);
        if (number < 1.0)
            numbers.push_back(number);
    }

    std::vector<Real> resultsOriginal;
    std::vector<Real> resultsImproved;
    std::vector<Real> resultsSystem;

    std::cout << std::setw(15) << "Number"
              << std::setw(20) << "Original Result"
              << std::setw(20) << "Improved Result"
              << std::setw(20) << "System Result"
              << std::setw(20) << "Original Time"
              << std::setw(20) << "Improved Time"
              << "\n";

    for (size_t i = 0; i < numbers.size(); i += 5) {
        Real number = numbers[i];

        auto start = std::chrono::high_resolution_clock::now();
        Real sqrtResultOriginal = heronSqrtOriginal(number);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationOriginal = end - start;

        start = std::chrono::high_resolution_clock::now();
        Real sqrtResultImproved = heronSqrtImproved(number);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationImproved = end - start;

        start = std::chrono::high_resolution_clock::now();
        Real sqrtResultSystem = sqrt_custom(number);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> durationSystem = end - start;

        resultsOriginal.push_back(sqrtResultOriginal);
        resultsImproved.push_back(sqrtResultImproved);
        resultsSystem.push_back(sqrtResultSystem);

        std::cout << std::setw(15) << number
                  << std::setw(20) << sqrtResultOriginal
                  << std::setw(20) << sqrtResultImproved
                  << std::setw(20) << sqrtResultSystem
                  << std::setw(20) << durationOriginal.count()
                  << std::setw(20) << durationImproved.count()
                  << "\n";
    }

    std::ofstream outFile("comparison_results_heron_" + typeName + ".csv");
    outFile << "Number,Original Result,Improved Result,System Result\n";
    for (size_t i = 0; i < numbers.size(); ++i) {
        outFile << numbers[i] << ","
                << resultsOriginal[i] << ","
                << resultsImproved[i] << ","
                << resultsSystem[i] << "\n";
    }
    outFile.close();
}

int main() {
    using Posit16 = sw::universal::posit<16, 2>;
    using Posit32 = sw::universal::posit<32, 2>;
    using Bfloat16 = sw::universal::bfloat16;
    using Half = sw::universal::half;
    using Fixpnt16 = sw::universal::fixpnt<16, 8>;
    using Float = float;
    using Double = double;

    std::cout << std::setw(15) << "Posit16\n";
    runComparisons<Posit16>("Posit16");

    std::cout << std::setw(15) << "Posit32\n";
    runComparisons<Posit32>("Posit32");
	
	//std::cout << std::setw(15) << "Fixpnt16\n";
    //runComparisons<Fixpnt16>("Fixpnt16");

    // Comment out bfloat16 and half for now as their fabs and sqrt are not available
    // runComparisons<Bfloat16>("Bfloat16");
    // runComparisons<Half>("Half");
    std::cout << std::setw(15) << "Float\n";
    runComparisons<Float>("Float");
    std::cout << std::setw(15) << "Double\n";
    runComparisons<Double>("Double");

    std::cout << "\nResults written to CSV files\n";

    return 0;
}
