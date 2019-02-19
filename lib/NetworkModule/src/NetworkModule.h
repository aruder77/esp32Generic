#ifndef NETWORK_MODULE_H_
#define NETWORK_MODULE_H_

class NetworkModule {

public:
    virtual void commandReceived(char *command, char *payload) = 0;
    virtual char *getTelemetryData() = 0;
};

#endif