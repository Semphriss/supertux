// SOURCE: https://cplusplus.com/forum/beginner/51572/

#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>
std::string base64_encode(const char* , unsigned int len);
std::string base64_decode(std::string const& s);
#endif 