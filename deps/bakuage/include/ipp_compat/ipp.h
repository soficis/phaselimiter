#ifndef BAKUAGE_IPP_COMPAT_IPP_H
#define BAKUAGE_IPP_COMPAT_IPP_H

#include <cstdint>
#include <cstddef>

typedef uint8_t  Ipp8u;
typedef int16_t  Ipp16s;
typedef int32_t  Ipp32s;
typedef uint16_t Ipp16u;
typedef uint32_t Ipp32u;
typedef float    Ipp32f;
typedef double   Ipp64f;
typedef uint16_t Ipp16f;

struct Ipp32fc {
    float re;
    float im;
};

struct Ipp64fc {
    double re;
    double im;
};

enum IppStatus {
    ippStsNoErr = 0,
    ippStsNullPtrErr = -8,
    ippStsSizeErr = -7,
    ippStsDivByZeroErr = -6,
    ippStsMemAllocErr = -9,
    ippStsErr = -1
};

enum IppDataType {
    ipp8u = 0,
    ipp16s = 1,
    ipp32s = 2,
    ipp32f = 3,
    ipp64f = 4,
    ipp32fc = 5,
    ipp64fc = 6
};

enum IppCmpOp {
    ippCmpLess = 0,
    ippCmpLessEq = 1,
    ippCmpEq = 2,
    ippCmpGreaterEq = 3,
    ippCmpGreater = 4
};

enum IppRoundMode {
    ippRndNear = 0,
    ippRndZero = 1
};

enum IppHintAlgorithm {
    ippAlgHintNone = 0,
    ippAlgHintFast = 1,
    ippAlgHintAccurate = 2,
    ippAlgAuto = 0
};

#define IPP_FFT_NODIV_BY_ANY 1
#define IPP_FFT_DIV_BY_SQRT  2
#define IPP_FFT_DIV_BY_N     4

struct IppiSize {
    int width;
    int height;
};

struct IppLibraryVersion {
    int major;
    int minor;
    int majorBuild;
    int build;
    char targetCpu[16];
    const char *Name;
    const char *Version;
    const char *BuildDate;
};

struct IppsDFTSpec_R_32f;
struct IppsDFTSpec_R_64f;
struct IppsDFTSpec_C_32fc;
struct IppsDFTSpec_C_64fc;
struct IppsFFTSpec_R_32f;
struct IppsFFTSpec_R_64f;
struct IppsFFTSpec_C_32fc;
struct IppsFFTSpec_C_64fc;

struct IppiDFTSpec_C_32fc;
struct IppiDCTFwdSpec_32f;

struct IppsFIRSpec_32f;
struct IppsFIRSpec_64f;
struct IppsFIRSpec_32fc;
struct IppsFIRSpec_64fc;

