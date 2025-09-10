#include "OTAMgr.h"

OTAMgr::OTAMgr() {
}

OTAMgr::~OTAMgr() {
}

void OTAMgr::init(const String otaServerAddress, const String softwareVersion, log_cb_t logCB, const char* caCert) {

  _otaServerAddress = otaServerAddress;
  _softwareVersion = softwareVersion;
  _logCallback = logCB;
  _isOK = true;
  _retryCount = 3;
  _caCert = caCert;
  log_i("in init");
}

void otaStartCB() {
  g_OTAMgr._isOK = true;
  g_OTAMgr._logCallback("OTA", "Starting");
}

void otaProgressCB(int done, int size) {
  String desc = "Done " + String(done) + " of " + String(size);
  g_OTAMgr._logCallback("OTA", desc.c_str());
}

void otaEndCB() {
  g_OTAMgr._logCallback("OTA", "Complete");
  g_OTAMgr._checked = true;
}

void otaErrorCB(int errorCode) {
  String desc = "Error = " + errorCode;
  g_OTAMgr._logCallback("OTA", desc.c_str());
  g_OTAMgr._isOK = false;
}

void OTAMgr::checkForUpdate() {
  WiFiClientSecure client;
  client.setCACert(_caCert);
  client.setTimeout(20000);
  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  httpUpdate.onStart(otaStartCB);
  httpUpdate.onProgress(otaProgressCB);
  httpUpdate.onEnd(otaEndCB);
  httpUpdate.onError(otaErrorCB);

  // It is possible to send an additional "currentVersion" string that is sent as a header "x-ESP32-version"
  // but that doesn't seem to have any advantages over just using the MD5 that is sent anyway.
  // As it's there I might as well send the user-friendly sw version string such as 1.0.1
  // but the true key to the software is the MD5 hash, which is always sent

  if (_otaServerAddress.length() == 0 || ! strchr(_otaServerAddress.c_str(), ':')) {
    _logCallback("OTA", "OTA ServerUrl not set");
  	log_e("OTA ServerUrl not set");
    return;
  }
  String currentVersion = _softwareVersion;
  currentVersion += "&deviceIP=";
  currentVersion += WiFi.localIP().toString();
  currentVersion += "&SSID=";
  currentVersion += WiFi.SSID();
  t_httpUpdate_return ret = httpUpdate.update(client, _otaServerAddress, currentVersion);

  String err, desc;
  switch (ret){
    case HTTP_UPDATE_FAILED:
	  err = String(httpUpdate.getLastError());
	  err += ", ";
	  err += httpUpdate.getLastErrorString();
      desc = "Error = " + err +  _otaServerAddress;
      _logCallback("OTA", desc.c_str());
  	  log_e("OTA error =", err.c_str());
      _isOK = false;
      break;
    
    case HTTP_UPDATE_NO_UPDATES:
      _logCallback("OTA", "No updates for this device");
	    log_i("OTA no updates");
      _checked = true;
      _isOK = true;
      break;
    
    case HTTP_UPDATE_OK:
      _logCallback("OTA", "New firmware installed - rebooting");
  	  log_i("OTA end");
      _checked = true;
      _isOK = true;
      break;
  }
  _retryCount -= 1;
}
	
OTAMgr g_OTAMgr;
