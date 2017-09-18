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

#include <Esp.h>
#include "ESP8266SPIFFSUpdateFirmware.h"
#include <string.h>

#define FIRMWAREEXT     ".bin"
#define FIRMWAREPATH    "/"
#define MINBINSIZE      100000
#define READBUFFSIZE    256 

SPIFFSUpdateFirmware::SPIFFSUpdateFirmware()
: _firmwareext(FIRMWAREEXT)
, _firmwarepath(FIRMWAREPATH)
, _filesystemexists(false)
, _minSketchSpace(MINBINSIZE)
, _maxSketchSpace(MINBINSIZE)
, _start_callback(NULL)
, _end_callback(NULL)
//, _error_callback(NULL)
, _progress_callback(NULL)
{
}

SPIFFSUpdateFirmware::~SPIFFSUpdateFirmware() {
  FirmwareListClean();
}

void SPIFFSUpdateFirmware::onStart(THandlerFunction fn) {
    _start_callback = fn;
}

void SPIFFSUpdateFirmware::onEnd(THandlerFunction fn) {
    _end_callback = fn;
}

void SPIFFSUpdateFirmware::onProgress(THandlerFunction_Progress fn) {
    _progress_callback = fn;
}
/*
void SPIFFSUpdateFirmware::onError(THandlerFunction_Error fn) {
    _error_callback = fn;
}
*/

uint32_t SPIFFSUpdateFirmware::getSize(uint8_t _index) {
  if (_filesystemexists && _index >= 0 && _index < FirmwareList.size()) {
    FirmwareFileList_t entry = FirmwareList[_index];
    return (atoi(entry.fmsize));
  } else
    return (0);
}

String SPIFFSUpdateFirmware::getName(uint8_t _index) {
  if (_filesystemexists && _index >= 0 && _index < FirmwareList.size()) {
    FirmwareFileList_t entry = FirmwareList[_index];
    return ((String)entry.fmname);
  } else
    return ("");
}

bool SPIFFSUpdateFirmware::startUpdate(String _fn, bool _rst) {
  bool ret = false;
  String _fname = _firmwarepath + "/" + _fn + _firmwareext;

  if ((_filesystemexists) && (_fname != "")) {
    if (SPIFFS.exists(_fname)) {
      File firmfile = SPIFFS.open(_fname, "r");
      uint32_t fsize = firmfile.size();
      if (Update.begin(_maxSketchSpace, U_FLASH)) {
        if (_start_callback) {
          _start_callback();
        }
        uint32_t filerest = 0;
        if (_progress_callback) {
          _progress_callback(filerest, fsize);
        }
        uint8_t ibuffer[READBUFFSIZE];
        while (firmfile.available()) {
          uint32_t isize = fsize-filerest>=READBUFFSIZE?READBUFFSIZE:fsize-filerest;
          firmfile.read((uint8_t *)ibuffer, isize);
          filerest += Update.write(ibuffer, isize);
          if (_progress_callback) {
            _progress_callback(filerest, fsize);
          }
        }
        if (Update.end(true)) {
          ret = true;
          if (_end_callback) {
            _end_callback();
          }
        }
      }
      firmfile.close();
      if (ret && _rst) {
        delay(250);
        ESP.restart();
      }
    }
  }
  return ret;
}

bool SPIFFSUpdateFirmware::startUpdate(uint8_t _index, bool _rst) {
  if (_filesystemexists && _index >= 0 && _index < FirmwareList.size())
    return (startUpdate(getName(_index), _rst));
  else
    return false;
}

uint8_t SPIFFSUpdateFirmware::getCount() {
  if (_filesystemexists) {
    Dir dir = SPIFFS.openDir(_firmwarepath);
    while (dir.next()) {
      String _fname = dir.fileName();
      if (_fname.endsWith(_firmwareext)) {
        uint32_t _fsize = dir.fileSize();
        if (_fsize > _minSketchSpace && _fsize <= _maxSketchSpace) {
          _fname = _fname.substring(_firmwarepath.length() + 1, _fname.length() - _firmwareext.length());
          FirmwareListAdd(_fname.c_str(), _fsize);
        }
      }
    }
  }
  return (FirmwareList.size());
}

void SPIFFSUpdateFirmware::FirmwareListAdd(const char* fmname, uint32_t fmsize) {
  FirmwareFileList_t newFirmware;
  char _fsch[16];
  sprintf(_fsch,"%lu", fmsize);  
  newFirmware.fmsize = strdup(_fsch);
  newFirmware.fmname = strdup(fmname);
  FirmwareList.push_back(newFirmware);
}

void SPIFFSUpdateFirmware::FirmwareListClean(void) {
  //clear & free firmware list
  for(uint8_t i = 0; i < FirmwareList.size(); i++) {
    FirmwareFileList_t entry = FirmwareList[i];
    if(entry.fmname) {
      free(entry.fmname);
    }
    if(entry.fmsize) {
      free(entry.fmsize);
    }
  }
  FirmwareList.clear();
}

bool SPIFFSUpdateFirmware::begin(String _fwpath) {
  _filesystemexists = SPIFFS.begin();
  if (_filesystemexists) {
    FirmwareListClean();
    _firmwarepath = _fwpath;
    _maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000; 
  }
  return (_filesystemexists);
}

bool SPIFFSUpdateFirmware::begin() {
  begin(FIRMWAREPATH);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFSFIRMWARE)
SPIFFSUpdateFirmware SPIFFSFirmware = SPIFFSUpdateFirmware();
#endif
