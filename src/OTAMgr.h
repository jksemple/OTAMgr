#ifndef OTA_MGR_H
#define OTA_MGR_H

#include <HTTPClient.h> // espressif / Arduino-ESP32 library
#include <HTTPUpdate.h> // espressif / Arduino-ESP32 library
#include <WiFiClientSecure.h>

#define OTA_UPTODATE   "OTA Uptodate"
#define OTA_START      "OTA Start"
#define OTA_ERROR      "OTA Error"
#define OTA_PROGRESS   "OTA Progress"
#define OTA_END        "OTA Done"

#define UUID_STRLEN 36

typedef void (*log_cb_t)(const char* eventType, const char* description);

class OTAMgr{
  public:

    OTAMgr();
    ~OTAMgr();
    void init(const String otaServerAddress, const String softwareVersion, log_cb_t logCB, const char* caCert);
    void checkForUpdate(void);
	  bool isOK() { return _isOK; }
	  bool canRetry() { return _retryCount > 0; }
	  bool checked() { return _checked; }
    bool adequateMemory() {
      // Check if there is enough free memory for the OTA update. It fails if it can't load the SSL root cert
      return ESP.getFreeHeap() > 100000; // 100kb free memory required (?)
    }
	
    friend void otaStartCB(void);
    friend void otaProgressCB(int done, int size);
    friend void otaEndCB(void);
    friend void otaErrorCB(int errorCode);

  private:
	bool _isOK;
	bool _checked;
	int _retryCount;
  const char* _caCert;
	log_cb_t _logCallback;
	String _otaServerAddress;
	String _softwareVersion;
	
};

extern OTAMgr g_OTAMgr;

#endif
