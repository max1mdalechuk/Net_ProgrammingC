#ifndef OWN_PROTOCOL_H
#define OWN_PROTOCOL_H

#define MAX_ARRAY_SIZE 256 

struct ClientRequest {
    int count;
    int data[MAX_ARRAY_SIZE];
};

struct ServerResponse {
    char buf[4096];
    bool isError;
};

#endif