#ifndef NETWORK_MODULE_H_
#define NETWORK_MODULE_H_

class NetworkModule {

public:
    virtual void getTelemetryData(char *targetBuffer) = 0;
};

#endif