#include "ipp.h"
#include "bakuage/pocketfft_hdronly.h"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <complex>
#include <vector>
#include <algorithm>
#include <limits>

#if defined(_WIN32)
#include <malloc.h>
static inline void* compat_aligned_alloc(size_t size) {
    return _aligned_malloc(size, 64);
}
static inline void compat_aligned_free(void *ptr) {
    _aligned_free(ptr);
}
#else
static inline void* compat_aligned_alloc(size_t size) {
    void *ptr = nullptr;
    if (posix_memalign(&ptr, 64, size ? size : 64) != 0) {
        return nullptr;
    }
    return ptr;
}
static inline void compat_aligned_free(void *ptr) {
    free(ptr);
}
#endif

/* System & Library version */
static const IppLibraryVersion g_ippLibVersion = {
    2021, 0, 0, 0,
    "portable",
    "ipp_compat_pocketfft",
    "2021.0.0 (pocketfft native port)",
    __DATE__
};

extern "C" IppStatus ippInit() {
    return ippStsNoErr;
}

extern "C" const IppLibraryVersion* ippGetLibVersion() {
    return &g_ippLibVersion;
}

/* Memory allocation */
extern "C" Ipp8u* ippsMalloc_8u(int len) {
    return (Ipp8u*)compat_aligned_alloc(len * sizeof(Ipp8u));
}

extern "C" Ipp32f* ippsMalloc_32f(int len) {
    return (Ipp32f*)compat_aligned_alloc(len * sizeof(Ipp32f));
}

extern "C" Ipp64f* ippsMalloc_64f(int len) {
    return (Ipp64f*)compat_aligned_alloc(len * sizeof(Ipp64f));
}

extern "C" Ipp32fc* ippsMalloc_32fc(int len) {
    return (Ipp32fc*)compat_aligned_alloc(len * sizeof(Ipp32fc));
}

extern "C" Ipp64fc* ippsMalloc_64fc(int len) {
    return (Ipp64fc*)compat_aligned_alloc(len * sizeof(Ipp64fc));
}

extern "C" void ippsFree(void *ptr) {
    if (ptr) compat_aligned_free(ptr);
}