#ifdef __cplusplus
extern "C" {
#endif

/* System & Library info */
IppStatus ippInit();
const IppLibraryVersion* ippGetLibVersion();

/* Memory allocation */
Ipp8u*  ippsMalloc_8u(int len);
Ipp32f* ippsMalloc_32f(int len);
Ipp64f* ippsMalloc_64f(int len);
Ipp32fc* ippsMalloc_32fc(int len);
Ipp64fc* ippsMalloc_64fc(int len);
void    ippsFree(void *ptr);

/* Vector math */
IppStatus ippsMulC_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pDst, int len);
IppStatus ippsMulC_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len);
IppStatus ippsMulC_32fc_I(Ipp32fc val, Ipp32fc *pSrcDst, int len);
IppStatus ippsMulC_64f(const Ipp64f *pSrc, Ipp64f val, Ipp64f *pDst, int len);
IppStatus ippsMulC_64f_I(Ipp64f val, Ipp64f *pSrcDst, int len);
IppStatus ippsMulC_64fc_I(Ipp64fc val, Ipp64fc *pSrcDst, int len);
IppStatus ippsMul_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pDst, int len);
IppStatus ippsMul_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len);
IppStatus ippsMul_32f32fc(const Ipp32f *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len);
IppStatus ippsMul_32f32fc_I(const Ipp32f *pSrc, Ipp32fc *pSrcDst, int len);
IppStatus ippsMul_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len);
IppStatus ippsMul_32fc_I(const Ipp32fc *pSrc, Ipp32fc *pSrcDst, int len);
IppStatus ippsMul_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pDst, int len);
IppStatus ippsMul_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len);
IppStatus ippsMul_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len);
IppStatus ippsMul_64fc_I(const Ipp64fc *pSrc, Ipp64fc *pSrcDst, int len);
IppStatus ippsMulPerm_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len);
IppStatus ippsMulPerm_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len);
IppStatus ippsMulByConj_32fc_A24(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len);
IppStatus ippsMulByConj_64fc_A53(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len);
IppStatus ippsAddC_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len);
IppStatus ippsAddC_64f_I(Ipp64f val, Ipp64f *pSrcDst, int len);
IppStatus ippsAdd_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len);
IppStatus ippsAdd_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len);
IppStatus ippsAdd_32fc_I(const Ipp32fc *pSrc, Ipp32fc *pSrcDst, int len);
IppStatus ippsAdd_64fc_I(const Ipp64fc *pSrc, Ipp64fc *pSrcDst, int len);
IppStatus ippsAdd_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pDst, int len);
IppStatus ippsAdd_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pDst, int len);
IppStatus ippsAdd_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pDst, int len);
IppStatus ippsAdd_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pDst, int len);
IppStatus ippsSubCRev_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pDst, int len);
IppStatus ippsDiv_32f_I(const Ipp32f *pSrc, Ipp32f *pSrcDst, int len);
IppStatus ippsDiv_64f_I(const Ipp64f *pSrc, Ipp64f *pSrcDst, int len);
IppStatus ippsAddProduct_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, Ipp32f *pSrcDst, int len);
IppStatus ippsAddProduct_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, Ipp64f *pSrcDst, int len);
IppStatus ippsAddProduct_32fc(const Ipp32fc *pSrc1, const Ipp32fc *pSrc2, Ipp32fc *pSrcDst, int len);
IppStatus ippsAddProduct_64fc(const Ipp64fc *pSrc1, const Ipp64fc *pSrc2, Ipp64fc *pSrcDst, int len);
IppStatus ippsAddProductC_32f(const Ipp32f *pSrc, Ipp32f val, Ipp32f *pSrcDst, int len);
IppStatus ippsAddProductC_64f(const Ipp64f *pSrc, Ipp64f val, Ipp64f *pSrcDst, int len);
IppStatus ippsPowx_32f_A24(const Ipp32f *pSrc1, const Ipp32f val, Ipp32f *pDst, int len);
IppStatus ippsPowx_64f_A53(const Ipp64f *pSrc1, const Ipp64f val, Ipp64f *pDst, int len);
IppStatus ippsSqrt_32f_I(Ipp32f *pSrcDst, int len);
IppStatus ippsSqrt_64f_I(Ipp64f *pSrcDst, int len);
IppStatus ippsPowerSpectr_32fc(const Ipp32fc *pSrc, Ipp32f *pDst, int len);
IppStatus ippsPowerSpectr_64fc(const Ipp64fc *pSrc, Ipp64f *pDst, int len);
IppStatus ippsNormDiff_L1_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff);
IppStatus ippsNormDiff_L1_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff);
IppStatus ippsNormDiff_L2_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff);
IppStatus ippsNormDiff_L2_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff);
IppStatus ippsNormDiff_Inf_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDiff);
IppStatus ippsNormDiff_Inf_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDiff);
IppStatus ippsNorm_Inf_32f(const Ipp32f *pSrc, int len, Ipp32f *pNorm);
IppStatus ippsNorm_Inf_64f(const Ipp64f *pSrc, int len, Ipp64f *pNorm);
IppStatus ippsNorm_L2_32f(const Ipp32f *pSrc, int len, Ipp32f *pNorm);
IppStatus ippsNorm_L2_64f(const Ipp64f *pSrc, int len, Ipp64f *pNorm);
IppStatus ippsSum_32f(const Ipp32f *pSrc, int len, Ipp32f *pSum, IppHintAlgorithm hint);
IppStatus ippsSum_64f(const Ipp64f *pSrc, int len, Ipp64f *pSum);
IppStatus ippsDivCRev_32f_I(Ipp32f val, Ipp32f *pSrcDst, int len);
IppStatus ippsSet_32f(Ipp32f val, Ipp32f *pDst, int len);
IppStatus ippsSet_64f(Ipp64f val, Ipp64f *pDst, int len);
IppStatus ippsSet_32s(Ipp32s val, Ipp32s *pDst, int len);
IppStatus ippsSampleDown_32f(const Ipp32f *pSrc, int srcLen, Ipp32f *pDst, int *pDstLen, int factor, int *pPhase);
IppStatus ippsSampleDown_64f(const Ipp64f *pSrc, int srcLen, Ipp64f *pDst, int *pDstLen, int factor, int *pPhase);
IppStatus ippsSampleUp_32f(const Ipp32f *pSrc, int srcLen, Ipp32f *pDst, int *pDstLen, int factor, int *pPhase);
IppStatus ippsSampleUp_64f(const Ipp64f *pSrc, int srcLen, Ipp64f *pDst, int *pDstLen, int factor, int *pPhase);
IppStatus ippsFlip_32fc_I(Ipp32fc *pSrcDst, int len);
IppStatus ippsFlip_64fc_I(Ipp64fc *pSrcDst, int len);
IppStatus ippsFlip_32f(const Ipp32f *pSrc, Ipp32f *pDst, int len);
IppStatus ippsFlip_64f(const Ipp64f *pSrc, Ipp64f *pDst, int len);
IppStatus ippsFlip_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int len);
IppStatus ippsFlip_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, int len);
IppStatus ippsConj_32fc_I(Ipp32fc *pSrcDst, int len);
IppStatus ippsConj_64fc_I(Ipp64fc *pSrcDst, int len);
IppStatus ippsMove_32f(const Ipp32f *pSrc, Ipp32f *pDst, int len);
IppStatus ippsMove_64f(const Ipp64f *pSrc, Ipp64f *pDst, int len);
IppStatus ippsMove_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int len);
IppStatus ippsZero_32f(Ipp32f *pDst, int len);
IppStatus ippsZero_32fc(Ipp32fc *pDst, int len);
IppStatus ippsDotProd_32f(const Ipp32f *pSrc1, const Ipp32f *pSrc2, int len, Ipp32f *pDp);
IppStatus ippsDotProd_64f(const Ipp64f *pSrc1, const Ipp64f *pSrc2, int len, Ipp64f *pDp);
IppStatus ippsReplaceNAN_32f_I(Ipp32f *pSrcDst, int len, Ipp32f val);
IppStatus ippsThreshold_32f_I(Ipp32f *pSrcDst, int len, Ipp32f level, IppCmpOp relOp);
IppStatus ippsThreshold_64f_I(Ipp64f *pSrcDst, int len, Ipp64f level, IppCmpOp relOp);
IppStatus ippsConvert_32f16f(const Ipp32f *pSrc, Ipp16f *pDst, int len, IppRoundMode rndMode);
IppStatus ippsConvert_16f32f(const Ipp16f *pSrc, Ipp32f *pDst, int len);
IppStatus ippsRealToCplx_32f(const Ipp32f *pSrcRe, const Ipp32f *pSrcIm, Ipp32fc *pDst, int len);
IppStatus ippsRealToCplx_64f(const Ipp64f *pSrcRe, const Ipp64f *pSrcIm, Ipp64fc *pDst, int len);
IppStatus ippsCplxToReal_32fc(const Ipp32fc *pSrc, Ipp32f *pDstRe, Ipp32f *pDstIm, int len);
IppStatus ippsCplxToReal_64fc(const Ipp64fc *pSrc, Ipp64f *pDstRe, Ipp64f *pDstIm, int len);
IppStatus ippsConvolveGetBufferSize(int src1Len, int src2Len, IppDataType dataType, IppHintAlgorithm alg, int *pBufferSize);
IppStatus ippsConvolve_32f(const Ipp32f *pSrc1, int src1Len, const Ipp32f *pSrc2, int src2Len, Ipp32f *pDst, IppHintAlgorithm alg, Ipp8u *pBuffer);
IppStatus ippsConvolve_64f(const Ipp64f *pSrc1, int src1Len, const Ipp64f *pSrc2, int src2Len, Ipp64f *pDst, IppHintAlgorithm alg, Ipp8u *pBuffer);

