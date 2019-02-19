#ifndef NETWORK_MODULE_H_
#define NETWORK_MODULE_H_

class NetworkModule {

public:
    virtual void commandReceived(const char *command, const char *payload) = 0;
    virtual const char *getTelemetryData() = 0;
};

#endif