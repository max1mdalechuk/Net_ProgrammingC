#ifndef _NETWORK_PROGRAMMING_COMMON_PROTOCOLS

#define _NETWORK_PROGRAMMING_COMMON_PROTOCOLS

struct SquareRootRequest {
	double a;
	double b;
	double c;
};

enum RootStatus { NO_ROOTS = 0, ONE_ROOTS = 1, TWO_ROOTS = 2 };

struct SquareRootResponse {
	double x1;
	double x2;
	enum RootStatus status;
};

struct MessagePacket {
    char data[256];
};

struct WordPacket {
    char data[256];
    int words;
};
#endif // !_NETWORK_PROGRAMMING_COMMON_PROTOCOLS