/* Vector math */
/* Vector math */
extern "C" IppStatus ippsMulC_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[i] * val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulC_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] *= val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulC_32fc_I(Ipp32fc val, Ipp32fc *pSrcDst, int len) {
    float c = val.re, d = val.im;
    for (int i = 0; i < len; ++i) {
        float a = pSrcDst[i].re, b = pSrcDst[i].im;
        pSrcDst[i].re = a * c - b * d;
        pSrcDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulC_64f(const Ipp64f *pSrc, Ipp64f val, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[i] * val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulC_64f_I(Ipp64f val, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] *= val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulC_64fc_I(Ipp64fc val, Ipp64fc *pSrcDst, int len) {
    double c = val.re, d = val.im;
    for (int i = 0; i < len; ++i) {
        double a = pSrcDst[i].re, b = pSrcDst[i].im;
        pSrcDst[i].re = a * c - b * d;
        pSrcDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc1[i] * pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] *= pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32f32fc(const Ipp32f *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        pDst[i].re = pSrc1[i] * pSrc2[i].re;
        pDst[i].im = pSrc1[i] * pSrc2[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32f32fc_I(const Ipp32f *pSrc, Ipp32fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        pSrcDst[i].re *= pSrc[i];
        pSrcDst[i].im *= pSrc[i];
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        float a = pSrc1[i].re, b = pSrc1[i].im;
        float c = pSrc2[i].re, d = pSrc2[i].im;
        pDst[i].re = a * c - b * d;
        pDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_32fc_I(const Ipp32fc *pSrc, Ipp32fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        float a = pSrcDst[i].re, b = pSrcDst[i].im;
        float c = pSrc[i].re, d = pSrc[i].im;
        pSrcDst[i].re = a * c - b * d;
        pSrcDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc1[i] * pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] *= pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        double a = pSrc1[i].re, b = pSrc1[i].im;
        double c = pSrc2[i].re, d = pSrc2[i].im;
        pDst[i].re = a * c - b * d;
        pDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMul_64fc_I(const Ipp64fc *pSrc, Ipp64fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        double a = pSrcDst[i].re, b = pSrcDst[i].im;
        double c = pSrc[i].re, d = pSrc[i].im;
        pSrcDst[i].re = a * c - b * d;
        pSrcDst[i].im = a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulPerm_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len) {
    if (len <= 0) return ippStsNoErr;
    pSrcDst[0] *= pSrc[0];
    if (len > 1) {
        pSrcDst[1] *= pSrc[1];
    }
    for (int k = 1; k < len / 2; ++k) {
        float re1 = pSrcDst[2 * k];
        float im1 = pSrcDst[2 * k + 1];
        float re2 = pSrc[2 * k];
        float im2 = pSrc[2 * k + 1];
        pSrcDst[2 * k]     = re1 * re2 - im1 * im2;
        pSrcDst[2 * k + 1] = re1 * im2 + im1 * re2;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulPerm_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len) {
    if (len <= 0) return ippStsNoErr;
    pSrcDst[0] *= pSrc[0];
    if (len > 1) {
        pSrcDst[1] *= pSrc[1];
    }
    for (int k = 1; k < len / 2; ++k) {
        double re1 = pSrcDst[2 * k];
        double im1 = pSrcDst[2 * k + 1];
        double re2 = pSrc[2 * k];
        double im2 = pSrc[2 * k + 1];
        pSrcDst[2 * k]     = re1 * re2 - im1 * im2;
        pSrcDst[2 * k + 1] = re1 * im2 + im1 * re2;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulByConj_32fc_A24(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        float a = pSrc1[i].re, b = pSrc1[i].im;
        float c = pSrc2[i].re, d = pSrc2[i].im;
        pDst[i].re = a * c + b * d;
        pDst[i].im = b * c - a * d;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsMulByConj_64fc_A53(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        double a = pSrc1[i].re, b = pSrc1[i].im;
        double c = pSrc2[i].re, d = pSrc2[i].im;
        pDst[i].re = a * c + b * d;
        pDst[i].im = b * c - a * d;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddC_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddC_64f_I(Ipp64f val, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_32fc_I(const Ipp32fc *pSrc, Ipp32fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        pSrcDst[i].re += pSrc[i].re;
        pSrcDst[i].im += pSrc[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_64fc_I(const Ipp64fc *pSrc, Ipp64fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        pSrcDst[i].re += pSrc[i].re;
        pSrcDst[i].im += pSrc[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc1[i] + pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc1[i] + pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        pDst[i].re = pSrc1[i].re + pSrc2[i].re;
        pDst[i].im = pSrc1[i].im + pSrc2[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAdd_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        pDst[i].re = pSrc1[i].re + pSrc2[i].re;
        pDst[i].im = pSrc1[i].im + pSrc2[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsSubCRev_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = val - pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsDiv_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] /= pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsDiv_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] /= pSrc[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProduct_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc1[i] * pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProduct_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc1[i] * pSrc2[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProduct_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        float a = pSrc1[i].re, b = pSrc1[i].im;
        float c = pSrc2[i].re, d = pSrc2[i].im;
        pSrcDst[i].re += a * c - b * d;
        pSrcDst[i].im += a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProduct_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) {
        double a = pSrc1[i].re, b = pSrc1[i].im;
        double c = pSrc2[i].re, d = pSrc2[i].im;
        pSrcDst[i].re += a * c - b * d;
        pSrcDst[i].im += a * d + b * c;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProductC_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc[i] * val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsAddProductC_64f(const Ipp64f *pSrc, Ipp64f val, Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] += pSrc[i] * val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsPowx_32f_A24(const Ipp32f *pSrc1, const Ipp32f val, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = std::pow(pSrc1[i], val);
    return ippStsNoErr;
}

extern "C" IppStatus ippsPowx_64f_A53(const Ipp64f *pSrc1, const Ipp64f val, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = std::pow(pSrc1[i], val);
    return ippStsNoErr;
}

extern "C" IppStatus ippsSqrt_32f_I(Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] = std::sqrt(pSrcDst[i]);
    return ippStsNoErr;
}

extern "C" IppStatus ippsSqrt_64f_I(Ipp64f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] = std::sqrt(pSrcDst[i]);
    return ippStsNoErr;
}

extern "C" IppStatus ippsPowerSpectr_32fc(const Ipp32fc *pSrc, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[i].re * pSrc[i].re + pSrc[i].im * pSrc[i].im;
    return ippStsNoErr;
}

extern "C" IppStatus ippsPowerSpectr_64fc(const Ipp64fc *pSrc, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[i].re * pSrc[i].re + pSrc[i].im * pSrc[i].im;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_L1_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += std::abs((double)pSrc1[i] - (double)pSrc2[i]);
    *pDiff = (float)sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_L1_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += std::abs(pSrc1[i] - pSrc2[i]);
    *pDiff = sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_L2_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) {
        double d = (double)pSrc1[i] - (double)pSrc2[i];
        sum += d * d;
    }
    *pDiff = (float)std::sqrt(sum);
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_L2_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) {
        double d = pSrc1[i] - pSrc2[i];
        sum += d * d;
    }
    *pDiff = std::sqrt(sum);
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_Inf_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff) {
    float maxVal = 0.0f;
    for (int i = 0; i < len; ++i) {
        float d = std::abs(pSrc1[i] - pSrc2[i]);
        if (d > maxVal) maxVal = d;
    }
    *pDiff = maxVal;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNormDiff_Inf_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff) {
    double maxVal = 0.0;
    for (int i = 0; i < len; ++i) {
        double d = std::abs(pSrc1[i] - pSrc2[i]);
        if (d > maxVal) maxVal = d;
    }
    *pDiff = maxVal;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNorm_Inf_32f(const Ipp32f *pSrc, int len, Ipp32f *pNorm) {
    float maxVal = 0.0f;
    for (int i = 0; i < len; ++i) {
        float v = std::abs(pSrc[i]);
        if (v > maxVal) maxVal = v;
    }
    *pNorm = maxVal;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNorm_Inf_64f(const Ipp64f *pSrc, int len, Ipp64f *pNorm) {
    double maxVal = 0.0;
    for (int i = 0; i < len; ++i) {
        double v = std::abs(pSrc[i]);
        if (v > maxVal) maxVal = v;
    }
    *pNorm = maxVal;
    return ippStsNoErr;
}

extern "C" IppStatus ippsNorm_L2_32f(const Ipp32f *pSrc, int len, Ipp32f *pNorm) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) {
        double v = pSrc[i];
        sum += v * v;
    }
    *pNorm = (float)std::sqrt(sum);
    return ippStsNoErr;
}

extern "C" IppStatus ippsNorm_L2_64f(const Ipp64f *pSrc, int len, Ipp64f *pNorm) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) {
        double v = pSrc[i];
        sum += v * v;
    }
    *pNorm = std::sqrt(sum);
    return ippStsNoErr;
}

extern "C" IppStatus ippsSum_32f(const Ipp32f *pSrc, int len, Ipp32f *pSum, IppHintAlgorithm) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += pSrc[i];
    *pSum = (float)sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSum_64f(const Ipp64f *pSrc, int len, Ipp64f *pSum) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += pSrc[i];
    *pSum = sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDivCRev_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i] = val / pSrcDst[i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsSet_32f(Ipp32f val, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSet_64f(Ipp64f val, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSet_32s(Ipp32s val, Ipp32s *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = val;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSampleDown_32f(const Ipp32f *pSrc, int srcLen, Ipp32f *pDst, int *pDstLen, int factor, int *pPhase) {
    int outIdx = 0;
    int phase = *pPhase;
    for (int i = phase; i < srcLen; i += factor) {
        pDst[outIdx++] = pSrc[i];
    }
    *pDstLen = outIdx;
    *pPhase = (phase + factor - (srcLen % factor)) % factor;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSampleDown_64f(const Ipp64f *pSrc, int srcLen, Ipp64f *pDst, int *pDstLen, int factor, int *pPhase) {
    int outIdx = 0;
    int phase = *pPhase;
    for (int i = phase; i < srcLen; i += factor) {
        pDst[outIdx++] = pSrc[i];
    }
    *pDstLen = outIdx;
    *pPhase = (phase + factor - (srcLen % factor)) % factor;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSampleUp_32f(const Ipp32f *pSrc, int srcLen, Ipp32f *pDst, int *pDstLen, int factor, int *pPhase) {
    int dstLen = srcLen * factor;
    std::memset(pDst, 0, dstLen * sizeof(float));
    int phase = *pPhase;
    for (int i = 0; i < srcLen; ++i) {
        pDst[i * factor + phase] = pSrc[i];
    }
    *pDstLen = dstLen;
    return ippStsNoErr;
}

extern "C" IppStatus ippsSampleUp_64f(const Ipp64f *pSrc, int srcLen, Ipp64f *pDst, int *pDstLen, int factor, int *pPhase) {
    int dstLen = srcLen * factor;
    std::memset(pDst, 0, dstLen * sizeof(double));
    int phase = *pPhase;
    for (int i = 0; i < srcLen; ++i) {
        pDst[i * factor + phase] = pSrc[i];
    }
    *pDstLen = dstLen;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_32fc_I(Ipp32fc *pSrcDst, int len) {
    std::reverse(pSrcDst, pSrcDst + len);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_64fc_I(Ipp64fc *pSrcDst, int len) {
    std::reverse(pSrcDst, pSrcDst + len);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_32f(const Ipp32f *pSrc, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[len - 1 - i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_64f(const Ipp64f *pSrc, Ipp64f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[len - 1 - i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[len - 1 - i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsFlip_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = pSrc[len - 1 - i];
    return ippStsNoErr;
}

extern "C" IppStatus ippsConj_32fc_I(Ipp32fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i].im = -pSrcDst[i].im;
    return ippStsNoErr;
}

extern "C" IppStatus ippsConj_64fc_I(Ipp64fc *pSrcDst, int len) {
    for (int i = 0; i < len; ++i) pSrcDst[i].im = -pSrcDst[i].im;
    return ippStsNoErr;
}

extern "C" IppStatus ippsMove_32f(const Ipp32f *pSrc, Ipp32f *pDst, int len) {
    std::memmove(pDst, pSrc, len * sizeof(float));
    return ippStsNoErr;
}

extern "C" IppStatus ippsMove_64f(const Ipp64f *pSrc, Ipp64f *pDst, int len) {
    std::memmove(pDst, pSrc, len * sizeof(double));
    return ippStsNoErr;
}

extern "C" IppStatus ippsMove_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int len) {
    std::memmove(pDst, pSrc, len * sizeof(Ipp32fc));
    return ippStsNoErr;
}

extern "C" IppStatus ippsZero_32f(Ipp32f *pDst, int len) {
    std::memset(pDst, 0, len * sizeof(float));
    return ippStsNoErr;
}

extern "C" IppStatus ippsZero_32fc(Ipp32fc *pDst, int len) {
    std::memset(pDst, 0, len * sizeof(Ipp32fc));
    return ippStsNoErr;
}

extern "C" IppStatus ippsDotProd_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDp) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += (double)pSrc1[i] * (double)pSrc2[i];
    *pDp = (float)sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDotProd_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDp) {
    double sum = 0.0;
    for (int i = 0; i < len; ++i) sum += pSrc1[i] * pSrc2[i];
    *pDp = sum;
    return ippStsNoErr;
}

extern "C" IppStatus ippsReplaceNAN_32f_I(Ipp32f *pSrcDst, int len, Ipp32f val) {
    for (int i = 0; i < len; ++i) {
        if (std::isnan(pSrcDst[i])) pSrcDst[i] = val;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsThreshold_32f_I(Ipp32f *pSrcDst, int len, Ipp32f level, IppCmpOp relOp) {
    if (relOp == ippCmpLess) {
        for (int i = 0; i < len; ++i) if (pSrcDst[i] < level) pSrcDst[i] = level;
    } else if (relOp == ippCmpGreater) {
        for (int i = 0; i < len; ++i) if (pSrcDst[i] > level) pSrcDst[i] = level;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsThreshold_64f_I(Ipp64f *pSrcDst, int len, Ipp64f level, IppCmpOp relOp) {
    if (relOp == ippCmpLess) {
        for (int i = 0; i < len; ++i) if (pSrcDst[i] < level) pSrcDst[i] = level;
    } else if (relOp == ippCmpGreater) {
        for (int i = 0; i < len; ++i) if (pSrcDst[i] > level) pSrcDst[i] = level;
    }
    return ippStsNoErr;
}

static inline uint16_t float_to_half_compat(float f) {
    uint32_t x;
    std::memcpy(&x, &f, sizeof(float));
    uint32_t sign = (x >> 16) & 0x8000;
    int32_t exp = ((x >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = x & 0x7FFFFF;

    if (exp <= 0) {
        if (exp < -10) return (uint16_t)sign;
        mant = (mant | 0x800000) >> (1 - exp);
        return (uint16_t)(sign | (mant >> 13));
    } else if (exp >= 31) {
        return (uint16_t)(sign | 0x7C00 | (mant ? 0x0200 : 0));
    }
    return (uint16_t)(sign | (exp << 10) | (mant >> 13));
}

static inline float half_to_float_compat(uint16_t h) {
    uint32_t sign = (h & 0x8000) << 16;
    uint32_t exp = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x03FF;
    uint32_t out;
    if (exp == 0) {
        if (mant == 0) {
            out = sign;
        } else {
            exp = 1;
            while ((mant & 0x0400) == 0) {
                mant <<= 1;
                exp--;
            }
            mant &= 0x03FF;
            out = sign | ((exp + 127 - 15) << 23) | (mant << 13);
        }
    } else if (exp == 31) {
        out = sign | 0x7F800000 | (mant << 13);
    } else {
        out = sign | ((exp + 127 - 15) << 23) | (mant << 13);
    }
    float f;
    std::memcpy(&f, &out, sizeof(float));
    return f;
}

extern "C" IppStatus ippsConvert_32f16f(const Ipp32f *pSrc, Ipp16f *pDst, int len, IppRoundMode) {
    for (int i = 0; i < len; ++i) pDst[i] = float_to_half_compat(pSrc[i]);
    return ippStsNoErr;
}

extern "C" IppStatus ippsConvert_16f32f(const Ipp16f *pSrc, Ipp32f *pDst, int len) {
    for (int i = 0; i < len; ++i) pDst[i] = half_to_float_compat(pSrc[i]);
    return ippStsNoErr;
}

extern "C" IppStatus ippsRealToCplx_32f(const Ipp32f *pSrcRe, const Ipp32f *pSrcIm, Ipp32fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        pDst[i].re = pSrcRe[i];
        pDst[i].im = pSrcIm ? pSrcIm[i] : 0.0f;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsRealToCplx_64f(const Ipp64f *pSrcRe, const Ipp64f *pSrcIm, Ipp64fc *pDst, int len) {
    for (int i = 0; i < len; ++i) {
        pDst[i].re = pSrcRe[i];
        pDst[i].im = pSrcIm ? pSrcIm[i] : 0.0;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsCplxToReal_32fc(const Ipp32fc *pSrc, Ipp32f *pDstRe, Ipp32f *pDstIm, int len) {
    for (int i = 0; i < len; ++i) {
        if (pDstRe) pDstRe[i] = pSrc[i].re;
        if (pDstIm) pDstIm[i] = pSrc[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsCplxToReal_64fc(const Ipp64fc *pSrc, Ipp64f *pDstRe, Ipp64f *pDstIm, int len) {
    for (int i = 0; i < len; ++i) {
        if (pDstRe) pDstRe[i] = pSrc[i].re;
        if (pDstIm) pDstIm[i] = pSrc[i].im;
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsConvolveGetBufferSize(int, int, IppDataType, IppHintAlgorithm, int *pBufferSize) {
    if (pBufferSize) *pBufferSize = 64;
    return ippStsNoErr;
}

extern "C" IppStatus ippsConvolve_32f(const Ipp32f *pSrc1, int src1Len, const Ipp32f *pSrc2, int src2Len, Ipp32f *pDst, IppHintAlgorithm, Ipp8u*) {
    int outLen = src1Len + src2Len - 1;
    std::memset(pDst, 0, outLen * sizeof(float));
    for (int i = 0; i < src1Len; ++i) {
        float val1 = pSrc1[i];
        for (int j = 0; j < src2Len; ++j) {
            pDst[i + j] += val1 * pSrc2[j];
        }
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsConvolve_64f(const Ipp64f *pSrc1, int src1Len, const Ipp64f *pSrc2, int src2Len, Ipp64f *pDst, IppHintAlgorithm, Ipp8u*) {
    int outLen = src1Len + src2Len - 1;
    std::memset(pDst, 0, outLen * sizeof(double));
    for (int i = 0; i < src1Len; ++i) {
        double val1 = pSrc1[i];
        for (int j = 0; j < src2Len; ++j) {
            pDst[i + j] += val1 * pSrc2[j];
        }
    }
    return ippStsNoErr;
}

/* DFT / FFT structures */
struct IppsDFTSpec_R_32f { int len; };
struct IppsDFTSpec_R_64f { int len; };
struct IppsDFTSpec_C_32fc { int len; };
struct IppsDFTSpec_C_64fc { int len; };
struct IppsFFTSpec_R_32f { int order; int len; };
struct IppsFFTSpec_R_64f { int order; int len; };
struct IppsFFTSpec_C_32fc { int order; int len; };
struct IppsFFTSpec_C_64fc { int order; int len; };

struct IppiDFTSpec_C_32fc { IppiSize size; };
struct IppiDCTFwdSpec_32f { IppiSize size; };

/* Real DFT 32f */
extern "C" IppStatus ippsDFTGetSize_R_32f(int len, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppsDFTSpec_R_32f);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = (len + 2) * sizeof(float);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInit_R_32f(int len, int, IppHintAlgorithm, IppsDFTSpec_R_32f *pSpec, Ipp8u*) {
    if (pSpec) pSpec->len = len;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToCCS_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToPerm_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)ccs, 1.0f, 1);

    pDst[0] = ccs[0];
    pDst[1] = ccs[len];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k]     = ccs[2 * k];
        pDst[2 * k + 1] = ccs[2 * k + 1];
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToPack_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)ccs, 1.0f, 1);

    pDst[0] = ccs[0];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k - 1] = ccs[2 * k];
        pDst[2 * k]     = ccs[2 * k + 1];
    }
    pDst[len - 1] = ccs[len];
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_CCSToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)pSrc, pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_PermToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0f;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k];
        ccs[2 * k + 1] = pSrc[2 * k + 1];
    }
    ccs[len]     = pSrc[1];
    ccs[len + 1] = 0.0f;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)ccs, pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_PackToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0f;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k - 1];
        ccs[2 * k + 1] = pSrc[2 * k];
    }
    ccs[len]     = pSrc[len - 1];
    ccs[len + 1] = 0.0f;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)ccs, pDst, 1.0f, 1);
    return ippStsNoErr;
}

/* Real DFT 64f */
extern "C" IppStatus ippsDFTGetSize_R_64f(int len, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppsDFTSpec_R_64f);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = (len + 2) * sizeof(double);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInit_R_64f(int len, int, IppHintAlgorithm, IppsDFTSpec_R_64f *pSpec, Ipp8u*) {
    if (pSpec) pSpec->len = len;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToCCS_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToPerm_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)ccs, 1.0, 1);

    pDst[0] = ccs[0];
    pDst[1] = ccs[len];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k]     = ccs[2 * k];
        pDst[2 * k + 1] = ccs[2 * k + 1];
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_RToPack_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)ccs, 1.0, 1);

    pDst[0] = ccs[0];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k - 1] = ccs[2 * k];
        pDst[2 * k]     = ccs[2 * k + 1];
    }
    pDst[len - 1] = ccs[len];
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_CCSToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)pSrc, pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_PermToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k];
        ccs[2 * k + 1] = pSrc[2 * k + 1];
    }
    ccs[len]     = pSrc[1];
    ccs[len + 1] = 0.0;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)ccs, pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_PackToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k - 1];
        ccs[2 * k + 1] = pSrc[2 * k];
    }
    ccs[len]     = pSrc[len - 1];
    ccs[len + 1] = 0.0;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)ccs, pDst, 1.0, 1);
    return ippStsNoErr;
}

/* Real FFT 32f */
extern "C" IppStatus ippsFFTGetSize_R_32f(int order, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    int len = 1 << order;
    if (pSpecSize) *pSpecSize = sizeof(IppsFFTSpec_R_32f);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = (len + 2) * sizeof(float);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInit_R_32f(IppsFFTSpec_R_32f **ppSpec, int order, int, IppHintAlgorithm, Ipp8u *pSpec, Ipp8u*) {
    auto *s = (IppsFFTSpec_R_32f*)pSpec;
    s->order = order;
    s->len = 1 << order;
    if (ppSpec) *ppSpec = s;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToCCS_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPerm_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)ccs, 1.0f, 1);

    pDst[0] = ccs[0];
    pDst[1] = ccs[len];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k]     = ccs[2 * k];
        pDst[2 * k + 1] = ccs[2 * k + 1];
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPerm_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTFwd_RToPerm_32f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTFwd_RToPack_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(float) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<float>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<float>*)ccs, 1.0f, 1);

    pDst[0] = ccs[0];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k - 1] = ccs[2 * k];
        pDst[2 * k]     = ccs[2 * k + 1];
    }
    pDst[len - 1] = ccs[len];
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPack_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTFwd_RToPack_32f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTInv_CCSToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)pSrc, pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PermToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0f;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k];
        ccs[2 * k + 1] = pSrc[2 * k + 1];
    }
    ccs[len]     = pSrc[1];
    ccs[len + 1] = 0.0f;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)ccs, pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PermToR_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTInv_PermToR_32f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTInv_PackToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    float *ccs = (float*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0f;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k - 1];
        ccs[2 * k + 1] = pSrc[2 * k];
    }
    ccs[len]     = pSrc[len - 1];
    ccs[len + 1] = 0.0f;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ sizeof(float) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<float>*)ccs, pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PackToR_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTInv_PackToR_32f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

