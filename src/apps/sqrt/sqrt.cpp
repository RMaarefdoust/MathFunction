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

// Template square root calculation
template <typename T>
T calculate_sqrt(T value) {
    if constexpr (std::is_same_v<T, Posit16> || std::is_same_v<T, Posit32>) {
        return sqrt(value);
    } else if constexpr (std::is_same_v<T, Fixpnt16>) {
        if (value < 0) {
            throw sw::universal::fixpnt_arithmetic_exception("argument to sqrt is negative");
        }
        return sw::universal::sqrt(value);
    } else {
        return sqrt(value);
    }
}

// Function to print results
template <typename T>
void print_result(const std::string& type_name, T value, T result) {
    std::cout << std::setw(10) << type_name << ": sqrt(" << value << ") = " << result << std::endl;
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

int main() {
    const double scale_factor = 1e-5; // Small numbers close to zero
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

        // Debugging: Print the converted values
        std::cout << "Debug: Converted values" << std::endl;
        std::cout << "Double: " << d << ", Posit16: " << p16 << ", Posit32: " << p32 << ", Fixpnt16: " << f16 << ", Float: " << f << std::endl;

        // Square root calculations
        Posit16 sqrt_p16;
        Posit32 sqrt_p32;
        Fixpnt16 sqrt_f16;
        Float sqrt_f;
        Double sqrt_d;

        try {
            sqrt_p16 = calculate_sqrt(p16);
            sqrt_p32 = calculate_sqrt(p32);
            if (f16 >= 0) { // Ensure non-negative value for Fixpnt16
                sqrt_f16 = calculate_sqrt(f16);
            } else {
                throw std::runtime_error("Fixpnt16 value is negative");
            }
            sqrt_f = calculate_sqrt(f);
            sqrt_d = calculate_sqrt(d);
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

        std::cout << "Value: " << double_value << std::endl;
        print_result("Posit16", p16, sqrt_p16);
        print_result("Posit32", p32, sqrt_p32);
        print_result("Fixpnt16", f16, sqrt_f16);
        print_result("Float", f, sqrt_f);
        print_result("Double", d, sqrt_d);
        std::cout << "----------------------------------------" << std::endl;

        // Save results to CSV
        std::stringstream ss;
        ss << std::scientific << std::setprecision(15);
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

    std::cout << "Average Error Posit16: " << avg_error_posit16 << std::endl;
    std::cout << "Average Error Posit32: " << avg_error_posit32 << std::endl;
    std::cout << "Average Error Fixpnt16: " << avg_error_fixpnt16 << std::endl;
    std::cout << "Average Error Float: " << avg_error_float << std::endl;

    write_to_csv("sqrt_comparison5.csv", csv_data);

    return 0;
}
