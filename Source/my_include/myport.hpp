//------------------------------------------------------------------------------
#ifndef MY_ABSTRUCT_COMMUNICATION_PORT_HEADER_INCLUDED__________
#define MY_ABSTRUCT_COMMUNICATION_PORT_HEADER_INCLUDED__________
//------------------------------------------------------------------------------
#include <system.hpp>
#include "boost\noncopyable.hpp"
//------------------------------------------------------------------------------
    namespace Inifiles
    {
        class TIniFile;
    };

	// интерфейс для канала приёмопередачи
	class MyPort : public boost::noncopyable
	{
	public:
        virtual ~MyPort() {}

		virtual AnsiString Description() const = 0;
        virtual bool IsConnected() const = 0;
        virtual bool ConnectionWasBreaked() const { return false; }

        virtual void Connect() = 0;
        virtual void Disconnect() = 0;
        
        virtual void SaveSettingsToIniFile(Inifiles::TIniFile*, const AnsiString& sekt) {}
        virtual void LoadSettingsFromIniFile(Inifiles::TIniFile*, const AnsiString& sekt) {}

        virtual int Write(void*, int) = 0;
		virtual int Read(void*, int) = 0;
		virtual int GetRxDSize() = 0;
		virtual void Purge() = 0;
	};



#endif