/* Real FFT 64f */
extern "C" IppStatus ippsFFTGetSize_R_64f(int order, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    int len = 1 << order;
    if (pSpecSize) *pSpecSize = sizeof(IppsFFTSpec_R_64f);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = (len + 2) * sizeof(double);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInit_R_64f(IppsFFTSpec_R_64f **ppSpec, int order, int, IppHintAlgorithm, Ipp8u *pSpec, Ipp8u*) {
    auto *s = (IppsFFTSpec_R_64f*)pSpec;
    s->order = order;
    s->len = 1 << order;
    if (ppSpec) *ppSpec = s;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToCCS_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPerm_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)ccs, 1.0, 1);

    pDst[0] = ccs[0];
    pDst[1] = ccs[len];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k]     = ccs[2 * k];
        pDst[2 * k + 1] = ccs[2 * k + 1];
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPerm_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTFwd_RToPerm_64f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTFwd_RToPack_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    pocketfft::shape_t shape_in{ len };
    pocketfft::stride_t stride_in{ sizeof(double) };
    pocketfft::stride_t stride_out{ sizeof(std::complex<double>) };
    pocketfft::r2c(shape_in, stride_in, stride_out, 0, true, pSrc, (std::complex<double>*)ccs, 1.0, 1);

    pDst[0] = ccs[0];
    for (size_t k = 1; k < len / 2; ++k) {
        pDst[2 * k - 1] = ccs[2 * k];
        pDst[2 * k]     = ccs[2 * k + 1];
    }
    pDst[len - 1] = ccs[len];
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTFwd_RToPack_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTFwd_RToPack_64f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTInv_CCSToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)pSrc, pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PermToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k];
        ccs[2 * k + 1] = pSrc[2 * k + 1];
    }
    ccs[len]     = pSrc[1];
    ccs[len + 1] = 0.0;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)ccs, pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PermToR_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTInv_PermToR_64f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

