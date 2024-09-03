#include <universal/number/posit/posit.hpp>
#include <universal/number/fixpnt/fixpnt.hpp>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>

// Define the types
using Posit16 = sw::universal::posit<16, 2>;
using Posit32 = sw::universal::posit<32, 2>;
using Fixpnt16 = sw::universal::fixpnt<16, 8>;

using Float = float;
using Double = double;

// Bakhshali square root function
template <typename T>
T bakhshaliSqrt(T S, T initialGuess = T(1.0), T tolerance = T(1e-10), int maxIterations = 1000) {
    T x = initialGuess;
    int iterations = 0;

    while (iterations < maxIterations) {
        T a = (S - x * x) / (T(2.0) * x);
        T b = x + a;
        x = b - (a * a) / (T(2.0) * b);

        if (abs(a) < tolerance) {
            break;
        }

        iterations++;
    }

    return x;
}

// Function to write data to CSV file
void write_to_csv(const std::string& filename, const std::vector<std::vector<std::string>>& data) {
    std::ofstream file(filename);
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

// Function to process range and save results in a CSV file
void process_range(double scale_factor, const std::string& filename) {
    const int bitset_size = 16; // Number of bits to iterate over
    std::cout << std::scientific << std::setprecision(15);

    // Prepare CSV data
    std::vector<std::vector<std::string>> csv_data;
    csv_data.push_back({"Value", "Posit16", "Posit32", "Fixpnt16", "Float", "Double"});

    double total_error_posit16 = 0.0;
    double total_error_posit32 = 0.0;
    double total_error_fixpnt16 = 0.0;
    double total_error_float = 0.0;
    int count = 0;

    for (int i = 0; i < bitset_size; ++i) {
        double double_value = static_cast<double>(std::bitset<bitset_size>(1 << i).to_ulong()) * scale_factor;

        // Ensure the value is non-negative before conversion
        if (double_value < 0) {
            std::cerr << "Negative value encountered: " << double_value << std::endl;
            continue;
        }

        // Convert type
        Posit16 p16(double_value);
        Posit32 p32(double_value);
        Fixpnt16 f16(double_value);
        Float f(double_value);
        Double d(double_value);

        // Square root calculations using Bakhshali method
        Posit16 sqrt_p16;
        Posit32 sqrt_p32;
        Fixpnt16 sqrt_f16;
        Float sqrt_f;
        Double sqrt_d;

        try {
            sqrt_p16 = bakhshaliSqrt(p16);
            sqrt_p32 = bakhshaliSqrt(p32);
            sqrt_f16 = bakhshaliSqrt(f16);
            sqrt_f = bakhshaliSqrt(f);
            sqrt_d = bakhshaliSqrt(d);
        } catch (const std::exception& e) {
            std::cerr << "Error calculating sqrt: " << e.what() << " for value: " << double_value << std::endl;
            continue;
        }

        // Errors
        double error_posit16 = std::abs(static_cast<Double>(sqrt_p16) - sqrt_d);
        double error_posit32 = std::abs(static_cast<Double>(sqrt_p32) - sqrt_d);
        double error_fixpnt16 = std::abs(static_cast<Double>(sqrt_f16) - sqrt_d);
        double error_float = std::abs(static_cast<Double>(sqrt_f) - sqrt_d);

        total_error_posit16 += error_posit16;
        total_error_posit32 += error_posit32;
        total_error_fixpnt16 += error_fixpnt16;
        total_error_float += error_float;
        count++;

        // Save results to CSV
        csv_data.push_back({
            std::to_string(double_value),
            std::to_string(static_cast<Double>(sqrt_p16)),
            std::to_string(static_cast<Double>(sqrt_p32)),
            std::to_string(static_cast<Double>(sqrt_f16)),
            std::to_string(static_cast<Double>(sqrt_f)),
            std::to_string(sqrt_d)
        });
    }

    // Average errors
    double avg_error_posit16 = total_error_posit16 / count;
    double avg_error_posit32 = total_error_posit32 / count;
    double avg_error_fixpnt16 = total_error_fixpnt16 / count;
    double avg_error_float = total_error_float / count;

    // Add average errors to CSV
    csv_data.push_back({
        "Average Error",
        std::to_string(avg_error_posit16),
        std::to_string(avg_error_posit32),
        std::to_string(avg_error_fixpnt16),
        std::to_string(avg_error_float),
        ""
    });

    write_to_csv(filename, csv_data);
}

int main() {
    // Different scale factors for different ranges close to zero
    const std::vector<double> scale_factors = {1.0e-5, 1.0e-6, 1.0e-7, 1.0e-8, 1.0e-9};//1.0e-10
    const std::vector<std::string> filenames = {
        "sqrt_comparison_bakhshali_range1.csv",
        "sqrt_comparison_bakhshali_range2.csv",
        "sqrt_comparison_bakhshali_range3.csv",
        "sqrt_comparison_bakhshali_range4.csv",
        "sqrt_comparison_bakhshali_range5.csv"
    };

    for (size_t i = 0; i < scale_factors.size(); ++i) {
        process_range(scale_factors[i], filenames[i]);
    }

    return 0;
}
