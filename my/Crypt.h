#pragma once

#include <string>

typedef unsigned char uchar;
typedef unsigned long ulong;

namespace my {

struct CryptRC {
	CryptRC() : rc(0) {}
	int rc;
	std::string msg;
};

struct CryptData {
	CryptData() : pInData(NULL), pOutData(NULL), inDataLen(0), outDataLen(0) {}
	uchar *pInData;
	uchar *pOutData;
	ulong inDataLen;
	ulong outDataLen;
	uchar *pPwd;
	ulong pwdLen;
};

class Crypt {
public:
	CryptRC encrypt( CryptData &cdata ) const;
	CryptRC decrypt( CryptData &cdata ) const;
};

} //my

