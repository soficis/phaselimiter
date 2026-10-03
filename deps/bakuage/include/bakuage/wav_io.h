#ifndef BAKUAGE_BAKUAGE_WAV_IO_H_
#define BAKUAGE_BAKUAGE_WAV_IO_H_

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

namespace bakuage {

struct WavInfo {
    int channels = 0;
    int sample_rate = 0;
    int bits_per_sample = 0;
    int format = 0; // 1 = PCM, 3 = IEEE_FLOAT
    size_t frames = 0;
};

void ReadWavRaw(const std::string &filename, WavInfo *info,
                std::vector<float> *samples_float,
                std::vector<double> *samples_double);

template <class Float>
std::vector<Float> ReadWav(const std::string &filename, WavInfo *info_out = nullptr);

template <>
inline std::vector<float> ReadWav<float>(const std::string &filename, WavInfo *info_out) {
    WavInfo info;
    std::vector<float> samples;
    ReadWavRaw(filename, &info, &samples, nullptr);
    if (info_out) *info_out = info;
    return samples;
}

template <>
inline std::vector<double> ReadWav<double>(const std::string &filename, WavInfo *info_out) {
    WavInfo info;
    std::vector<double> samples;
    ReadWavRaw(filename, &info, nullptr, &samples);
    if (info_out) *info_out = info;
    return samples;
}

void WriteWavPcm16(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate);
void WriteWavPcm16(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate);
void WriteWavPcm24(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate);
void WriteWavPcm24(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate);
void WriteWavPcm32(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate);
void WriteWavPcm32(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate);
void WriteWavFloat32(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate);
void WriteWavFloat32(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate);

template <class Float>
void WriteWav(const std::string &filename, const std::vector<Float> &wave, int channels, int sample_rate, int bits_per_sample = 16, bool is_float = false) {
    if (is_float) {
        if (bits_per_sample != 32) {
            throw std::invalid_argument("Float WAV only supports 32-bit");
        }
        WriteWavFloat32(filename, wave.data(), wave.size(), channels, sample_rate);
    } else {
        if (bits_per_sample == 16) {
            WriteWavPcm16(filename, wave.data(), wave.size(), channels, sample_rate);
        } else if (bits_per_sample == 24) {
            WriteWavPcm24(filename, wave.data(), wave.size(), channels, sample_rate);
        } else if (bits_per_sample == 32) {
            WriteWavPcm32(filename, wave.data(), wave.size(), channels, sample_rate);
        } else {
            throw std::invalid_argument("PCM WAV only supports 16, 24, or 32 bits per sample");
        }
    }
}

} // namespace bakuage

#endif // BAKUAGE_BAKUAGE_WAV_IO_H_
