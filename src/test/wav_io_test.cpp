#include "gtest/gtest.h"
#include "bakuage/wav_io.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <vector>

namespace {

void TruncateFile(const std::string &path, size_t new_size) {
    std::ifstream in(path, std::ios::binary);
    std::vector<char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    if (new_size > data.size()) {
        new_size = data.size();
    }
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(data.data(), new_size);
}

} // namespace

TEST(WavIoTest, RoundTripInt16Mono) {
    const std::string filename = "/tmp/test_pcm16_mono.wav";
    const std::vector<int16_t> original = {
        static_cast<int16_t>(-32768), -32767, -20000, -1000, -1, 0, 1, 1000, 20000, 32766, 32767
    };
    std::vector<double> float_samples(original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        float_samples[i] = static_cast<double>(original[i]) / 32768.0;
    }

    bakuage::WriteWavPcm16(filename, float_samples.data(), float_samples.size(), 1, 44100);

    bakuage::WavInfo info;
    const auto read_samples = bakuage::ReadWav<double>(filename, &info);

    EXPECT_EQ(1, info.channels);
    EXPECT_EQ(44100, info.sample_rate);
    EXPECT_EQ(16, info.bits_per_sample);
    EXPECT_EQ(1, info.format);
    EXPECT_EQ(original.size(), info.frames);
    ASSERT_EQ(original.size(), read_samples.size());

    for (size_t i = 0; i < original.size(); ++i) {
        int16_t reconstructed = static_cast<int16_t>(std::llround(read_samples[i] * 32768.0));
        EXPECT_EQ(original[i], reconstructed) << "Mismatch at sample " << i;
    }
}

TEST(WavIoTest, RoundTripInt16Stereo) {
    const std::string filename = "/tmp/test_pcm16_stereo.wav";
    const std::vector<int16_t> original = {
        -32768, 32767,
        -1000, 1000,
        0, 0,
        1234, -4321
    };
    std::vector<float> float_samples(original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        float_samples[i] = static_cast<float>(original[i]) / 32768.0f;
    }

    bakuage::WriteWavPcm16(filename, float_samples.data(), float_samples.size(), 2, 48000);

    bakuage::WavInfo info;
    const auto read_samples = bakuage::ReadWav<float>(filename, &info);

    EXPECT_EQ(2, info.channels);
    EXPECT_EQ(48000, info.sample_rate);
    EXPECT_EQ(16, info.bits_per_sample);
    EXPECT_EQ(1, info.format);
    EXPECT_EQ(original.size() / 2, info.frames);
    ASSERT_EQ(original.size(), read_samples.size());

    for (size_t i = 0; i < original.size(); ++i) {
        int16_t reconstructed = static_cast<int16_t>(std::llround(read_samples[i] * 32768.0f));
        EXPECT_EQ(original[i], reconstructed) << "Stereo mismatch at sample " << i;
    }
}

TEST(WavIoTest, RoundTripInt24MonoAndStereo) {
    const std::string filename_mono = "/tmp/test_pcm24_mono.wav";
    const std::vector<int32_t> original_mono = {
        -8388608, -8388607, -1000000, -1, 0, 1, 1000000, 8388606, 8388607
    };
    std::vector<double> float_mono(original_mono.size());
    for (size_t i = 0; i < original_mono.size(); ++i) {
        float_mono[i] = static_cast<double>(original_mono[i]) / 8388608.0;
    }

    bakuage::WriteWavPcm24(filename_mono, float_mono.data(), float_mono.size(), 1, 44100);

    bakuage::WavInfo info_mono;
    const auto read_mono = bakuage::ReadWav<double>(filename_mono, &info_mono);

    EXPECT_EQ(1, info_mono.channels);
    EXPECT_EQ(24, info_mono.bits_per_sample);
    EXPECT_EQ(1, info_mono.format);
    ASSERT_EQ(original_mono.size(), read_mono.size());

    for (size_t i = 0; i < original_mono.size(); ++i) {
        int32_t reconstructed = static_cast<int32_t>(std::llround(read_mono[i] * 8388608.0));
        EXPECT_EQ(original_mono[i], reconstructed) << "24-bit mono mismatch at sample " << i;
    }

    // Stereo
    const std::string filename_stereo = "/tmp/test_pcm24_stereo.wav";
    const std::vector<int32_t> original_stereo = {
        -8388608, 8388607,
        -500, 500,
        0, 100000
    };
    std::vector<double> float_stereo(original_stereo.size());
    for (size_t i = 0; i < original_stereo.size(); ++i) {
        float_stereo[i] = static_cast<double>(original_stereo[i]) / 8388608.0;
    }

    bakuage::WriteWavPcm24(filename_stereo, float_stereo.data(), float_stereo.size(), 2, 44100);

    bakuage::WavInfo info_stereo;
    const auto read_stereo = bakuage::ReadWav<double>(filename_stereo, &info_stereo);
    EXPECT_EQ(2, info_stereo.channels);
    EXPECT_EQ(24, info_stereo.bits_per_sample);
    ASSERT_EQ(original_stereo.size(), read_stereo.size());

    for (size_t i = 0; i < original_stereo.size(); ++i) {
        int32_t reconstructed = static_cast<int32_t>(std::llround(read_stereo[i] * 8388608.0));
        EXPECT_EQ(original_stereo[i], reconstructed) << "24-bit stereo mismatch at sample " << i;
    }
}

