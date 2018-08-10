#ifndef MY_BIN_STREAM_HLPR
#define MY_BIN_STREAM_HLPR
#define VCL_IOSTREAM

#include "MyIostream.h"
#include "MyExcpt.hpp"
#include "crc16.h"

DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(CantReadIStream, Exception );
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(CantWriteOStream, Exception );

class MyBinaryIStreamHlpr : public CalcCRC16Hlpr
{
public:
    typedef void (__closure *OnReadMthd)(const char*, unsigned);
    explicit MyBinaryIStreamHlpr( std::istream& strm, bool enableThrow = true,
        unsigned short crc16 = 0xFFFF ) : CalcCRC16Hlpr( crc16 ), 
        strm_(strm), enableThrow_(enableThrow)
    {}
    template<class T> void operator>>(T& t ) const
    {
        DoRead<T*>( &t, sizeof(t) );
    }

    template<class T> T Get() const
    {
        T t;
        DoRead<T*>( &t, sizeof(T) );
        return t;
    }

    template<class T> void ReadArray(T* p, unsigned size ) const
    {
        DoRead<T*>( p, size*sizeof(T) );
    }

    bool CheckCRC16() const
    {
        this->Get<unsigned char>();
        this->Get<unsigned char>();
        return this->GetCRC16()==0;
    }

private:
    mutable std::istream& strm_;
    const bool enableThrow_;

    template<class T> void DoRead( T t, unsigned sz ) const
    {
        char* p = reinterpret_cast<char*>( t );
        strm_.read( p, sz );
        if( strm_.fail() && enableThrow_ )
            MY_THROW_CLASS_(MyCantReadIStreamException, "Ошибка чтения файла.");
        UpdateCRC16( (unsigned char*) p, sz  );
    }

};

class MyBinaryOStreamHlpr : public CalcCRC16Hlpr
{
public:
    explicit MyBinaryOStreamHlpr( std::ostream& strm,
        unsigned short crc16 = 0xFFFF ) :
        CalcCRC16Hlpr( crc16 ), strm_(strm)
    {}
    template<class T> void operator<<(const T& t ) const
    {
        DoWrite<const T*>( &t, sizeof(t) );
    }
    template<class T> void WriteArray(const T* p, unsigned size ) const
    {
        DoWrite<const T*>( p, size*sizeof(T) );
    }
    void FixCRC16() const
    {
        const unsigned short
            crc16 = this->GetCRC16();
        const unsigned char ch[2] = {crc16 >> 8, crc16 };
        WriteArray<unsigned char>(ch,2);
        assert( this->GetCRC16()==0 );
    }

private:
    mutable std::ostream& strm_;


    template<class T> void DoWrite( T t, unsigned sz ) const
    {
        const char* p = reinterpret_cast<const char*>( t );
        strm_.write( p, sz );
        if( strm_.fail() ) MY_THROW_CLASS_(MyCantWriteOStreamException,
            "Ошибка записи файла");
        UpdateCRC16( (unsigned char*) p, sz  );
    }
};

#endif