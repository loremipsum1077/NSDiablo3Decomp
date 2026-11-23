// X/Client/ImageUtils/ImageCompressAPI.cpp -- installs the stub compressor.
// Written so the stub's vtable and inline methods are emitted; main.elf
// 0x71003dcf60 / 0x71003dcfd0 are this file's two functions.
#include "ImageCompressAPI.h"
#include "ImageCompressStubAPI.h"














ImageCompressAPI * gptImageCompressAPI;  // ImageCompressAPI.cpp:20




void ImageCompressAPIInitialize() {  // ImageCompressAPI.cpp:25

    // Dispose is inlined here; the destructor is virtual, so `delete` calls it.
    ImageCompressAPIDispose();




    gptImageCompressAPI = new ImageCompressStubAPI;
}




void ImageCompressAPIDispose() {  // ImageCompressAPI.cpp:39
    if (gptImageCompressAPI) {
        ::delete gptImageCompressAPI;
        gptImageCompressAPI = 0;
    }
}
