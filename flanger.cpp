#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double DEPTH = 1.0; // depth of the flanger effect, between 0 and 1
const double FREQUENCY = 0.5; // frequency of the flanger effect in Hz
const double DELAY = 5.0; // initial delay of the flanger effect in ms

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

    // Process the frame using a flanger effect
    std::vector<int16_t> processed_frame(HOP_SIZE);
    // Calculate the flanger depth coefficient
    double depth = 1.0 - DEPTH / 2.0;
    // Calculate the phase increment per sample
    double phase_inc = 2 * M_PI * FREQUENCY / 44100.0;
    // Calculate the initial delay in samples
    int delay = static_cast<int>(DELAY * 44100.0 / 1000.0);

    // Initialize the phase
    double phase = 0.0;

    // Process the frame
    for (int j = 0; j < HOP_SIZE; j++) {
      // Calculate the flanger coefficient
      double flanger = depth + DEPTH * std::sin(phase);
      // Calculate the flanger delay in samples
      int flanger_delay = static_cast<int>(flanger * delay);
      // Apply the flanger effect to the sample
      processed_frame[j] = static_cast<int16_t>(frame[j]);
      if (j - flanger_delay >= 0) {
        processed_frame[j] += static_cast<int16_t>(frame[j - flanger_delay]);
      }
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

    // Copy the processed frame to the output vector
    processed.insert(processed.end(), processed_frame.begin(), processed_frame.end());
    }

    // Write the processed samples to the output audio file
    output.write(reinterpret_cast<char*>(processed.data()), processed.size() * sizeof(int16_t));
    }

