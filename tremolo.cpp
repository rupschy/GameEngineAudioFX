// this effect will input an audio signal and output the tremolo effected signal

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double DEPTH = 0.5; // depth of the tremolo effect, between 0 and 1
const double FREQUENCY = 5.0; // frequency of the tremolo effect in Hz

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

    // Process the frame using a tremolo effect
    std::vector<int16_t> processed_frame(HOP_SIZE);
    // Calculate the tremolo depth coefficient
    double depth = 1.0 - DEPTH / 2.0;
    // Calculate the phase increment per sample
    double phase_inc = 2 * M_PI * FREQUENCY / 44100.0;

    // Initialize the phase
    double phase = 0.0;

    // Process the frame
    for (int j = 0; j < HOP_SIZE; j++) {
      // Calculate the tremolo coefficient
      double tremolo = depth + DEPTH * std::sin(phase);
      // Apply the tremolo effect to the sample
      processed_frame[j] = static_cast<int16_t>(tremolo * frame[j]);
      // Increment the phase
      phase += phase_inc;
    }

    // Overlap-add the processed frame with the previous synthesis frame
    if (processed.size() < processed_frame.size()) {
      processed.resize(processed_frame.size());
    }
    for (int j = 0; j < processed_frame.size(); j++) {
      processed[j] += processed_frame[j];
    }
  }

  // Normalize the processed samples and write them to the output file
  int16_t max_sample = *std::max_element(processed.begin(), processed.end());
    for (int i = 0; i < processed.size(); i++) {
        int16_t sample = static_cast<int16_t>(processed[i] * 32767.0 / max_sample);
        output.write(reinterpret_cast<char*>(&sample), sizeof(int16_t));
    }

    return 0;
}


