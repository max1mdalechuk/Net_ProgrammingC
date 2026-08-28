#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_ARRAY_SIZE 256 

struct ClientRequest {
    int count;
    int data[MAX_ARRAY_SIZE];
};

struct ServerResponse {
    int min;
    int max;
    double avg;
    char message[256];
    bool isError;
};

#endif