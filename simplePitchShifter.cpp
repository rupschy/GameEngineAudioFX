#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const int HOP_SIZE = 512; // size of the hop between analysis and synthesis frames in samples
const double PITCH_SHIFT = 2.0; // amount of pitch shifting to apply

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

  // Initialize a vector to store the pitch shifted samples
  std::vector<int16_t> pitch_shifted;

  // Process the samples in overlapping frames
  for (int i = 0; i < samples.size() - HOP_SIZE; i += HOP_SIZE) {
    // Extract the current analysis frame
    std::vector<int16_t> frame(samples.begin() + i, samples.begin() + i + HOP_SIZE);

    // Shift the pitch of the frame
    std::vector<int16_t> shifted(HOP_SIZE * PITCH_SHIFT);
    for (int j = 0; j < shifted.size(); j++) {
      int index = static_cast<int>(j / PITCH_SHIFT);
      if (index < HOP_SIZE) {
        shifted[j] = frame[index];
      } else {
        shifted[j] = 0;
      }
    }

    // Overlap-add the pitch shifted frame with the previous synthesis frame
    if (pitch_shifted.size() < shifted.size()) {
      pitch_shifted.resize(shifted.size());
    }
    for (int j = 0; j < shifted.size(); j++) {
      pitch_shifted[j] += shifted[j];
    }

    // Add the overlapped samples to the output vector
    for (int j = 0; j < HOP_SIZE; j++) {
      int16_t sample = pitch_shifted[j];
      pitch_shifted[j] = 0;
      pitch_shifted[j + HOP_SIZE] = sample;
      if (j + HOP_SIZE < pitch_shifted.size()) {
        pitch_shifted[j + HOP_SIZE] = 0;
      }
      samples.push_back(sample);
    }
  }

  // Write the pitch shifted samples to the output audio file
    output.write(reinterpret_cast<char*>(samples.data()), samples.size() * sizeof(int16_t));

    return 0;
}

// Path: simplePitchShifter.cpp
