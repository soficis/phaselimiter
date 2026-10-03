#!/usr/bin/env python3
"""
scripts/equivalence_gate.py

PhaseLimiter Numerical Equivalence Gate (R2-4).
Renders audio through both reference and candidate PhaseLimiter binaries,
and validates that differences satisfy:
  - Integrated loudness: |Δ| <= 0.1 LU
  - True Peak: |Δ| <= 0.1 dBTP
  - Difference-signal RMS: <= -60 dBFS
"""

import argparse
import math
import os
import re
import struct
import subprocess
import sys
import tempfile
import wave

MAX_LUFS_DIFF = 0.1
MAX_PEAK_DIFF = 0.1
MAX_DIFF_RMS_DBFS = -60.0


def generate_90s_test_wav(filepath):
    """Generate a deterministic 90-second test WAV (sines + pseudo pink noise)."""
    sample_rate = 44100
    duration_s = 90
    num_samples = sample_rate * duration_s
    channels = 2

    print(f"Generating deterministic 90s test WAV: {filepath}...")
    os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)

    b0, b1, b2, b3, b4, b5, b6 = 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
    rand_state = 123456789

    with wave.open(filepath, "wb") as w:
        w.setnchannels(channels)
        w.setsampwidth(2)
        w.setframerate(sample_rate)

        chunk_size = 4096
        frames_written = 0

        while frames_written < num_samples:
            current_chunk = min(chunk_size, num_samples - frames_written)
            frame_bytes = bytearray()

            for i in range(current_chunk):
                t = (frames_written + i) / float(sample_rate)

                # Linear congruential generator for white noise
                rand_state = (1103515245 * rand_state + 12345) & 0x7FFFFFFF
                white = (rand_state / 1073741824.0) - 1.0

                # Paul Kellet's filter for pink noise approximation
                b0 = 0.99886 * b0 + white * 0.0555179
                b1 = 0.99332 * b1 + white * 0.0750759
                b2 = 0.96900 * b2 + white * 0.1538520
                b3 = 0.86650 * b3 + white * 0.3104856
                b4 = 0.55000 * b4 + white * 0.5329522
                b5 = -0.7616 * b5 - white * 0.0168980
                pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362
                b6 = white * 0.115926

                # Sines at 100 Hz, 1 kHz, 5 kHz
                s1 = math.sin(2.0 * math.pi * 100.0 * t) * 0.2
                s2 = math.sin(2.0 * math.pi * 1000.0 * t) * 0.15
                s3 = math.sin(2.0 * math.pi * 5000.0 * t) * 0.1
                combined = (s1 + s2 + s3 + pink * 0.1) * 0.35  # headroom

                val = int(max(-32768, min(32767, combined * 32767.0)))
                frame_bytes.extend(struct.pack("<hh", val, val))

            w.writeframes(frame_bytes)
            frames_written += current_chunk


def get_wav_duration(filepath):
    """Return duration of WAV file in seconds."""
    try:
        with wave.open(filepath, "rb") as w:
            frames = w.getnframes()
            rate = w.getframerate()
            return frames / float(rate)
    except Exception:
        return 0.0


def run_phase_limiter(binary_path, input_wav, output_wav, resource_dir, tmp_dir):
    """Run phase_limiter with AutoMixMaster exact flags."""
    ref_file = os.path.join(resource_dir, "mastering_reference.json")
    cache_file = os.path.join(resource_dir, "sound_quality2_cache")

    if not os.path.exists(ref_file):
        # Look in directory of binary
        alt_res = os.path.join(os.path.dirname(binary_path), "..", "resource")
        ref_file = os.path.join(alt_res, "mastering_reference.json")
        cache_file = os.path.join(alt_res, "sound_quality2_cache")

    cmd = [
        binary_path,
        f"-input={input_wav}",
        f"-output={output_wav}",
        f"-mastering_reference_file={ref_file}",
        f"-sound_quality2_cache={cache_file}",
        "-disable_input_encode=true",
        "-output_format=wav",
        "-sample_rate=44100",
        "-bit_depth=16",
        "-ceiling=-0.1",
        "-mastering=true",
        f"-tmp={tmp_dir}"
    ]

    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        raise RuntimeError(f"Command failed ({res.returncode}): {' '.join(cmd)}\nStderr: {res.stderr}")


def measure_loudness_and_peak(wav_path):
    """Measure integrated loudness (LUFS) and True Peak (dBTP) with ffmpeg ebur128."""
    cmd = [
        "ffmpeg", "-nostats", "-i", wav_path,
        "-af", "ebur128=peak=true",
        "-f", "null", "-"
    ]
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    out = res.stderr

    # Parse Integrated Loudness: "  I:         -14.2 LUFS"
    i_match = re.search(r"I:\s*(-?[0-9.]+)\s*LUFS", out)
    integrated = float(i_match.group(1)) if i_match else float("nan")

    # Parse Peak: "  Peak:       -0.1 dBFS" or True Peak
    p_match = re.search(r"Peak:\s*(-?[0-9.]+)\s*dB", out)
    peak = float(p_match.group(1)) if p_match else float("nan")

    return integrated, peak


