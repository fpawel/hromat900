//---------------------------------------------------------------------------
#include <windows.h>
#include <wincrypt.h>


#include <fstream>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

#define MY_GOOD_PASSWORD "15C4775F-633E-456A-897F-CD3278185AE2-17DB69CB-1C08-413A-A147-25382BB6DD3F-554FD1A4-51BB-4116-8939-3B6458AF7A50"
//#define MY_CRYPTO_KEY_CONTAINER_NAME "Hromat-900 Keys Conatiner"

#pragma hdrstop

#include "mycrypt.h"
//#include "MyAppProfile.h"
#include "MyIostream.h"
#include "guicon.h"
#include "MyExeption.h"
#include "AnsiStringUtils_.h"
#include "..\my_include\FooCalledOnce.hpp"

// loki
#include "loki\singleton.h"

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define MY_SAFE_CALL_CRYPTOAPI(foo,params)\
		if(::##foo##params##==FALSE ) throw MyExcptionFactory::Create<MyWindowsException>\
		(::GetLastError(), GET_FLINE_,"::"#foo);
//------------------------------------------------------------------------------
#define ON_CRYPTOAPI_EXCEPTION \
const PMyExcpt excpt = RethrowMyException(GET_FLINE_);\
MyWCout( MYSPRINTF_( "Ошибка модуля шифрования! %s %s\n", __FUNC__, excpt->Root()->Message() ));

//------------------------------------------------------------------------------
#define KEY_PASSWORD_STRING \
"7E80A7D3-552C-4A3C-8D74-73A9A659467E-B3C0F9AF-9D8D-4F0C-9776-2E6C60AB5BEB-"\
"61B451F8-7671-4C98-B2D8-E8BF840C0900-3D2D8CD8-44ED-4BA0-9000-EF1B92F6D25B-" \
"005412B0-EDAB-40A1-8775-0FE4006D9394-33C4A753-C909-4C1A-9996-FF57E9DB5808435AB602"\
"-AAFE-475C-AC6C-ABC84C88067F-588A5619-31C1-4602-A6BC-BA3A03340671-50245577-"\
"6047-4789-9073-A8428D97A309-38011AD7-1A68-49F3-82D7-9F9CBC2BDBF8-E9B2888D-A8C2-"\
"4B3C-81F1-F3A5E5D02998-3D06C397-6400-4906-BFC1-C7FE62805562-178BBB56-59FA-477A-"\
"8676-E89C59B6D0D9-AD80E86C-DEA1-4919-AB6C-BCD84008DD2D-DF60FF10-07A6-4893-83EC-"\
"CB4935F37553-6D18A33D-E9E9-470C-8E80-C0124A7A5C59-07BA8C17-4468-4CCB-B14E-"\
"40DB563C554B-9A3FF810-36C9-4CC3-A65E-B0CAEB98EE2C-AA297FD2-B59B-426B-9B31-"\
"8F8A5F8CAD8B-2E2EA80F-720D-4A14-BDC0-44126DCBD280-9891D351-587F-4FE5-99AB-"\
"FA6D1970B5E6-4F65E0AE-FBD2-4267-96EF-EDD4F1BC7004-0F55D354-173D-4D4B-B4B3-"\
"C5A68F5C00BE-A39137C4-C946-4B57-AD99-D908E095F8CE-40F6D13F-4A73-4AE4-B02B-"\
"4805A5DEBB8F-73F9F14C-2E26-4D0B-AB50-B77D1902CBD5-5F40C21E-1735-4430-AEEC-"\
"BD9A839D84C2-89943321-CEE8-4718-9373-C3"

HCRYPTPROV MyCryptAcquireContext()
{
	HCRYPTPROV hCryptProv_ = 0;
	const char keyContainerName[] = "Защищённое хранилище ключей Хромат-900 8CFF1473-2438-454A-A47D-B3F9E7F226AD";
	// открыть контейнер ключей
    MyWCout( AnsiString().sprintf( "Пытаюсь открыть контейнер ключей \"%s\"...\n", keyContainerName) );

    if( ::CryptAcquireContext( &hCryptProv_, keyContainerName, "",
        PROV_RSA_FULL, CRYPT_NEWKEYSET)==FALSE && (unsigned)::GetLastError()==NTE_EXISTS )
    	::CryptAcquireContext( &hCryptProv_, keyContainerName, "",
        	PROV_RSA_FULL, 0);
    if( hCryptProv_==NULL )
    	throw MyExcptionFactory::Create<MyWindowsException>(::GetLastError(), GET_FLINE_,"::CryptAcquireContext");
    return hCryptProv_;
}

class MyCryptHash : public boost::noncopyable
{
public:
	explicit MyCryptHash(HCRYPTPROV hCryptProv, DWORD alg = CALG_MD5) : hHash_(NULL)
    {
    	//MyWCout( "Создаём объект-хэш...\n");
    	// создать хэш
    	MY_SAFE_CALL_CRYPTOAPI( CryptCreateHash, ( hCryptProv, alg, NULL, 0, &hHash_ ) );
    }
    ~MyCryptHash()
    {
    	//MyWCout( "Удаляем объект-хэш...\n");
    	::CryptDestroyHash(hHash_);
    }
    HCRYPTHASH Get() const { return hHash_; }
    void Extract( std::vector<char>& dt ) const
    {
    	//MyWCout( "Получаю хешированные данные...\n");
    	DWORD dwDataLen = 0;
    	MY_SAFE_CALL_CRYPTOAPI( CryptGetHashParam, (hHash_, HP_HASHVAL, NULL, &dwDataLen, 0) );
        //MyWCout( MYSPRINTF_("Буфер под хешированные данные %d байт.\n", dwDataLen));
        dt.resize(dwDataLen);
        //MyWCout( "Помещаю хешированные данные в буфер...\n");
        MY_SAFE_CALL_CRYPTOAPI( CryptGetHashParam, (hHash_, HP_HASHVAL, &dt.at(0), &dwDataLen, 0) );
        //MyWCout( MYSPRINTF_("В буфер помещено %d байт хешированных данных.\n", dwDataLen));

    }
    bool Compare( const char *p, unsigned len )
    {
    	std::vector<char> dt;
    	this->Extract( dt );
        return std::memcmp( p, &dt.at(0), std::min( dt.size(), len ) )==0;
    }

private:
	HCRYPTHASH hHash_;
};
//------------------------------------------------------------------------------
class MyCryptDerivedKey : public boost::noncopyable
{
public:
	explicit MyCryptDerivedKey(HCRYPTPROV hCryptProv, const char passStr[] )
    {
    		// создать хэш
    		MyWCout( "Создаём ключ из пароля...\n");

    		const MyCryptHash hash(hCryptProv);

    		// Compute the cryptographic hash of the buffer.
    		MyWCout( "Хэшируем пароль...\n");
    		MY_SAFE_CALL_CRYPTOAPI( CryptHashData, (hash.Get(), passStr, std::strlen(passStr), 0) );

        	std::vector<char> hashedPassword;
        	hash.Extract(hashedPassword);
        	hashedPassword.resize(hashedPassword.size()+1);
        	hashedPassword[ hashedPassword.size()-1 ]  = '\0';
        	MyWCout( AnsiString(&hashedPassword[0]) + "\n");

    		enum{  KEYLENGTH = 0x00800000 };
    		MyWCout( "Создаём объект-ключ...\n");
    		MY_SAFE_CALL_CRYPTOAPI( CryptDeriveKey, (hCryptProv, CALG_RC4, hash.Get(), KEYLENGTH, &hKey_));
    }
    ~MyCryptDerivedKey()
    {
    	MyWCout( "Удаляем объект-ключ...\n");
        ::CryptDestroyKey(hKey_);
    }
    HCRYPTKEY Get() const { return hKey_; }
private:
	HCRYPTKEY hKey_;
};
//------------------------------------------------------------------------------
class MyCryptT : public boost::noncopyable
{
public:
	explicit MyCryptT();
    void CalcDigSig( const char* pDataToBeHashedAndSigned, unsigned dataToBeHashedAndSignedLen,
		std::vector<char>& digitalSignature);

    void Encrypt( std::vector<char>& dt );
    void Decrypt( std::vector<char>& dt );

    void HashData(const char* pData, unsigned len, std::vector<char>& hashOut );

private:
	HCRYPTPROV hCryptProv_;
    boost::shared_ptr<MyCryptDerivedKey> privKey_;
    HCRYPTKEY GetKey() const { return privKey_->Get(); }
};
//------------------------------------------------------------------------------
typedef Loki::SingletonHolder
    	<
    		MyCryptT,
        	Loki::CreateUsingNew,
    		Loki::NoDestroy
    	> MyCrypt;
//------------------------------------------------------------------------------
MyCryptT::MyCryptT() :	hCryptProv_(NULL), privKey_()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	hCryptProv_ = MyCryptAcquireContext();
    MyWCout( "Мутим НАШ ключик...\n");
    privKey_.reset( new MyCryptDerivedKey(hCryptProv_, KEY_PASSWORD_STRING) );
}
//------------------------------------------------------------------------------
void MyCryptT::HashData(const char* pData, unsigned len, std::vector<char>& hashOut )
{
	const MyCryptHash hash(hCryptProv_);
	// Compute the cryptographic hash of the buffer.
    //MyWCout( MYSPRINTF_("Хэшируем %d байт .\n", len));
    MY_SAFE_CALL_CRYPTOAPI( CryptHashData, (hash.Get(), pData, len, 0) );
    	// помещаем хэш в буфер
    hash.Extract(hashOut);
}
//------------------------------------------------------------------------------
void MyCryptT::CalcDigSig( const char* pDataToBeHashedAndSigned, unsigned dataToBeHashedAndSignedLen,
		std::vector<char>& digitalSignature)
{
	MyWCout( MYSPRINTF_("Мутим цифровую подпись для буфера %d байт",dataToBeHashedAndSignedLen ));
    this->HashData(pDataToBeHashedAndSigned, dataToBeHashedAndSignedLen, digitalSignature);
    // шифруем буфер
    this->Encrypt(digitalSignature);
}
//------------------------------------------------------------------------------
void MyCryptT::Encrypt( std::vector<char>& dt )
{
	DWORD pdwDataLen = dt.size();
	//MyWCout( MYSPRINTF_("MyCrypt::Encrypt  - шифрую %d байт.\n", pdwDataLen ));

    //MyWCout( "::CryptEncrypt - определяю размер буфера...\n");
    MY_SAFE_CALL_CRYPTOAPI( CryptEncrypt, ( GetKey(), 0, TRUE, 0, NULL, &pdwDataLen, 0 ) );
    dt.resize(pdwDataLen);
    MyWCout( MYSPRINTF_("MyCrypt::Encrypt - размер буфера %d байт. Выполняем шифрование...\n", pdwDataLen) );
    MY_SAFE_CALL_CRYPTOAPI( CryptEncrypt, ( GetKey(), 0, TRUE, 0, &dt.at(0), &pdwDataLen, pdwDataLen ) );
}
//------------------------------------------------------------------------------
void MyCryptT::Decrypt( std::vector<char>& dt )
{
	DWORD pdwDataLen = dt.size();
	//MyWCout( MYSPRINTF_("MyCrypt::Decrypt  - расшифровка %d байт.\n", pdwDataLen ));
    MyWCout( "MyCrypt::Decrypt - выполняем расшифровку...\n");
    MY_SAFE_CALL_CRYPTOAPI( CryptDecrypt, (GetKey(), 0, TRUE, 0, &dt.at(0), &pdwDataLen ) );
}
//------------------------------------------------------------------------------
void InitializeMyCrypt()
{
	try
    {
		MyCrypt::Instance();
    }
    catch(...)
    {
    	ON_CRYPTOAPI_EXCEPTION
    }
}
//------------------------------------------------------------------------------
void MyCryptEncrypt( std::vector<char>& dt )
{
	try
    {
		MyCrypt::Instance().Encrypt( dt );
    }
    catch(...)
    {
    	ON_CRYPTOAPI_EXCEPTION
    }
}
//------------------------------------------------------------------------------
void MyCryptDecrypt( std::vector<char>& dt )
{
	try
    {
		MyCrypt::Instance().Decrypt( dt );
    }
    catch(...)
    {
    	ON_CRYPTOAPI_EXCEPTION
    }
}
//------------------------------------------------------------------------------
void MyCryptHashData(const char* pData, unsigned len, std::vector<char>& hash )
{
	try
    {
		MyCrypt::Instance().HashData(pData, len, hash );
    }
    catch(...)
    {
    	ON_CRYPTOAPI_EXCEPTION
    }
}
//------------------------------------------------------------------------------
void MyCryptPrimitiveCrypt( const char* pData, const char* pDataEnd,
	char* pEncryptedData, const char *pPassKey, const char *pPassKeyEnd )
{
	for( ; pData!=pDataEnd; ++pData, ++pEncryptedData )
    {
    	*pEncryptedData = *pData;
    	for( const char *p=pPassKey; p!=pPassKeyEnd; ++p ) *pEncryptedData ^= *p;
    }
}



