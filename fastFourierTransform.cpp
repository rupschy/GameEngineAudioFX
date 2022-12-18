#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <algorithm>

const int FFT_SIZE = 1024; // size of the FFT

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

  // Pad the samples with zeros to the nearest power of 2
  int padded_length = 1;
  while (padded_length < length) {
    padded_length *= 2;
  }
  samples.resize(padded_length, 0);

  // Convert the samples to the frequency domain using the FFT
  std::vector<std::complex<double>> fft(padded_length);
  for (int i = 0; i < padded_length; i++) {
    for (int j = 0; j < padded_length; j++) {
      fft[i] += std::polar(1.0, -2.0 * M_PI * i * j / (padded_length * samples[j]));
      // May have issues with parentheses
    }
  }

  // Write the frequency domain signal to the output audio file
  output.write(reinterpret_cast<char*>(fft.data()), fft.size() * sizeof(std::complex<double>));

  return 0;
}
