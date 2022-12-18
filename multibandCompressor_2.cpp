#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int kNumBands = 3;
const int kSampleRate = 44100;
const float kThresholds[kNumBands] = { -6.0f, -12.0f, -24.0f };
const float kRatios[kNumBands] = { 2.0f, 4.0f, 8.0f };
const float kAttackTimes[kNumBands] = { 0.01f, 0.1f, 0.5f };
const float kReleaseTimes[kNumBands] = { 0.1f, 0.5f, 2.0f };
const float kMakeupGains[kNumBands] = { 6.0f, 12.0f, 24.0f };

class BandCompressor
{
public:
  BandCompressor(float threshold, float ratio, float attack_time, float release_time, float makeup_gain)
    : threshold_(threshold),
      ratio_(ratio),
      attack_time_(attack_time),
      release_time_(release_time),
      makeup_gain_(makeup_gain),
      gain_(1.0f),
      envelope_(0.0f)
  {
  }

  float ProcessSample(float sample)
  {
    float abs_sample = std::fabs(sample);
    if (abs_sample > envelope_)
    {
      // Attack
      envelope_ = abs_sample;
      gain_ = std::pow(envelope_ / threshold_, 1.0f / ratio_ - 1.0f);
    }
    else
    {
      // Release
      envelope_ = envelope_ * std::exp(-1.0f / (release_time_ * kSampleRate));
      gain_ = std::pow(envelope_ / threshold_, 1.0f / ratio_ - 1.0f);
    }
    return sample * (1.0f + makeup_gain_ * gain_);
  }

private:
  float threshold_;
  float ratio_;
  float attack_time_;
  float release_time_;
  float makeup_gain_;
  float gain_;
  float envelope_;
};

std::vector<float> ProcessAudio(const std::vector<float>& audio)
{
  // Create the band compressors
  std::vector<BandCompressor> compressors;
  for (int i = 0; i < kNumBands; i++)
  {
    compressors.emplace_back(kThresholds[i], kRatios[i], kAttackTimes[i], kReleaseTimes[i], kMakeupGains[i]);
  }

  // Process the audio samples
  std::vector<float> processed_audio;
  processed_audio.reserve(audio.size());
  for (float sample : audio)
  {
    // Apply each band compressor to the sample
    for (BandCompressor& compressor : compressors)
    {
      sample = compressor.ProcessSample(sample);
    }
    processed_audio.push_back(sample);
  }