extern "C" IppStatus ippsFFTInv_PackToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    size_t len = pSpec->len;
    double *ccs = (double*)pBuffer;
    ccs[0] = pSrc[0];
    ccs[1] = 0.0;
    for (size_t k = 1; k < len / 2; ++k) {
        ccs[2 * k]     = pSrc[2 * k - 1];
        ccs[2 * k + 1] = pSrc[2 * k];
    }
    ccs[len]     = pSrc[len - 1];
    ccs[len + 1] = 0.0;

    pocketfft::shape_t shape_out{ len };
    pocketfft::stride_t stride_in{ sizeof(std::complex<double>) };
    pocketfft::stride_t stride_out{ sizeof(double) };
    pocketfft::c2r(shape_out, stride_in, stride_out, 0, false, (const std::complex<double>*)ccs, pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsFFTInv_PackToR_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer) {
    return ippsFFTInv_PackToR_64f(pSrcDst, pSrcDst, pSpec, pBuffer);
}

/* Complex DFT 32fc & 64fc */
extern "C" IppStatus ippsDFTGetSize_C_32fc(int len, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppsDFTSpec_C_32fc);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = 0;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTGetSize_C_64fc(int len, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppsDFTSpec_C_64fc);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = 0;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInit_C_32fc(int len, int, IppHintAlgorithm, IppsDFTSpec_C_32fc *pSpec, Ipp8u*) {
    if (pSpec) pSpec->len = len;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInit_C_64fc(int len, int, IppHintAlgorithm, IppsDFTSpec_C_64fc *pSpec, Ipp8u*) {
    if (pSpec) pSpec->len = len;
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_CToC_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, const IppsDFTSpec_C_32fc *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape{ len };
    pocketfft::stride_t stride{ sizeof(std::complex<float>) };
    pocketfft::shape_t axes{ 0 };
    pocketfft::c2c(shape, stride, stride, axes, true, (const std::complex<float>*)pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTFwd_CToC_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, const IppsDFTSpec_C_64fc *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape{ len };
    pocketfft::stride_t stride{ sizeof(std::complex<double>) };
    pocketfft::shape_t axes{ 0 };
    pocketfft::c2c(shape, stride, stride, axes, true, (const std::complex<double>*)pSrc, (std::complex<double>*)pDst, 1.0, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_CToC_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, const IppsDFTSpec_C_32fc *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape{ len };
    pocketfft::stride_t stride{ sizeof(std::complex<float>) };
    pocketfft::shape_t axes{ 0 };
    pocketfft::c2c(shape, stride, stride, axes, false, (const std::complex<float>*)pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippsDFTInv_CToC_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, const IppsDFTSpec_C_64fc *pSpec, Ipp8u*) {
    size_t len = pSpec->len;
    pocketfft::shape_t shape{ len };
    pocketfft::stride_t stride{ sizeof(std::complex<double>) };
    pocketfft::shape_t axes{ 0 };
    pocketfft::c2c(shape, stride, stride, axes, false, (const std::complex<double>*)pSrc, (std::complex<double>*)pDst, 1.0, 1);
    return ippStsNoErr;
}

/* 2D DFT / DCT stubs */
extern "C" IppStatus ippiDFTGetSize_C_32fc(IppiSize size, int, IppHintAlgorithm, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppiDFTSpec_C_32fc);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = 0;
    return ippStsNoErr;
}

extern "C" IppStatus ippiDFTInit_C_32fc(IppiSize size, int, IppHintAlgorithm, IppiDFTSpec_C_32fc *pSpec, Ipp8u*) {
    if (pSpec) pSpec->size = size;
    return ippStsNoErr;
}

extern "C" IppStatus ippiDFTFwd_CToC_32fc_C1R(const Ipp32fc *pSrc, int srcStep, Ipp32fc *pDst, int dstStep, const IppiDFTSpec_C_32fc *pSpec, Ipp8u*) {
    size_t h = pSpec->size.height;
    size_t w = pSpec->size.width;
    pocketfft::shape_t shape{ h, w };
    pocketfft::stride_t stride_in{ (ptrdiff_t)srcStep, (ptrdiff_t)sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ (ptrdiff_t)dstStep, (ptrdiff_t)sizeof(std::complex<float>) };
    pocketfft::shape_t axes{ 0, 1 };
    pocketfft::c2c(shape, stride_in, stride_out, axes, true, (const std::complex<float>*)pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippiDFTInv_CToC_32fc_C1R(const Ipp32fc *pSrc, int srcStep, Ipp32fc *pDst, int dstStep, const IppiDFTSpec_C_32fc *pSpec, Ipp8u*) {
    size_t h = pSpec->size.height;
    size_t w = pSpec->size.width;
    pocketfft::shape_t shape{ h, w };
    pocketfft::stride_t stride_in{ (ptrdiff_t)srcStep, (ptrdiff_t)sizeof(std::complex<float>) };
    pocketfft::stride_t stride_out{ (ptrdiff_t)dstStep, (ptrdiff_t)sizeof(std::complex<float>) };
    pocketfft::shape_t axes{ 0, 1 };
    pocketfft::c2c(shape, stride_in, stride_out, axes, false, (const std::complex<float>*)pSrc, (std::complex<float>*)pDst, 1.0f, 1);
    return ippStsNoErr;
}

extern "C" IppStatus ippiDCTFwdGetSize_32f(IppiSize size, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppiDCTFwdSpec_32f);
    if (pSpecBufferSize) *pSpecBufferSize = 0;
    if (pBufferSize) *pBufferSize = 0;
    return ippStsNoErr;
}

extern "C" IppStatus ippiDCTFwdInit_32f(IppiDCTFwdSpec_32f *pSpec, IppiSize size, Ipp8u*) {
    if (pSpec) pSpec->size = size;
    return ippStsNoErr;
}

extern "C" IppStatus ippiDCTFwd_32f_C1R(const Ipp32f *pSrc, int srcStep, Ipp32f *pDst, int dstStep, const IppiDCTFwdSpec_32f *pSpec, Ipp8u*) {
    size_t h = pSpec->size.height;
    size_t w = pSpec->size.width;
    pocketfft::shape_t shape{ h, w };
    pocketfft::stride_t stride_in{ (ptrdiff_t)srcStep, (ptrdiff_t)sizeof(float) };
    pocketfft::stride_t stride_out{ (ptrdiff_t)dstStep, (ptrdiff_t)sizeof(float) };
    pocketfft::shape_t axes{ 0, 1 };
    pocketfft::dct(shape, stride_in, stride_out, axes, 2, pSrc, pDst, 1.0f, 1);
    return ippStsNoErr;
}

/* Multirate FIR */
template <typename T>
struct FIRSpecImpl {
    std::vector<T> taps;
    int upFactor = 1;
    int upPhase = 0;
    int downFactor = 1;
    int downPhase = 0;
};

struct IppsFIRSpec_32f : FIRSpecImpl<float> {};
struct IppsFIRSpec_64f : FIRSpecImpl<double> {};
struct IppsFIRSpec_32fc : FIRSpecImpl<Ipp32fc> {};
struct IppsFIRSpec_64fc : FIRSpecImpl<Ipp64fc> {};

extern "C" IppStatus ippsFIRMRGetSize(int, int, int, IppDataType, int *pSpecSize, int *pBufSize) {
    if (pSpecSize) *pSpecSize = sizeof(IppsFIRSpec_64fc);
    if (pBufSize) *pBufSize = 64;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMRInit_32f(const Ipp32f *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_32f *pSpec) {
    pSpec->taps.assign(pTaps, pTaps + tapsLen);
    pSpec->upFactor = upFactor;
    pSpec->upPhase = upPhase;
    pSpec->downFactor = downFactor;
    pSpec->downPhase = downPhase;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMRInit_64f(const Ipp64f *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_64f *pSpec) {
    pSpec->taps.assign(pTaps, pTaps + tapsLen);
    pSpec->upFactor = upFactor;
    pSpec->upPhase = upPhase;
    pSpec->downFactor = downFactor;
    pSpec->downPhase = downPhase;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMRInit_32fc(const Ipp32fc *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_32fc *pSpec) {
    pSpec->taps.assign(pTaps, pTaps + tapsLen);
    pSpec->upFactor = upFactor;
    pSpec->upPhase = upPhase;
    pSpec->downFactor = downFactor;
    pSpec->downPhase = downPhase;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMRInit_64fc(const Ipp64fc *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_64fc *pSpec) {
    pSpec->taps.assign(pTaps, pTaps + tapsLen);
    pSpec->upFactor = upFactor;
    pSpec->upPhase = upPhase;
    pSpec->downFactor = downFactor;
    pSpec->downPhase = downPhase;
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMR_32f(const Ipp32f *pSrc, Ipp32f *pDst, int numIters, IppsFIRSpec_32f *pSpec, const Ipp32f *pDlySrc, Ipp32f *pDlyDst, Ipp8u*) {
    int L = (int)pSpec->taps.size();
    int U = pSpec->upFactor;
    int D = pSpec->downFactor;
    int srcSamples = numIters * D;
    std::vector<float> hist(L + srcSamples, 0.0f);
    if (pDlySrc) std::memcpy(hist.data(), pDlySrc, L * sizeof(float));
    std::memcpy(hist.data() + L, pSrc, srcSamples * sizeof(float));

    for (int p = 0; p < numIters; ++p) {
        int m = p * D;
        float acc = 0.0f;
        for (int k = 0; k < L; ++k) {
            int timeUp = m - k;
            if (timeUp % U == 0) {
                int inIdx = timeUp / U + L;
                if (inIdx >= 0 && inIdx < (int)hist.size()) {
                    acc += pSpec->taps[k] * hist[inIdx];
                }
            }
        }
        pDst[p] = acc;
    }
    if (pDlyDst && (int)hist.size() >= L) {
        std::memcpy(pDlyDst, hist.data() + hist.size() - L, L * sizeof(float));
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMR_64f(const Ipp64f *pSrc, Ipp64f *pDst, int numIters, IppsFIRSpec_64f *pSpec, const Ipp64f *pDlySrc, Ipp64f *pDlyDst, Ipp8u*) {
    int L = (int)pSpec->taps.size();
    int U = pSpec->upFactor;
    int D = pSpec->downFactor;
    int srcSamples = numIters * D;
    std::vector<double> hist(L + srcSamples, 0.0);
    if (pDlySrc) std::memcpy(hist.data(), pDlySrc, L * sizeof(double));
    std::memcpy(hist.data() + L, pSrc, srcSamples * sizeof(double));

    for (int p = 0; p < numIters; ++p) {
        int m = p * D;
        double acc = 0.0;
        for (int k = 0; k < L; ++k) {
            int timeUp = m - k;
            if (timeUp % U == 0) {
                int inIdx = timeUp / U + L;
                if (inIdx >= 0 && inIdx < (int)hist.size()) {
                    acc += pSpec->taps[k] * hist[inIdx];
                }
            }
        }
        pDst[p] = acc;
    }
    if (pDlyDst && (int)hist.size() >= L) {
        std::memcpy(pDlyDst, hist.data() + hist.size() - L, L * sizeof(double));
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMR_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int numIters, IppsFIRSpec_32fc *pSpec, const Ipp32fc *pDlySrc, Ipp32fc *pDlyDst, Ipp8u*) {
    int L = (int)pSpec->taps.size();
    int U = pSpec->upFactor;
    int D = pSpec->downFactor;
    int srcSamples = numIters * D;
    std::vector<Ipp32fc> hist(L + srcSamples, {0.0f, 0.0f});
    if (pDlySrc) std::memcpy(hist.data(), pDlySrc, L * sizeof(Ipp32fc));
    std::memcpy(hist.data() + L, pSrc, srcSamples * sizeof(Ipp32fc));

    for (int p = 0; p < numIters; ++p) {
        int m = p * D;
        float acc_re = 0.0f, acc_im = 0.0f;
        for (int k = 0; k < L; ++k) {
            int timeUp = m - k;
            if (timeUp % U == 0) {
                int inIdx = timeUp / U + L;
                if (inIdx >= 0 && inIdx < (int)hist.size()) {
                    float a = pSpec->taps[k].re, b = pSpec->taps[k].im;
                    float c = hist[inIdx].re, d = hist[inIdx].im;
                    acc_re += a * c - b * d;
                    acc_im += a * d + b * c;
                }
            }
        }
        pDst[p].re = acc_re;
        pDst[p].im = acc_im;
    }
    if (pDlyDst && (int)hist.size() >= L) {
        std::memcpy(pDlyDst, hist.data() + hist.size() - L, L * sizeof(Ipp32fc));
    }
    return ippStsNoErr;
}

extern "C" IppStatus ippsFIRMR_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, int numIters, IppsFIRSpec_64fc *pSpec, const Ipp64fc *pDlySrc, Ipp64fc *pDlyDst, Ipp8u*) {
    int L = (int)pSpec->taps.size();
    int U = pSpec->upFactor;
    int D = pSpec->downFactor;
    int srcSamples = numIters * D;
    std::vector<Ipp64fc> hist(L + srcSamples, {0.0, 0.0});
    if (pDlySrc) std::memcpy(hist.data(), pDlySrc, L * sizeof(Ipp64fc));
    std::memcpy(hist.data() + L, pSrc, srcSamples * sizeof(Ipp64fc));

    for (int p = 0; p < numIters; ++p) {
        int m = p * D;
        double acc_re = 0.0, acc_im = 0.0;
        for (int k = 0; k < L; ++k) {
            int timeUp = m - k;
            if (timeUp % U == 0) {
                int inIdx = timeUp / U + L;
                if (inIdx >= 0 && inIdx < (int)hist.size()) {
                    double a = pSpec->taps[k].re, b = pSpec->taps[k].im;
                    double c = hist[inIdx].re, d = hist[inIdx].im;
                    acc_re += a * c - b * d;
                    acc_im += a * d + b * c;
                }
            }
        }
        pDst[p].re = acc_re;
        pDst[p].im = acc_im;
    }
    if (pDlyDst && (int)hist.size() >= L) {
        std::memcpy(pDlyDst, hist.data() + hist.size() - L, L * sizeof(Ipp64fc));
    }
    return ippStsNoErr;
}

/* Kaiser Window */
static double bessel_i0_compat(double x) {
    double ax = std::abs(x);
    if (ax < 3.75) {
        double y = x / 3.75;
        y = y * y;
        return 1.0 + y * (3.5156229 + y * (3.0899424 + y * (1.2067492 + y * (0.2659732 + y * (0.360768e-1 + y * 0.45813e-2)))));
    } else {
        double y = 3.75 / ax;
        return (std::exp(ax) / std::sqrt(ax)) * (0.39894228 + y * (0.1328592e-1 + y * (0.225319e-2 + y * (-0.157565e-2 + y * (0.916281e-2 + y * (-0.2057706e-1 + y * (0.2635537e-1 + y * (-0.1647633e-1 + y * 0.392377e-2))))))));
    }
}

extern "C" IppStatus ippsWinKaiser_64f_I(Ipp64f *pSrcDst, int len, Ipp64f alpha) {
    if (len <= 1) return ippStsNoErr;
    double beta = alpha * 3.14159265358979323846;
    double denom = bessel_i0_compat(beta);
    for (int n = 0; n < len; ++n) {
        double u = (2.0 * n) / (len - 1.0) - 1.0;
        double val = beta * std::sqrt(std::max(0.0, 1.0 - u * u));
        pSrcDst[n] *= (bessel_i0_compat(val) / denom);
    }
    return ippStsNoErr;
}