/* DFT / FFT */
IppStatus ippsDFTGetSize_R_32f(int len, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsDFTGetSize_R_64f(int len, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsDFTInit_R_32f(int len, int flag, IppHintAlgorithm hint, IppsDFTSpec_R_32f *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsDFTInit_R_64f(int len, int flag, IppHintAlgorithm hint, IppsDFTSpec_R_64f *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsDFTFwd_RToCCS_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_RToCCS_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_RToPerm_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_RToPerm_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_RToPack_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_RToPack_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_CCSToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_CCSToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_PermToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_PermToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_PackToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsDFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_PackToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsDFTSpec_R_64f *pSpec, Ipp8u *pBuffer);

IppStatus ippsFFTGetSize_R_32f(int order, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsFFTGetSize_R_64f(int order, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsFFTInit_R_32f(IppsFFTSpec_R_32f **ppSpec, int order, int flag, IppHintAlgorithm hint, Ipp8u *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsFFTInit_R_64f(IppsFFTSpec_R_64f **ppSpec, int order, int flag, IppHintAlgorithm hint, Ipp8u *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsFFTFwd_RToCCS_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToCCS_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPerm_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPerm_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPerm_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPerm_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPack_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPack_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPack_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTFwd_RToPack_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_CCSToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_CCSToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PermToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PermToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PermToR_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PermToR_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PackToR_32f(const Ipp32f *pSrc, Ipp32f *pDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PackToR_64f(const Ipp64f *pSrc, Ipp64f *pDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PackToR_32f_I(Ipp32f *pSrcDst, const IppsFFTSpec_R_32f *pSpec, Ipp8u *pBuffer);
IppStatus ippsFFTInv_PackToR_64f_I(Ipp64f *pSrcDst, const IppsFFTSpec_R_64f *pSpec, Ipp8u *pBuffer);

IppStatus ippsDFTGetSize_C_32fc(int len, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsDFTGetSize_C_64fc(int len, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippsDFTInit_C_32fc(int len, int flag, IppHintAlgorithm hint, IppsDFTSpec_C_32fc *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsDFTInit_C_64fc(int len, int flag, IppHintAlgorithm hint, IppsDFTSpec_C_64fc *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippsDFTFwd_CToC_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, const IppsDFTSpec_C_32fc *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTFwd_CToC_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, const IppsDFTSpec_C_64fc *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_CToC_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, const IppsDFTSpec_C_32fc *pSpec, Ipp8u *pBuffer);
IppStatus ippsDFTInv_CToC_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, const IppsDFTSpec_C_64fc *pSpec, Ipp8u *pBuffer);

/* 2D DFT / DCT */
IppStatus ippiDFTGetSize_C_32fc(IppiSize size, int flag, IppHintAlgorithm hint, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippiDFTInit_C_32fc(IppiSize size, int flag, IppHintAlgorithm hint, IppiDFTSpec_C_32fc *pSpec, Ipp8u *pSpecBuffer);
IppStatus ippiDFTFwd_CToC_32fc_C1R(const Ipp32fc *pSrc, int srcStep, Ipp32fc *pDst, int dstStep, const IppiDFTSpec_C_32fc *pSpec, Ipp8u *pBuffer);
IppStatus ippiDFTInv_CToC_32fc_C1R(const Ipp32fc *pSrc, int srcStep, Ipp32fc *pDst, int dstStep, const IppiDFTSpec_C_32fc *pSpec, Ipp8u *pBuffer);

IppStatus ippiDCTFwdGetSize_32f(IppiSize size, int *pSpecSize, int *pSpecBufferSize, int *pBufferSize);
IppStatus ippiDCTFwdInit_32f(IppiDCTFwdSpec_32f *pSpec, IppiSize size, Ipp8u *pSpecBuffer);
IppStatus ippiDCTFwd_32f_C1R(const Ipp32f *pSrc, int srcStep, Ipp32f *pDst, int dstStep, const IppiDCTFwdSpec_32f *pSpec, Ipp8u *pBuffer);

/* FIR Multirate */
IppStatus ippsFIRMRGetSize(int tapsLen, int upFactor, int downFactor, IppDataType dataType, int *pSpecSize, int *pBufSize);
IppStatus ippsFIRMRInit_32f(const Ipp32f *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_32f *pSpec);
IppStatus ippsFIRMRInit_64f(const Ipp64f *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_64f *pSpec);
IppStatus ippsFIRMRInit_32fc(const Ipp32fc *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_32fc *pSpec);
IppStatus ippsFIRMRInit_64fc(const Ipp64fc *pTaps, int tapsLen, int upFactor, int upPhase, int downFactor, int downPhase, IppsFIRSpec_64fc *pSpec);
IppStatus ippsFIRMR_32f(const Ipp32f *pSrc, Ipp32f *pDst, int numIters, IppsFIRSpec_32f *pSpec, const Ipp32f *pDlySrc, Ipp32f *pDlyDst, Ipp8u *pBuf);
IppStatus ippsFIRMR_64f(const Ipp64f *pSrc, Ipp64f *pDst, int numIters, IppsFIRSpec_64f *pSpec, const Ipp64f *pDlySrc, Ipp64f *pDlyDst, Ipp8u *pBuf);
IppStatus ippsFIRMR_32fc(const Ipp32fc *pSrc, Ipp32fc *pDst, int numIters, IppsFIRSpec_32fc *pSpec, const Ipp32fc *pDlySrc, Ipp32fc *pDlyDst, Ipp8u *pBuf);
IppStatus ippsFIRMR_64fc(const Ipp64fc *pSrc, Ipp64fc *pDst, int numIters, IppsFIRSpec_64fc *pSpec, const Ipp64fc *pDlySrc, Ipp64fc *pDlyDst, Ipp8u *pBuf);

/* Window functions */
IppStatus ippsWinKaiser_64f_I(Ipp64f *pSrcDst, int len, Ipp64f alpha);

#ifdef __cplusplus
}
#endif

#endif /* BAKUAGE_IPP_COMPAT_IPP_H */
