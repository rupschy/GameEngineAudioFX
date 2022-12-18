#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double THRESHOLD = -10.0; // threshold of the gain expander in dB
const double RATIO = 2.0; // ratio of the gain expander
const double KNEE = 0.0; // knee of the gain expander in dB

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

  // Initialize a vector to store the processed samples
  std::vector<int16_t> processed;

  // Process the samples in overlapping frames
  for (int i = 0; i < samples.size() - HOP_SIZE; i += HOP_SIZE) {
    // Extract the current analysis frame
    std::vector<int16_t> frame(samples.begin() + i, samples.begin() + i + HOP_SIZE);

    // Process the frame using a gain expander effect
    std::vector<int16_t> processed_frame(HOP_SIZE);
    // Calculate the threshold and knee coefficients
    double threshold = std::pow(10.0, THRESHOLD / 20.0);
    double knee = std::pow(10.0, KNEE / 20.0);

    // Process the frame
    for (int j = 0; j < HOP_SIZE; j++) {
      // Calculate the input and output gain
      double input_gain = std::abs(frame[j]) / 32768.0;
      double output_gain = input_gain;
      // Apply the gain expander effect
      if (input_gain < threshold) {
        output_gain = threshold + (input_gain - threshold) * RATIO;
      }
      // Soften the knee if necessary
      if (KNEE > 0.0 && input_gain < knee) {
        double slope = RATIO - 1.0;
        double soft_knee = (input_gain - threshold) / slope;
        output_gain = threshold + soft_knee;
        }
        // Apply the gain expander to the sample
        processed_frame[j] = static_cast<int16_t>(output_gain * 32768.0);
        }

        // Overlap-add the processed frame with the previous synthesis frame
        if (processed.size() < processed_frame.size()) {
          processed.resize(processed_frame.size());
        }
        for (int j = 0; j < processed_frame.size(); j++) {
          processed[j] += processed_frame[j];
        }
        }

        // Write the processed samples to the output audio file
        output.write(reinterpret_cast<char*>(processed.data()), processed.size() * sizeof(int16_t));

        return 0;
        }

        // Path: gainExpander.cpp
        
       
