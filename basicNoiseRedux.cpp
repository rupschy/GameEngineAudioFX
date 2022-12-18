#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

const int WINDOW_SIZE = 1024; // size of the sliding window in samples
const double NOISE_THRESHOLD = 0.1; // threshold for identifying noise

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

  // Initialize a vector to store the noise-reduced samples
  std::vector<int16_t> reduced(length);

  // Process the samples in overlapping windows
  for (int i = 0; i < samples.size() - WINDOW_SIZE; i += WINDOW_SIZE / 2) {
    // Calculate the average energy of the window
    double energy = 0;
    for (int j = i; j < i + WINDOW_SIZE; j++) {
      energy += samples[j] * samples[j];
    }
    energy /= WINDOW_SIZE;

    // If the energy is below the noise threshold, process the window as noise
    if (energy < NOISE_THRESHOLD) {
      // Calculate the median of the window
      std::vector<int16_t> window(samples.begin() + i, samples.begin() + i + WINDOW_SIZE);
      std::sort(window.begin(), window.end());
      int median = window[WINDOW_SIZE / 2];

      // Replace the samples in the window with the median value
      for (int j = i; j < i + WINDOW_SIZE; j++) {
        reduced[j] = median;
      }
    } else {
      // Otherwise, copy the samples from the input to the output
      for (int j = i; j < i + WINDOW_SIZE; j++) {
        reduced[j] = samples[j];
      }
    }
  }

  // Write the noise-reduced samples to the output audio file
  output.write(reinterpret_cast<char*>(reduced.data()), reduced.size() * sizeof(int16_t));

  return 0;
}


// This current version assumes wav file and that signal is in mono.