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

// the types
using Posit16 = sw::universal::posit<16, 2>;
using Posit32 = sw::universal::posit<32, 2>;
using Fixpnt16 = sw::universal::fixpnt<16, 8>;
using Float = float;
using Double = double;

// Template square root
template <typename T>
T calculate_sqrt(T value) {
    return std::sqrt(value);
}

// Posit types
template <>
Posit16 calculate_sqrt(Posit16 value) {
    return sw::universal::sqrt(value);
}

template <>
Posit32 calculate_sqrt(Posit32 value) {
    return sw::universal::sqrt(value);
}

// Fixpnt16
template <>
Fixpnt16 calculate_sqrt(Fixpnt16 value) {
    return sw::universal::sqrt(value);
}

// Function to print 
template <typename T>
void print_result(const std::string& type_name, T value, T result) {
    std::cout << std::setw(10) << type_name << ": sqrt(" << value << ") = " << result << std::endl;
}

// CSV file
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
    // Define bitset range
    const int bitset_size = 16; // Adjust size as needed
    const double scale_factor = 1e-10; // Small numbers close to zero

    unsigned long start = 1; // Start at 1
    unsigned long end = 5;  // End at 5
    unsigned long step = 1;  // Increment by 1

    std::cout << std::scientific << std::setprecision(15);

    // Prepare CSV 
    std::vector<std::vector<std::string>> csv_data;
    csv_data.push_back({"Value", "Posit16", "Posit32", "Fixpnt16", "Float", "Double"});

    double total_error_posit16 = 0.0;
    double total_error_posit32 = 0.0;
    double total_error_fixpnt16 = 0.0;
    double total_error_float = 0.0;
    int count = 0;

    for (unsigned long value = start; value <= end; value += step) {
        double double_value = value * scale_factor;

        // Convert type
        Posit16 p16(double_value);
        Posit32 p32(double_value);
        Fixpnt16 f16(double_value);
        Float f(double_value);
        Double d(double_value);

        // square roots
        auto sqrt_p16 = calculate_sqrt(p16);
        auto sqrt_p32 = calculate_sqrt(p32);
        auto sqrt_f16 = calculate_sqrt(f16);
        auto sqrt_f = calculate_sqrt(f);
        auto sqrt_d = calculate_sqrt(d);

        // errors
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
            ss.str(), std::to_string(static_cast<Double>(sqrt_p16)),
            std::to_string(static_cast<Double>(sqrt_p32)),
            std::to_string(static_cast<Double>(sqrt_f16)),
            std::to_string(static_cast<Double>(sqrt_f)),
            std::to_string(sqrt_d)
        });
    }

    // average errors
    double avg_error_posit16 = total_error_posit16 / count;
    double avg_error_posit32 = total_error_posit32 / count;
    double avg_error_fixpnt16 = total_error_fixpnt16 / count;
    double avg_error_float = total_error_float / count;

    std::cout << "Average Error Posit16: " << avg_error_posit16 << std::endl;
    std::cout << "Average Error Posit32: " << avg_error_posit32 << std::endl;
    std::cout << "Average Error Fixpnt16: " << avg_error_fixpnt16 << std::endl;
    std::cout << "Average Error Float: " << avg_error_float << std::endl;

    write_to_csv("sqrt_comparison2.csv", csv_data);

    return 0;
}
