#include "bakuage/wav_io.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace bakuage {
namespace {

template <typename T>
inline T Clamp(T val, T low, T high) {
    return (val < low) ? low : (val > high ? high : val);
}

uint16_t ReadUint16LE(const uint8_t *p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t ReadUint32LE(const uint8_t *p) {
    return static_cast<uint32_t>(p[0]) |
           (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

void WriteUint16LE(uint8_t *p, uint16_t value) {
    p[0] = static_cast<uint8_t>(value & 0xFF);
    p[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

void WriteUint32LE(uint8_t *p, uint32_t value) {
    p[0] = static_cast<uint8_t>(value & 0xFF);
    p[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    p[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    p[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

// SubFormat GUID prefix for KSDATAFORMAT_SUBTYPE_PCM / IEEE_FLOAT:
// {0000000X-0000-0010-8000-00aa00389b71}
const uint8_t kExtensibleGuidTail[14] = {
    0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
};

} // namespace

void ReadWavRaw(const std::string &filename, WavInfo *info,
                std::vector<float> *samples_float,
                std::vector<double> *samples_double) {
    std::ifstream is(filename, std::ios::binary);
    if (!is.is_open()) {
        throw std::runtime_error("Cannot open WAV file: " + filename);
    }

    uint8_t header[12];
    if (!is.read(reinterpret_cast<char *>(header), 12)) {
        throw std::runtime_error("Invalid WAV file: header too short: " + filename);
    }

    if (std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0) {
        throw std::runtime_error("Invalid WAV file: missing RIFF/WAVE signature: " + filename);
    }

    bool has_fmt = false;
    bool has_data = false;
    WavInfo local_info;

    while (is) {
        uint8_t chunk_header[8];
        if (!is.read(reinterpret_cast<char *>(chunk_header), 8)) {
            break;
        }

        const uint32_t chunk_size = ReadUint32LE(chunk_header + 4);

        if (std::memcmp(chunk_header, "fmt ", 4) == 0) {
            if (chunk_size < 16) {
                throw std::runtime_error("Invalid WAV file: fmt chunk too small: " + filename);
            }
            std::vector<uint8_t> fmt_buf(chunk_size);
            if (!is.read(reinterpret_cast<char *>(fmt_buf.data()), chunk_size)) {
                throw std::runtime_error("Truncated WAV file in fmt chunk: " + filename);
            }

            uint16_t format_tag = ReadUint16LE(fmt_buf.data());
            local_info.channels = ReadUint16LE(fmt_buf.data() + 2);
            local_info.sample_rate = ReadUint32LE(fmt_buf.data() + 4);
            local_info.bits_per_sample = ReadUint16LE(fmt_buf.data() + 14);

            if (format_tag == 0xFFFE) { // WAVE_FORMAT_EXTENSIBLE
                if (chunk_size < 40) {
                    throw std::runtime_error("Invalid WAVE_FORMAT_EXTENSIBLE chunk size: " + filename);
                }
                format_tag = ReadUint16LE(fmt_buf.data() + 24);
                if (std::memcmp(fmt_buf.data() + 26, kExtensibleGuidTail, sizeof(kExtensibleGuidTail)) != 0) {
                    throw std::runtime_error("Unsupported extensible WAV GUID: " + filename);
                }
            }

            local_info.format = format_tag;

            if (local_info.channels != 1 && local_info.channels != 2) {
                throw std::runtime_error("Unsupported channel count (" + std::to_string(local_info.channels) +
                                         "): expected mono or stereo: " + filename);
            }

            if (local_info.format == 1) { // PCM
                if (local_info.bits_per_sample != 16 &&
                    local_info.bits_per_sample != 24 &&
                    local_info.bits_per_sample != 32) {
                    throw std::runtime_error("Unsupported PCM bit depth (" + std::to_string(local_info.bits_per_sample) +
                                             "): only 16, 24, 32-bit supported: " + filename);
                }
            } else if (local_info.format == 3) { // IEEE FLOAT
                if (local_info.bits_per_sample != 32) {
                    throw std::runtime_error("Unsupported float bit depth (" + std::to_string(local_info.bits_per_sample) +
                                             "): only 32-bit float supported: " + filename);
                }
            } else {
                throw std::runtime_error("Unsupported WAV audio format code (" + std::to_string(local_info.format) +
                                         "): only PCM and IEEE float supported: " + filename);
            }

            has_fmt = true;
        } else if (std::memcmp(chunk_header, "data", 4) == 0) {
            if (!has_fmt) {
                throw std::runtime_error("WAV data chunk appeared before fmt chunk: " + filename);
            }

            const int bytes_per_sample = local_info.bits_per_sample / 8;
            const int bytes_per_frame = bytes_per_sample * local_info.channels;
            if (bytes_per_frame <= 0) {
                throw std::runtime_error("Invalid frame size in WAV: " + filename);
            }

            local_info.frames = chunk_size / bytes_per_frame;
            const size_t total_samples = local_info.frames * local_info.channels;

            std::vector<uint8_t> raw_data(chunk_size);
            if (!is.read(reinterpret_cast<char *>(raw_data.data()), chunk_size)) {
                throw std::runtime_error("Truncated WAV file in data chunk: " + filename);
            }

            if (samples_float) {
                samples_float->resize(total_samples);
            }
            if (samples_double) {
                samples_double->resize(total_samples);
            }

            const uint8_t *ptr = raw_data.data();
            for (size_t i = 0; i < total_samples; ++i) {
                double val = 0.0;
                if (local_info.format == 1) { // PCM
                    if (local_info.bits_per_sample == 16) {
                        int16_t s = static_cast<int16_t>(ReadUint16LE(ptr));
                        val = static_cast<double>(s) / 32768.0;
                        ptr += 2;
                    } else if (local_info.bits_per_sample == 24) {
                        uint32_t u = static_cast<uint32_t>(ptr[0]) |
                                     (static_cast<uint32_t>(ptr[1]) << 8) |
                                     (static_cast<uint32_t>(ptr[2]) << 16);
                        int32_t s = (u & 0x800000) ? static_cast<int32_t>(u | 0xFF000000) : static_cast<int32_t>(u);
                        val = static_cast<double>(s) / 8388608.0;
                        ptr += 3;
                    } else if (local_info.bits_per_sample == 32) {
                        int32_t s = static_cast<int32_t>(ReadUint32LE(ptr));
                        val = static_cast<double>(s) / 2147483648.0;
                        ptr += 4;
                    }
                } else if (local_info.format == 3) { // IEEE float 32-bit
                    float fval = 0.0f;
                    std::memcpy(&fval, ptr, sizeof(float));
                    val = static_cast<double>(fval);
                    ptr += 4;
                }

                if (samples_float) {
                    (*samples_float)[i] = static_cast<float>(val);
                }
                if (samples_double) {
                    (*samples_double)[i] = val;
                }
            }

            has_data = true;
            break;
        } else {
            // Skip unknown chunk with 2-byte alignment padding
            std::streamoff skip_bytes = chunk_size + (chunk_size % 2);
            is.seekg(skip_bytes, std::ios::cur);
        }
    }

    if (!has_fmt || !has_data) {
        throw std::runtime_error("Incomplete WAV file: missing fmt or data chunk: " + filename);
    }

    if (info) {
        *info = local_info;
    }
}

namespace {

template <class Float>
void WriteWavInternal(const std::string &filename, const Float *samples, size_t count,
                      int channels, int sample_rate, int bits_per_sample, bool is_float) {
    if (channels != 1 && channels != 2) {
        throw std::invalid_argument("WAV channels must be 1 or 2");
    }
    if (count % channels != 0) {
        throw std::invalid_argument("Sample count must be a multiple of channels");
    }

    const size_t frames = count / channels;
    const int bytes_per_sample = bits_per_sample / 8;
    const int block_align = channels * bytes_per_sample;
    const uint32_t data_size = static_cast<uint32_t>(frames * block_align);
    const uint32_t byte_rate = static_cast<uint32_t>(sample_rate * block_align);

    std::ofstream os(filename, std::ios::binary);
    if (!os.is_open()) {
        throw std::runtime_error("Cannot open WAV file for writing: " + filename);
    }

    // fmt chunk size: 16 bytes for standard PCM, 18 for basic extensible/float or 16
    const uint32_t fmt_size = 16;
    const uint32_t riff_size = 4 + (8 + fmt_size) + (8 + data_size);

    uint8_t header[44];
    std::memcpy(header, "RIFF", 4);
    WriteUint32LE(header + 4, riff_size);
    std::memcpy(header + 8, "WAVE", 4);

    std::memcpy(header + 12, "fmt ", 4);
    WriteUint32LE(header + 16, fmt_size);
    WriteUint16LE(header + 20, is_float ? 3 : 1);
    WriteUint16LE(header + 22, static_cast<uint16_t>(channels));
    WriteUint32LE(header + 24, static_cast<uint32_t>(sample_rate));
    WriteUint32LE(header + 28, byte_rate);
    WriteUint16LE(header + 32, static_cast<uint16_t>(block_align));
    WriteUint16LE(header + 34, static_cast<uint16_t>(bits_per_sample));

    std::memcpy(header + 36, "data", 4);
    WriteUint32LE(header + 40, data_size);

    os.write(reinterpret_cast<const char *>(header), 44);

    std::vector<uint8_t> buffer(data_size);
    uint8_t *ptr = buffer.data();

    for (size_t i = 0; i < count; ++i) {
        const double val = static_cast<double>(samples[i]);
        if (is_float) {
            float f = static_cast<float>(val);
            std::memcpy(ptr, &f, sizeof(float));
            ptr += sizeof(float);
        } else {
            if (bits_per_sample == 16) {
                double scaled = Clamp(val * 32768.0, -32768.0, 32767.0);
                int16_t s = static_cast<int16_t>(std::llround(scaled));
                WriteUint16LE(ptr, static_cast<uint16_t>(s));
                ptr += 2;
            } else if (bits_per_sample == 24) {
                double scaled = Clamp(val * 8388608.0, -8388608.0, 8388607.0);
                int32_t s = static_cast<int32_t>(std::llround(scaled));
                ptr[0] = static_cast<uint8_t>(s & 0xFF);
                ptr[1] = static_cast<uint8_t>((s >> 8) & 0xFF);
                ptr[2] = static_cast<uint8_t>((s >> 16) & 0xFF);
                ptr += 3;
            } else if (bits_per_sample == 32) {
                double scaled = Clamp(val * 2147483648.0, -2147483648.0, 2147483647.0);
                int32_t s = static_cast<int32_t>(std::llround(scaled));
                WriteUint32LE(ptr, static_cast<uint32_t>(s));
                ptr += 4;
            }
        }
    }

    os.write(reinterpret_cast<const char *>(buffer.data()), data_size);
}

} // namespace

void WriteWavPcm16(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 16, false);
}
void WriteWavPcm16(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 16, false);
}
void WriteWavPcm24(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 24, false);
}
void WriteWavPcm24(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 24, false);
}
void WriteWavPcm32(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 32, false);
}
void WriteWavPcm32(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 32, false);
}
void WriteWavFloat32(const std::string &filename, const float *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 32, true);
}
void WriteWavFloat32(const std::string &filename, const double *samples, size_t count, int channels, int sample_rate) {
    WriteWavInternal(filename, samples, count, channels, sample_rate, 32, true);
}

} // namespace bakuage
