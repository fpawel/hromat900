//------------------------------------------------------------------------------
#ifndef mycryptH
#define mycryptH
//------------------------------------------------------------------------------
//#include <SysUtils.hpp>
#include <vector>

void InitializeMyCrypt();
void MyCryptPrimitiveCrypt( const char* pData, const char* pDataEnd,
	char* pCryptedData, const char *pPassKey, const char *pPassKeyEnd );

void MyCryptEncrypt( std::vector<char>& dt );
void MyCryptDecrypt( std::vector<char>& dt );
void MyCryptHashData(const char* pData, unsigned len, std::vector<char>& hash );

//------------------------------------------------------------------------------
#endif
