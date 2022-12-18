#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int WINDOW_SIZE = 4096; // size of the analysis and synthesis windows in samples
const int HOP_SIZE = WINDOW_SIZE / 4; // size of the hop between analysis and synthesis windows in samples
const double STRETCH_FACTOR = 0.5; // amount of time stretching to apply

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

  // Initialize a vector to store the stretched samples
  std::vector<int16_t> stretched;

  // Process the samples in overlapping windows
  for (int i = 0; i < samples.size() - WINDOW_SIZE; i += HOP_SIZE) {
    // Extract the current analysis window
    std::vector<int16_t> window(samples.begin() + i, samples.begin() + i + WINDOW_SIZE);

    // Calculate the phase difference between the current and previous analysis windows
    double phase_diff = 0;
    if (i > 0) {
      double prev_magnitude = 0;
      double prev_phase = 0;
      for (int j = 0; j < WINDOW_SIZE; j++) {
        double angle = 2 * M_PI * j / WINDOW_SIZE;
        double real = std::cos(angle) * samples[i - HOP_SIZE + j];
        double imag = std::sin(angle) * samples[i - HOP_SIZE + j];
        prev_magnitude += real * real + imag * imag;
        prev_phase += std::atan2(imag, real);
      }
      prev_magnitude = std::sqrt(prev_magnitude / WINDOW_SIZE);
      prev_phase /= WINDOW_SIZE;

      double magnitude = 0;
      double phase = 0;
      for (int j = 0; j < WINDOW_SIZE; j++) {
        double angle = 2 * M_PI * j / WINDOW_SIZE;
        double real = std::cos(angle) * samples[i + j];
        double imag = std::sin(angle) * samples[i + j];
        magnitude += real * real + imag * imag;
        phase += std::atan2(imag, real);
        }
        magnitude = std::sqrt(magnitude / WINDOW_SIZE);
        phase /= WINDOW_SIZE;

        phase_diff = phase - prev_phase;
        if (phase_diff > M_PI) {
          phase_diff -= 2 * M_PI;
        } else if (phase_diff < -M_PI) {
          phase_diff += 2 * M_PI;
        }
        phase_diff *= STRETCH_FACTOR;
        }

        // Calculate the new phase of each frequency bin
        std::vector<double> new_phase(WINDOW_SIZE);
        for (int j = 0; j < WINDOW_SIZE; j++) {
          double angle = 2 * M_PI * j / WINDOW_SIZE;
          double real = std::cos(angle) * samples[i + j];
          double imag = std::sin(angle) * samples[i + j];
          double magnitude = std::sqrt(real * real + imag * imag);
          double phase = std::atan2(imag, real);
          new_phase[j] = phase + phase_diff * j / WINDOW_SIZE;
        }

// This is where a missing piece might go. wanted synthesis of new window here...

        // Calculate the magnitude of each frequency bin
        double magnitude = 0;
        for (int j = 0; j < WINDOW_SIZE; j++) {
          double angle = 2 * M_PI * j / WINDOW_SIZE;
          double real = std::cos(angle) * samples[i + j];
          double imag = std::sin(angle) * samples[i + j];
          magnitude += real * real + imag * imag;
        }
        magnitude = std::sqrt(magnitude / WINDOW_SIZE);

        // Calculate the new window
        std::vector<int16_t> new_window(WINDOW_SIZE);
        for (int j = 0; j < WINDOW_SIZE; j++) {
          double angle = 2 * M_PI * j / WINDOW_SIZE;
          double real = std::cos(new_phase[j]) * magnitude;
          double imag = std::sin(new_phase[j]) * magnitude;
          new_window[j] = real * std::cos(angle) - imag * std::sin(angle);
        }

        // Add the new window to the stretched audio
        stretched.insert(stretched.end(), new_window.begin(), new_window.end());
        }

        // Write the stretched audio to the output file
        output.write(reinterpret_cast<char*>(stretched.data()), stretched.size() * sizeof(int16_t));
        }

        // Path: myPaulxStretch.h