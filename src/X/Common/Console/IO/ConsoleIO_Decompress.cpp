// Original file: X/Common/Console/IO/ConsoleIO_Decompress.cpp
#include "../../../Sigma/SigmaTypes.h"
#include "ConsoleIO_Decompress.h"

// Blank lines keep the original line numbers.

// Blizzard/Util/Crypto.h:235 and ConsoleIO_Decompress.h:38 define these inline; the adopted
// headers do not have their bodies yet, so they are kept local to this file.
namespace Blizzard { namespace Util {
inline void DecXor64(uint64 uKey, uint8 * pData, uint32 uDataSize) {
    while (uDataSize--) {
        uint8 uByte;
        uByte = *pData ^ (uint8)uKey;
        *pData = uByte;
        uKey = (uKey >> 8) | ((uKey ^ uByte) << 56);
        pData++;
    }
} } }
static const uint64 XOR64_ENCRYPTION_KEY = 0x873ae8e53c11d761ULL;
inline uint32 ZLibPacketHeader::GetDecompSize() const { return dwDecompSizeLow | (dwDecompSizeHigh << 16); }


BOOL SkipDecompressData(z_stream * ptStream, int32 nSize, uint8 * pVoidBuffer, int32 nVoidBufferSize, BOOL fByteSwap, uint32 & dwCurrentPacketRemainingDecompSize, BOOL & fInsidePacketHeader)







{
    int32 nToDecompressInVoid = nSize;

    ZLibPacketHeader tPacketHeader = *(ZLibPacketHeader *)ptStream->next_in;

    ptStream->next_in += sizeof(ZLibPacketHeader);
    ptStream->avail_in -= sizeof(ZLibPacketHeader);











    while ((int32)tPacketHeader.GetDecompSize() <= nToDecompressInVoid) {

        ptStream->next_in += tPacketHeader.tEncryptedAndCompSize.dwCompSize;
        ptStream->avail_in -= tPacketHeader.tEncryptedAndCompSize.dwCompSize;
        nToDecompressInVoid -= tPacketHeader.GetDecompSize();

        tPacketHeader = *(ZLibPacketHeader *)ptStream->next_in;
        ptStream->next_in += sizeof(ZLibPacketHeader);
        ptStream->avail_in -= sizeof(ZLibPacketHeader);
        if (fByteSwap) {
            tPacketHeader.dwEncryptedAndCompSize = (uint16)((tPacketHeader.dwEncryptedAndCompSize << 8) | (tPacketHeader.dwEncryptedAndCompSize >> 8));
            tPacketHeader.dwDecompSizeHigh = (uint16)((tPacketHeader.dwDecompSizeHigh << 8) | (tPacketHeader.dwDecompSizeHigh >> 8));
            tPacketHeader.dwDecompSizeLow = (uint16)((tPacketHeader.dwDecompSizeLow << 8) | (tPacketHeader.dwDecompSizeLow >> 8));
        }
    }




    if (nToDecompressInVoid == 0) {

        ptStream->next_in -= sizeof(ZLibPacketHeader);
        ptStream->avail_in += sizeof(ZLibPacketHeader);
        dwCurrentPacketRemainingDecompSize = 0;
        fInsidePacketHeader = FALSE;
        return TRUE;
    }

    dwCurrentPacketRemainingDecompSize = tPacketHeader.GetDecompSize() - nToDecompressInVoid;
    fInsidePacketHeader = TRUE;


    if (tPacketHeader.tEncryptedAndCompSize.fEncrypted) {

        ptStream->next_in -= sizeof(ZLibPacketHeader);
        Blizzard::Util::DecXor64(XOR64_ENCRYPTION_KEY, ptStream->next_in + sizeof(ZLibPacketHeader), tPacketHeader.tEncryptedAndCompSize.dwCompSize);
        ((ZLibPacketHeader *)ptStream->next_in)->tEncryptedAndCompSize.fEncrypted = 0;
        ptStream->next_in += sizeof(ZLibPacketHeader);
    }


    while (nToDecompressInVoid > 0) {


        int32 nToDecompress = nToDecompressInVoid > nVoidBufferSize ? nVoidBufferSize : nToDecompressInVoid;
        ptStream->next_out = pVoidBuffer;
        ptStream->avail_out = nToDecompress;
        int err = inflate(ptStream, Z_NO_FLUSH);






        if (err != Z_OK && ptStream->avail_out != 0) {

            return FALSE;
        }


        nToDecompressInVoid -= nToDecompress;
    }

    return TRUE;
}


BOOL DecompressData(z_stream * ptStream, int32 nSize, uint8 * pBuffer, BOOL fByteSwap, uint32 & dwCurrentPacketRemainingDecompSize, BOOL & fInsidePacketHeader)






{
    if (!fInsidePacketHeader) {

        ZLibPacketHeader tPacketHeader = *(ZLibPacketHeader *)ptStream->next_in;











        if (tPacketHeader.tEncryptedAndCompSize.fEncrypted) {

            Blizzard::Util::DecXor64(XOR64_ENCRYPTION_KEY, ptStream->next_in + sizeof(ZLibPacketHeader), tPacketHeader.tEncryptedAndCompSize.dwCompSize);
            ((ZLibPacketHeader *)ptStream->next_in)->tEncryptedAndCompSize.fEncrypted = 0;
        }

        ptStream->next_in += sizeof(ZLibPacketHeader);
        ptStream->avail_in -= sizeof(ZLibPacketHeader);


        dwCurrentPacketRemainingDecompSize = tPacketHeader.GetDecompSize();
        fInsidePacketHeader = TRUE;
    }

    int32 nRemainingSize = nSize;
    int32 nCurrentTotalDecompressedSize = 0;
    while (nRemainingSize > 0) {

        ptStream->next_out = pBuffer + nCurrentTotalDecompressedSize;
        ptStream->avail_out = nRemainingSize;
        int32 nPreviousTotalOut = ptStream->total_out;
        int err = inflate(ptStream, Z_NO_FLUSH);
        int32 nCurrentDecompressedSize = ptStream->total_out - nPreviousTotalOut;
        nCurrentTotalDecompressedSize += nCurrentDecompressedSize;
        nRemainingSize -= nCurrentDecompressedSize;


        if (err == Z_STREAM_END) {

            err = inflateReset(ptStream);

            if (ptStream->avail_in > sizeof(ZLibPacketHeader)) {

                ZLibPacketHeader tPacketHeader = *(ZLibPacketHeader *)ptStream->next_in;











                if (tPacketHeader.tEncryptedAndCompSize.fEncrypted) {

                    Blizzard::Util::DecXor64(XOR64_ENCRYPTION_KEY, ptStream->next_in + sizeof(ZLibPacketHeader), tPacketHeader.tEncryptedAndCompSize.dwCompSize);
                    ((ZLibPacketHeader *)ptStream->next_in)->tEncryptedAndCompSize.fEncrypted = 0;
                }

                ptStream->next_in += sizeof(ZLibPacketHeader);
                ptStream->avail_in -= sizeof(ZLibPacketHeader);


                dwCurrentPacketRemainingDecompSize = tPacketHeader.GetDecompSize() - nRemainingSize;
                fInsidePacketHeader = TRUE;
            }

            if (err != Z_OK) {

                return FALSE;
            }

        }

        else if (err != Z_OK && ptStream->avail_out != 0) {

            return FALSE;
        }
    }


    return nCurrentTotalDecompressedSize == nSize;






}
