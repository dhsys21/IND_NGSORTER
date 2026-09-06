#ifndef ProductionProtocolH
#define ProductionProtocolH
#include <string>

// MC 3E binary response: 9-byte header followed by length bytes (incl. end code).
// Returns 0 for incomplete input, 1 for a frame, -1 for an invalid stream.
inline int TakeMcResponse(std::string &buffer, std::string &frame)
{
    if(buffer.size() > 8192) return -1;
    if(buffer.size() < 9) return 0;
    const unsigned char *p = reinterpret_cast<const unsigned char*>(buffer.data());
    if(p[0] != 0xd0 || p[1] != 0 || p[2] != 0 || p[3] != 0xff ||
       p[4] != 0xff || p[5] != 3 || p[6] != 0) return -1;
    unsigned length = p[7] | (unsigned(p[8]) << 8);
    if(length < 2 || length > 1024) return -1;
    if(buffer.size() < 9 + length) return 0;
    frame.assign(buffer, 0, 9 + length);
    buffer.erase(0, 9 + length);
    return 1;
}
#endif
