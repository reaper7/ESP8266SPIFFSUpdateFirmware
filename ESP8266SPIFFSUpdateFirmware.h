/**The MIT License (MIT)

Copyright (c) 2017 by Reaper7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __SPIFFS_UPDATE_FIRMWARE
#define __SPIFFS_UPDATE_FIRMWARE

#include <FS.h>
#include <functional>
#undef min
#undef max
#include <vector>

#define FIRMWAREEXT     ".bin"
#define FIRMWAREPATH    "/f"
#define MINBINSIZE      100000

typedef struct {
        char * fmname;
        char * fmsize;
} FirmwareFileList_t;

class SPIFFSUpdateFirmware
{
  public:
	typedef std::function<void(void)> THandlerFunction;
//	typedef std::function<void(firm_error_t)> THandlerFunction_Error;
	typedef std::function<void(unsigned int, unsigned int)> THandlerFunction_Progress;

    SPIFFSUpdateFirmware();
    ~SPIFFSUpdateFirmware();

    //This callback will be called when Upgrade has begun
    void onStart(THandlerFunction fn);
    
    //This callback will be called when Upgrade has finished
    void onEnd(THandlerFunction fn);

    //This callback will be called when Upgrade is receiving data from SPIFFS file 
    void onProgress(THandlerFunction_Progress fn);

//    void onError(THandlerFunction_Error fn);

    //return file size from index
    uint32_t getSize(uint8_t _index);

    //return file name from index
    String getName(uint8_t _index);

    //start update with file name, return true when OK
    bool startUpdate(String _fn, bool _rst = false);

    //start update with file index, return true when OK
    bool startUpdate(uint8_t _index, bool _rst = false);

    //get number of valid binary files
    uint8_t getCount();

    //start this lib (init SPIFFS file system), return true when OK
    bool begin(String _fwpath = FIRMWAREPATH);

  private:
    String _firmwareext;
    String _firmwarepath;
    bool _filesystemexists;
    uint32_t _minSketchSpace;
    uint32_t _maxSketchSpace;

    THandlerFunction _start_callback;
    THandlerFunction _end_callback;
//    THandlerFunction_Error _error_callback;
    THandlerFunction_Progress _progress_callback;
    
    std::vector<FirmwareFileList_t> FirmwareList;
    void FirmwareListAdd(const char* fmname, uint32_t fmsize);
    void FirmwareListClean(void);    
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFSFIRMWARE)
extern SPIFFSUpdateFirmware SPIFFSFirmware;
#endif

#endif /* __SPIFFS_UPDATE_FIRMWARE */
