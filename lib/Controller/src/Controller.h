#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <Arduino.h>
#include <HttpFOTA.h>
#include <ArduinoLog.h>
#include <NetworkControl.h>
#include <Prefs.h>
#include <LedController.h>
#include <NetworkModule.h>
#include <HeatingController.h>

/* topics */
#define OTA_TOPIC "ota"

typedef enum
{
  Runnning_e = 0x01,
  Fota_e
} SysState;


class Modules {
    private:
        Module *modules[10];
        uint8_t length = 0;
    
    public:
        void addModule(Module *module);
        Module *getAt(uint8_t index);
        uint8_t count();

        virtual void setup();
        virtual void loop();
        virtual void everyMillisecond();
        virtual void every10Milliseconds();
        virtual void every50Milliseconds();
        virtual void every100Milliseconds();
        virtual void everySecond();
        virtual void getTelemetryData(char *targetBuffer);        
};

class Controller : public NetworkModule, public PrefsClient {
    public:
        Controller();
        ~Controller();

        void setup();
        void loop();
        
        virtual void commandReceived(const char *command, const char *payload);
        virtual const char *getTelemetryData();  
        
        virtual void configUpdate(const char *id, const char *value);          

    private:
        static const int BAUD_RATE = 115200;
        const char *root_ca = \
            "-----BEGIN CERTIFICATE-----\n"\
            "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"\
            "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"\
            "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"\
            "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"\
            "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"\
            "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"\
            "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"\
            "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"\
            "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"\
            "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"\
            "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"\
            "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"\
            "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"\
            "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"\
            "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"\
            "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"\
            "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"\
            "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"\
            "-----END CERTIFICATE-----";

        NetworkControl* networkControl;
        Prefs* prefs;
        LedController* ledController;
        HeatingController* heatingController;

        Modules modules;

        char url[100];
        char md5_check[50];
        SysState state = Runnning_e;

        uint8_t enterConfigPortalPin = A4;
};

#endif