def compute_diff_rms_dbfs(ref_wav, cand_wav):
    """Calculate the difference-signal RMS in dBFS between two 16-bit WAV files."""
    with wave.open(ref_wav, "rb") as w_ref, wave.open(cand_wav, "rb") as w_cand:
        if w_ref.getnchannels() != w_cand.getnchannels() or w_ref.getsampwidth() != w_cand.getsampwidth():
            raise ValueError("WAV channel count or sample width mismatch")

        sampwidth = w_ref.getsampwidth()
        channels = w_ref.getnchannels()
        if sampwidth != 2:
            raise ValueError(f"Unsupported sample width: {sampwidth}")

        total_samples = 0
        sum_sq = 0.0

        while True:
            b_ref = w_ref.readframes(4096)
            b_cand = w_cand.readframes(4096)
            if not b_ref or not b_cand:
                break

            count = min(len(b_ref), len(b_cand)) // 2
            fmt = f"<{count}h"
            vals_ref = struct.unpack(fmt, b_ref[:count * 2])
            vals_cand = struct.unpack(fmt, b_cand[:count * 2])

            for vr, vc in zip(vals_ref, vals_cand):
                diff = (vr - vc) / 32768.0
                sum_sq += diff * diff
                total_samples += 1

        if total_samples == 0:
            return -120.0

        rms = math.sqrt(sum_sq / total_samples)
        if rms <= 1e-9:
            return -120.0
        return 20.0 * math.log10(rms)


def main():
    parser = argparse.ArgumentParser(description="PhaseLimiter Numerical Equivalence Gate")
    parser.add_argument("reference_bin", help="Path to reference PhaseLimiter binary (upstream IPP)")
    parser.add_argument("candidate_bin", help="Path to candidate PhaseLimiter binary")
    parser.add_argument("wav_inputs", nargs="*", help="Input WAV files to evaluate")
    parser.add_argument("--resource-dir", default="resource", help="Path to PhaseLimiter resource directory")
    args = parser.parse_args()

    wavs = list(args.wav_inputs)

    # Ensure at least 3 inputs with at least one >= 60 seconds
    has_long = any(get_wav_duration(w) >= 60.0 for w in wavs)
    if not has_long or len(wavs) < 3:
        default_dir = os.path.join(tempfile.gettempdir(), "phaselimiter_gate")
        gen_path = os.path.join(default_dir, "synth_90s.wav")
        if not os.path.exists(gen_path):
            generate_90s_test_wav(gen_path)
        if gen_path not in wavs:
            wavs.append(gen_path)

    print("=== PhaseLimiter Equivalence Gate ===")
    print(f"Reference: {args.reference_bin}")
    print(f"Candidate: {args.candidate_bin}")
    print(f"Evaluating {len(wavs)} audio input(s):\n")

    table_header = f"{'Input File':<28} | {'Ref LUFS':<8} | {'Cand LUFS':<9} | {'Δ LUFS':<6} | {'Ref Pk':<6} | {'Cand Pk':<7} | {'Δ Pk':<5} | {'Diff RMS':<9} | {'Gate'}"
    print(table_header)
    print("-" * len(table_header))

    all_passed = True

    with tempfile.TemporaryDirectory() as tmp_dir:
        for idx, wav in enumerate(wavs, 1):
            base_name = os.path.basename(wav)
            ref_out = os.path.join(tmp_dir, f"ref_{idx}.wav")
            cand_out = os.path.join(tmp_dir, f"cand_{idx}.wav")
            sub_tmp = os.path.join(tmp_dir, f"tmp_{idx}")
            os.makedirs(sub_tmp, exist_ok=True)

            run_phase_limiter(args.reference_bin, wav, ref_out, args.resource_dir, sub_tmp)
            run_phase_limiter(args.candidate_bin, wav, cand_out, args.resource_dir, sub_tmp)

            ref_lufs, ref_pk = measure_loudness_and_peak(ref_out)
            cand_lufs, cand_pk = measure_loudness_and_peak(cand_out)

            delta_lufs = abs(ref_lufs - cand_lufs)
            delta_pk = abs(ref_pk - cand_pk)
            diff_rms = compute_diff_rms_dbfs(ref_out, cand_out)

            passed = (
                delta_lufs <= MAX_LUFS_DIFF and
                delta_pk <= MAX_PEAK_DIFF and
                diff_rms <= MAX_DIFF_RMS_DBFS
            )

            if not passed:
                all_passed = False

            status_str = "PASS" if passed else "FAIL"
            print(f"{base_name:<28} | {ref_lufs:>8.2f} | {cand_lufs:>9.2f} | {delta_lufs:>6.2f} | {ref_pk:>6.2f} | {cand_pk:>7.2f} | {delta_pk:>5.2f} | {diff_rms:>8.2f}d | {status_str}")

    print("\n" + "=" * len(table_header))
    if all_passed:
        print("✅ EQUIVALENCE GATE PASSED: All numerical tolerances satisfied.")
        sys.exit(0)
    else:
        print("❌ EQUIVALENCE GATE FAILED: One or more files exceeded numerical tolerance thresholds.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
