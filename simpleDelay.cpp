#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

const int DELAY_TIME = 44100; // delay time in samples
const double FEEDBACK = 0.5; // feedback amount
const double DRY_WET_MIX = 0.5; // mix of dry and wet signals

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

  // Initialize a vector to store the delayed samples
  std::vector<int16_t> delayed(length);

  // Process the samples with a delay effect
  for (int i = 0; i < samples.size(); i++) {
    int delay_index = i - DELAY_TIME;
    if (delay_index < 0) {
      delay_index = 0;
    }
    delayed[i] = static_cast<int16_t>(samples[i] + FEEDBACK * delayed[delay_index]);
  }

  // Mix the dry and wet signals
  for (int i = 0; i < samples.size(); i++) {
    int16_t wet = delayed[i];
    int16_t dry = samples[i];
    int16_t mixed = static_cast<int16_t>(DRY_WET_MIX * wet + (1 - DRY_WET_MIX) * dry);
    samples[i] = mixed;
  }

  // Write the delayed samples to the output audio file
  output.write(reinterpret_cast<char*>(samples.data()), samples.size() * sizeof(int16_t));

  return 0;
}

// Path: simpleDelay.cpp