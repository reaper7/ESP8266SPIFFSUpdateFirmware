/*
*  2017 Reaper7
*/

#ifndef __SPIFFS_UPDATE_FIRMWARE
#define __SPIFFS_UPDATE_FIRMWARE

#include <FS.h>
#include <functional>

#define FIRMWAREEXT     ".bin"
#define FIRMWAREPATH    "/f"
#define MAXFIRMAREFILES 5
#define MINBINSIZE      100000

class SPIFFSUpdateFirmwareClass
{
  public:
	typedef std::function<void(void)> THandlerFunction;
//	typedef std::function<void(firm_error_t)> THandlerFunction_Error;
	typedef std::function<void(unsigned int, unsigned int)> THandlerFunction_Progress;

    SPIFFSUpdateFirmwareClass();
    ~SPIFFSUpdateFirmwareClass();

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
    uint8_t _numberfirmwarefiles;
    bool _filesystemexists;
    uint32_t _minSketchSpace;
    uint32_t _maxSketchSpace;

    THandlerFunction _start_callback;
    THandlerFunction _end_callback;
//    THandlerFunction_Error _error_callback;
    THandlerFunction_Progress _progress_callback;

};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFSFIRMWARE)
extern SPIFFSUpdateFirmwareClass SPIFFSFirmware;
#endif

#endif /* __SPIFFS_UPDATE_FIRMWARE */
