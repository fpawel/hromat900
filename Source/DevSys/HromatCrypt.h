//------------------------------------------------------------------------------
#ifndef HromatCryptH
#define HromatCryptH
//------------------------------------------------------------------------------

extern "C" __import unsigned short int CalculateHromatCRC16( const unsigned char* pBuff,
    unsigned int buffLen, unsigned short int key);

extern "C" __import void HromatCryptData( const unsigned char* pData, unsigned len,
    unsigned char* pCryptedData );

extern "C" __import unsigned short int HromatSignData( const unsigned char *pIn, unsigned len);



//------------------------------------------------------------------------------
#endif