TEST(WavIoTest, RoundTripInt32MonoAndStereo) {
    const std::string filename = "/tmp/test_pcm32_mono.wav";
    const std::vector<int32_t> original = {
        -2147483647 - 1, -2147483647, -1000000000, -1, 0, 1, 1000000000, 2147483646, 2147483647
    };
    std::vector<double> float_samples(original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        float_samples[i] = static_cast<double>(original[i]) / 2147483648.0;
    }

    bakuage::WriteWavPcm32(filename, float_samples.data(), float_samples.size(), 1, 44100);

    bakuage::WavInfo info;
    const auto read_samples = bakuage::ReadWav<double>(filename, &info);

    EXPECT_EQ(1, info.channels);
    EXPECT_EQ(32, info.bits_per_sample);
    EXPECT_EQ(1, info.format);
    ASSERT_EQ(original.size(), read_samples.size());

    for (size_t i = 0; i < original.size(); ++i) {
        int32_t reconstructed = static_cast<int32_t>(std::llround(read_samples[i] * 2147483648.0));
        EXPECT_EQ(original[i], reconstructed) << "32-bit int mismatch at sample " << i;
    }
}

TEST(WavIoTest, RoundTripFloat32MonoAndStereo) {
    const std::string filename = "/tmp/test_float32.wav";
    const std::vector<float> original = {
        -1.0f, -0.75f, -0.5f, -0.25f, 0.0f, 0.125f, 0.25f, 0.5f, 0.75f, 1.0f
    };

    bakuage::WriteWavFloat32(filename, original.data(), original.size(), 1, 44100);

    bakuage::WavInfo info;
    const auto read_samples = bakuage::ReadWav<float>(filename, &info);

    EXPECT_EQ(1, info.channels);
    EXPECT_EQ(32, info.bits_per_sample);
    EXPECT_EQ(3, info.format);
    ASSERT_EQ(original.size(), read_samples.size());

    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_FLOAT_EQ(original[i], read_samples[i]) << "Float mismatch at sample " << i;
    }
}

TEST(WavIoTest, TruncatedWavThrowsAndNeverCrashes) {
    const std::string filename = "/tmp/test_truncated.wav";
    const std::vector<float> original = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    bakuage::WriteWavFloat32(filename, original.data(), original.size(), 2, 44100);

    // Truncate to 10 bytes (header too short)
    TruncateFile(filename, 10);
    EXPECT_THROW(bakuage::ReadWav<float>(filename), std::runtime_error);

    // Truncate in data chunk
    bakuage::WriteWavFloat32(filename, original.data(), original.size(), 2, 44100);
    TruncateFile(filename, 50); // Cut data section midway
    EXPECT_THROW(bakuage::ReadWav<float>(filename), std::runtime_error);
}

TEST(WavIoTest, CorruptSignatureThrows) {
    const std::string filename = "/tmp/test_corrupt_sig.wav";
    std::ofstream out(filename, std::ios::binary);
    out.write("NOT_A_WAV_FILE_HEADER", 21);
    out.close();

    EXPECT_THROW(bakuage::ReadWav<float>(filename), std::runtime_error);
}

TEST(WavIoTest, NonPcmAudioFormatThrows) {
    const std::string filename = "/tmp/test_non_pcm.wav";
    const std::vector<float> original = {0.1f, 0.2f};
    bakuage::WriteWavPcm16(filename, original.data(), original.size(), 1, 44100);

    // Corrupt format code at offset 20 from 1 (PCM) to 7 (mu-law)
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    file.seekp(20);
    uint16_t bad_format = 7;
    file.write(reinterpret_cast<const char *>(&bad_format), sizeof(bad_format));
    file.close();

    EXPECT_THROW(bakuage::ReadWav<float>(filename), std::runtime_error);
}

TEST(WavIoTest, UnsupportedChannelsThrows) {
    const std::string filename = "/tmp/test_unsupported_ch.wav";
    const std::vector<float> original = {0.1f, 0.2f};
    bakuage::WriteWavPcm16(filename, original.data(), original.size(), 1, 44100);

    // Corrupt channel count at offset 22 to 6
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    file.seekp(22);
    uint16_t six_channels = 6;
    file.write(reinterpret_cast<const char *>(&six_channels), sizeof(six_channels));
    file.close();

    EXPECT_THROW(bakuage::ReadWav<float>(filename), std::runtime_error);
}
