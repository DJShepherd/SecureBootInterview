#define IRAM_BASE     0x70000000
#define STACK_OFFSET  0x10000
#define STACK_SIZE    0x1000
#define STACK_ADDRESS (IRAM_BASE + STACK_OFFSET + STACK_SIZE)
#define BL_BASE       IRAM_BASE

#if defined(DEBUG)
#define ASSERT_FATAL(bCond) BootAssertFatal(bCond)
#else // RELEASE
#define ASSERT_FATAL(bCond)
#endif

typedef struct { // BootLoaderHeader
    uint8_t  blSig[RSA_SIG_SIZE];
    uint8_t  blHash[MD5_HASH_SIZE];
    uint8_t  blKey[AES_KEY_SIZE];
    uint32_t blVersion;
    uint32_t blBlock;
    uint32_t blSize;
    uint32_t blEntryOffset;
    uint8_t  _pad[BLHDR_AES_PAD_SIZE];
} BLHeader;

extern uint32_t  FuseGetVersion(void);
extern void      BootAssertFatal(int bCond);
extern BootError FaultDetectRandomLengthDelay(void);
extern BootError ReadFromEmmc(uint32_t nBlock, void *pDest, uint32_t nSize);
// Compute hash digest and memcmp hash
extern BootError ValidateHash(const void *pHash, const void *pData, uint32_t nSize);
// Compute signature digest and memcmp hash
extern BootError ValidateSig(const void *pSig, const void *pHash);
extern void      AesEcbDecrypt(void *pKey, void *pData, uint32_t nSize);
extern void      _asmJumpToAddress(void *pAddr);

BootError LoadRunBootloader(void) {
    BootError rc;
    BLHeader *pHdr;
    void     *pEntry;
    void     *pBootloader;
    
    // Fault injection protection
    rc = FaultDetectRandomLengthDelay();
    if (rc != BootError_Success) goto _error;
    
    // Read the bootloader header
    pHdr = (BLHeader*)BL_BASE;
    rc   = ReadFromEmmc(0, pHdr, sizeof(*pHdr));
    if (rc != BootError_Success) goto _error;
    
    rc = FaultDetectRandomLengthDelay();
    if (rc != BootError_Success) goto _error;
    
    // Prevent bootloader downgrade
    ASSERT_FATAL(FuseGetVersion() == pHdr->blVersion);
    
    // Read the bootloader payload into memory
    pBootloader = pHdr + 1;
    rc          = ReadFromEmmc(pHdr->blBlock, pBootloader, pHdr->blSize);
    if (rc != BootError_Success) goto _error;
    
    // Validate bootloader integrity
    rc = ValidateHash(pHdr->blHash, pBootloader, pHdr->blSize);
    if (rc != BootError_Success) goto _error;
    
    rc = ValidateSig(pHdr->blSig, pHdr->blHash);
    if (rc != BootError_Success) goto _error;
    
    rc = FaultDetectRandomLengthDelay();
    if (rc != BootError_Success) goto _error;
    
    // Decrypt bootloader
    AesEcbDecrypt(pHdr->blKey, pBootloader, pHdr->blSize);
    
    // Validate entry address
    pEntry = (uint8_t*)pBootloader + pHdr->blEntryOffset;
    BootAssertFatal(
        ((uint32_t)pEntry >= IRAM_BASE) &&
        ((uint32_t)pEntry <  IRAM_END)
    );
    
    // Jump to bootloader
    _asmJumpToAddress(pEntry);
    
_error:
    return rc;
}