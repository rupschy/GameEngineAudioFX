#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double CUTOFF = 1000.0; // center frequency of the notch in Hz
const double Q = 0.707; // Q factor of the filter

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

    // Filter the frame using a notch filter
    std::vector<int16_t> filtered_frame(HOP_SIZE);
    // Calculate the filter coefficients
    double w0 = 2 * M_PI * CUTOFF / 44100.0;
    double alpha = std::sin(w0) / (2 * Q);
    double a0 = 1 + alpha;
    double b0 = 1;
    double b1 = -2 * std::cos(w0);
    double b2 = 1;

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
      filtered.resize(filtered_frame.size());
    }
    for (int j = 0; j < filtered_frame.size(); j++) {
      filtered[j] += filtered_frame[j];
    }

    // Add the overlapped samples to the output vector
    for (int j = 0; j < HOP_SIZE; j++) {
      int16_t sample = filtered[j];
      filtered[j] = 0;
      filtered[j + HOP_SIZE] = sample;
      if (j + HOP_SIZE < filtered.size()) {
        filtered[j + HOP_SIZE] = 0;
      }
      samples.push_back(sample);
    }
    }

    // Write the filtered samples to the output audio file
    output.write(reinterpret_cast<char*>(samples.data()), samples.size() * sizeof(int16_t));
    }


