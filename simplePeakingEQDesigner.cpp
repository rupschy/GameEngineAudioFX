#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double FREQUENCY = 1000.0; // center frequency of the peaking EQ in Hz
const double Q = 1.0; // Q factor of the filter
const double GAIN = 6.0; // gain of the peaking EQ in dB

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
  input.read(reinterpret_cast<char*>(samples.data()), length * sizeof(int16_t));

  // Initialize a vector to store the filtered samples
  std::vector<int16_t> filtered;

  // Process the samples in overlapping frames
  for (int i = 0; i < samples.size() - HOP_SIZE; i += HOP_SIZE) {
    // Extract the current analysis frame
    std::vector<int16_t> frame(samples.begin() + i, samples.begin() + i + HOP_SIZE);

    // Filter the frame using a peaking EQ
    std::vector<int16_t> filtered_frame(HOP_SIZE);
    // Calculate the filter coefficients
    double w0 = 2 * M_PI * FREQUENCY / 44100.0;
    double alpha = std::sin(w0) / (2 * Q);
    double A = std::pow(10.0, GAIN / 40.0);
    double a0 = 1 + alpha / A;
    double b0 = 1 + alpha * A;
    double b1 = -2 * std::cos(w0);
    double b2 = 1 - alpha * A;

    // Initialize the filter state
    int16_t x1 = 0;
    int16_t x2 = 0;
    int16_t y1 = 0;
    int16_t y2 = 0;

    // Filter the frame using a second-order recursive filter
    for (int j = 0; j < HOP_SIZE; j++) {
      int16_t x = frame[j];
      int16_t y = (b0 * x + b1 * x1 + b2 * x2 - a0 * y1 - y2) / a0;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
        filtered_frame[j] = y;
    }

    // Overlap-add the filtered frame with the previous synthesis frame
    if (filtered.size() < filtered_frame.size()) {
      filtered = filtered_frame;
    } else {
      for (int j = 0; j < filtered_frame.size(); j++) {
        filtered[j] += filtered_frame[j];
      }
    }
    }

    // Write the filtered samples to the output audio file
    output.write(reinterpret_cast<char*>(filtered.data()), filtered.size() * sizeof(int16_t));
}

// The code above is a simple peaking EQ filter. It is a second-order recursive filter that is implemented 
// using the Direct Form II transposed structure. The filter coefficients are calculated using the bilinear
// transform. The filter is applied to the input audio file in overlapping frames. The filtered frames are 
// then overlap-added to produce the output audio file.
