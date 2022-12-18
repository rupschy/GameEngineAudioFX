#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

const double THRESHOLD = 0.1; // threshold for expanding the signal
const double RATIO = 2.0; // expansion ratio
const double KNEE = 0.5; // knee width
const double MAKEUP_GAIN = 1.0; // makeup gain to apply after expansion

int main() {
  // Open the input and output audio files
  std::ifstream input("input.wav", std::ios::binary);
  std::ofstream output("output.wav", std::ios::binary);

  // Read the input audio file into a vector of samples
  std::vector<int16_t> samples;
  input.seekg(0, std::ios::end);
  int length = input.tellg() / sizeof(int16_t);
  input.seekg(0, std::ios::beg);
  samples.resize(length);
  input.read(reinterpret_cast<char*>(samples.data()), length * sizeof (int16_t));

    // Initialize a vector to store the expanded samples
    std::vector<int16_t> expanded(length);

    // Process the samples with an expansion effect
    for (int i = 0; i < samples.size(); i++) {
        double sample = samples[i] / 32768.0;
        double abs_sample = std::abs(sample);
        double gain = 1.0;
        if (abs_sample > THRESHOLD) {
            double x = (abs_sample - THRESHOLD) / KNEE;
            gain = 1.0 + (RATIO - 1.0) * x / (1.0 + x);
        }
        expanded[i] = static_cast<int16_t>(32768.0 * MAKEUP_GAIN * gain * sample);
    }

    // Write the expanded samples to the output audio file
    output.write(reinterpret_cast<char*>(expanded.data()), expanded.size() * sizeof(int16_t));
    
    return 0;
}

// Path: dimensionExpander.cpp
