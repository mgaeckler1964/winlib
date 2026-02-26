/*
		Project:		WINLIB
		Module:			registry.cpp
		Description:	Registry keys
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

#ifndef GAK_WINLIB_REGISTRY_H
#define GAK_WINLIB_REGISTRY_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <windows.h>
#include <gak/CopyProtection.h>
#include <gak/string.h>
#include <gak/array.h>
#include <gak/types.h>

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifndef KEY_WOW64_32KEY	// for Borland C++ Builder
#	define KEY_WOW64_32KEY         (0x0200)
#	define KEY_WOW64_64KEY         (0x0100)
#	define KEY_WOW64_RES           (0x0300)
#endif

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

enum RegistryType
{
	rtMISSING = REG_NONE-2,
	rtERROR = REG_NONE-1,
	rtNONE = REG_NONE,
	rtSTRING = REG_SZ,
	rtENV = REG_EXPAND_SZ,
	rtBINRAY = REG_BINARY,
	rtINTEGER = REG_DWORD,
	rtINT64 = REG_QWORD
};

enum ReadSuccess
{
	rsOK,
	rsBadType,
	rsBadSize,
	rsNotFound,
	rsError,
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

template <typename REGTYPE>
struct RegTraits
{
	static const RegistryType	s_registryType = rtNONE;
};

template <>
struct RegTraits<gak::STRING>
{
	static const RegistryType	s_registryType = rtSTRING;
};

template <>
struct RegTraits<long>
{
	static const RegistryType	s_registryType = rtINTEGER;
};

template <>
struct RegTraits<gak::int64>
{
	static const RegistryType	s_registryType = rtINT64;
};


template <typename RESULTTYPE>
class ResultTraits : public RegTraits<RESULTTYPE>
{
	RESULTTYPE	m_valueBuff;

	public:
	static const bool s_fixBufferSize = true;

	ResultTraits() {}

	size_t expand( size_t valueSize )
	{
		return  valueSize;
	}
	void setBufferSize( size_t  )
	{
	}
	size_t size() const
	{
		return sizeof( m_valueBuff );
	}
	void *getBuffer() const
	{
		return (void *)&m_valueBuff;
	}
	void setActSize( size_t )
	{
	}
	RESULTTYPE	GetValue()
	{
		return m_valueBuff;
	}
	static size_t GetValueSize(const RESULTTYPE &value)
	{
		return sizeof(RESULTTYPE);
	}
};

template <>
class ResultTraits<gak::STRING> : public RegTraits<gak::STRING>
{
	size_t		m_minSize;
	gak::STRING	m_valueBuff;

	void release()
	{
		m_valueBuff.release();
		m_minSize = 0;
	}

	public:
	static const bool s_fixBufferSize = false;

	ResultTraits() : m_minSize(0) {}

	size_t expand( size_t valueSize )
	{
		setActSize(valueSize);
		gak::STRING	newBuffer = m_valueBuff;
		DWORD	newSize = ExpandEnvironmentStrings( newBuffer, nullptr, 0 );
		release();	// this ensures, that this is a real new buffer
		setBufferSize(newSize+1);
		return  ExpandEnvironmentStrings( newBuffer, LPSTR(m_valueBuff.c_str()), newSize );
	}

	void setBufferSize( size_t size )
	{
		if( size > m_minSize )		// never shrink the buffer
		{
			m_minSize = size;
			m_valueBuff.release();	// do not copy the existing buffer -> create a new one
									/// TODO: analyse what is faster: memory mnagement or block copying
			m_valueBuff.setMinSize( size+1 );
		}
	}
	size_t size() const
	{
		return m_minSize;
	}
	void *getBuffer() const
	{
		return (void *)m_valueBuff.c_str();
	}
	void setActSize( size_t size )
	{
		if( !size )
			m_valueBuff.release();
		else
		{
			if( !m_valueBuff.c_str()[size-1] )
				--size;
			m_valueBuff.setActSize(size);
		}
	}
	gak::STRING	GetValue()
	{
		return m_valueBuff;
	}
	static size_t GetValueSize(const gak::STRING &value)
	{
		return strlen(value)+1;
	}
};

template <typename INPUTTYPE>
struct InputTraits : public RegTraits<INPUTTYPE>
{
	const INPUTTYPE m_value;
	
	InputTraits(const INPUTTYPE &value) : m_value(value) {}

	size_t GetValueSize()
	{
		return sizeof(INPUTTYPE);
	}

	LPBYTE GetAdress() const
	{
		return LPBYTE(&m_value);
	}
};

template <>
struct InputTraits<gak::STRING> : public RegTraits<gak::STRING>
{
	const gak::STRING &m_value;
	
	InputTraits(const gak::STRING &value) : m_value(value) {}

	size_t GetValueSize()
	{
		return m_value.isNullPtr() ? 0 : m_value.strlen()+1;
	}

	LPBYTE GetAdress() const
	{
		return LPBYTE(m_value.c_str());
	}
};

class Registry : public gak::CopyProtection
{
	protected:
	HKEY	m_key;
	long	m_perm;

	private:
	long createKey2( HKEY parent, const char *name, unsigned long perm );
	long openKey2( HKEY parent, const char *name, unsigned long perm );

	static RegistryType queryValue( HKEY key, const char *var, void *buffer, size_t *io_size );

	static size_t getValueSize(HKEY key, const char *var)
	{
		DWORD		valueSize = 0;
		long		openResult = RegQueryValueEx(
			key, var, nullptr, nullptr, nullptr, &valueSize
		);
		return openResult == ERROR_SUCCESS ? valueSize : 0;
	}

	template <class RESULTTYPE>
	static ReadSuccess readValue( HKEY key, const char *var, RESULTTYPE *result, long perm );

	template <class INPUTTYPE>
	static long writeValue( HKEY key, const char *var, const INPUTTYPE &i_value )
	{
		typedef InputTraits<INPUTTYPE>	tInputTraits;

		tInputTraits	value(i_value);
		DWORD			valueSize = DWORD(value.GetValueSize());
		assert(tInputTraits::s_registryType > 0);
		return RegSetValueEx( key, var, 0, tInputTraits::s_registryType, value.GetAdress(), valueSize );
	}

	static long setValue( HKEY key, const char *var, RegistryType type, const void *data, size_t len )
	{
		// Unike ...Ex RegSetValue creates a new key with a nameless value
		return RegSetValue( key, var, type, LPCSTR(data), DWORD(len) );
	}
	static long setValueEx( HKEY key, const char *var, RegistryType type, const void *data, size_t len )
	{
		// Unike ...Ex RegSetValue creates a new key with a nameless value
		return RegSetValueEx( key, var, 0, type, LPBYTE(data), DWORD(len) );
	}

	public:
	Registry() : m_key(0), m_perm(0) {}
	~Registry()
	{
		if( m_key && m_perm )
		{
			RegCloseKey( m_key );
		}
	}
	operator bool () const
	{
		return m_key != 0;
	}

	/*
		creating new keys
	*/
	/// create a public key under Local HKEY_LOCAL_MACHINE
	long createPublic( const char *name, unsigned long perm=KEY_ALL_ACCESS )
	{
		return createKey2( HKEY_LOCAL_MACHINE, name, perm );
	}
	/// create a private key under Local HKEY_CURRENT_USER
	long createPrivate( const char *name, unsigned long perm=KEY_ALL_ACCESS )
	{
		return createKey2( HKEY_CURRENT_USER, name, perm );
	}
	/// open a public or private key 
	long createKey( bool publicKey, const char *name, unsigned long perm=KEY_ALL_ACCESS )
	{
		return publicKey ? createPublic( name, perm ) : createPrivate( name, perm );
	}
	/// create a sub key under an open key
	long createKey( const Registry &parent, const char *name, unsigned long perm=KEY_ALL_ACCESS )
	{
		return createKey2( parent.m_key, name, perm );
	}

	/*
		Opening existing keys
	*/
	/// open a private key from Local HKEY_CURRENT_USER
	long openPrivate( const char *name, unsigned long perm=KEY_READ )
	{
		return openKey2( HKEY_CURRENT_USER, name, perm );
	}

	/// open a public key from Local HKEY_LOCAL_MACHINE
	long openPublic( const char *name, unsigned long perm=KEY_READ )
	{
		return openKey2( HKEY_LOCAL_MACHINE, name, perm );
	}

	/// open a public or private key 
	long openKey( bool publicKey, const char *name, unsigned long perm=KEY_READ )
	{
		return publicKey ? openPublic( name, perm ) : openPrivate( name, perm );
	}

	/// open a sub key from an open key
	long openSubkey( const Registry &parent, const char *name, unsigned long perm=KEY_READ )
	{
		return openKey2( parent.m_key, name, perm );
	}


	/*
		deleting existing keys/values
	*/
#ifndef __BORLANDC__		// my old Borland C++ Builder does not know that windows function
	/// delete a sub key you may decide to delete the 64 or 32 bit path
	/// TODO: find out whether it is usefull to decide between a 64 and 32 bit path
	long deleteSubkeyEx( const char *keyName, unsigned long perm )  const
	{
		return RegDeleteKeyEx( m_key, keyName, perm, 0 );
	}
#endif
	/// delete a sub key
	long deleteSubkey( const char *keyName )  const
	{
		return RegDeleteKey( m_key, keyName );
	}
	/// delete a value
	long deleteValue( const char *name )  const
	{
		return RegDeleteValue( m_key, name );
	}


	/*
		Reading values whithout allocation
		These function do not allocate the buffer
	*/
	/// load a value into an existing buffer
	RegistryType queryValue( const char *var, void *buffer, size_t *io_size ) const
	{
		return queryValue( m_key, var, buffer, io_size );
	}
	/// load a nameless value into an existing buffer
	RegistryType queryValue( void *buffer, size_t *io_size ) const
	{
		return queryValue( m_key, nullptr, buffer, io_size );
	}
	/// get the content size of a value
	size_t getValueSize(const char *var) const
	{
		return getValueSize(m_key, var);
	}

	/*
		Reading values whith allocation
	*/
	/// reading a value from any key
	template <class RESULTTYPE>
	ReadSuccess readValue( const char *var, RESULTTYPE *result ) const
	{
		return readValue( m_key, var, result, m_perm );
	}

	/// reading a nameless value from any key
	template <class RESULTTYPE>
	ReadSuccess readValue( RESULTTYPE *result ) const
	{
		return readValue( m_key, nullptr, result, m_perm );
	}

	/*
		Writing
	*/

	/// write a value of any type
	template <class INPUTTYPE>
	long writeValue( const char *var, const INPUTTYPE &i_value ) const
	{
		return writeValue( m_key, var, i_value );
	}

	/*
		legacy writing
	*/
	/// write a key with unamed value given type and size
	long setValue( const char *var, RegistryType type, const void *data, size_t len ) const
	{
		return setValue( m_key, var, type, data, len );
	}
	/// write an unnamed value with given type and size
	long setValue( RegistryType type, const void *data, size_t len ) const
	{
		return setValue( m_key, nullptr, type, data, len );
	}

	/// write a key with unamed text value
	long setValue( const char *var, const gak::STRING &data ) const
	{
		return setValue( m_key, var, rtSTRING, data.c_str(), data.size()+1 );
	}
	/// write an unnamed text value
	long setValue( const gak::STRING &data  ) const
	{
		return setValue( m_key, nullptr, rtSTRING, data.c_str(), data.size()+1 );
	}

	/*
		querying key and value names
	*/
	void getKeyNames( gak::ArrayOfStrings *keyNames );
	void getValueNames( gak::ArrayOfStrings *valueNames );

	// for testing, only can be used to create values of type REG_EXPAND_SZ
	/// TODO create an api that allows to create/write/read REG_EXPAND_SZ values actually when reading Strings and we find rtENV
	/// we expand the env-variables and return a string 
	long setValueEx( const char *var, RegistryType type, const void *data, size_t len ) const
	{
		return setValueEx( m_key, var, type, data, len );
	}
#if 0
	/// this method is useful, when migrating old code that still uses the windows registry api
	HKEY get() const
	{
		return m_key;
	}
#endif
};

class RegistryClassesRoot : public Registry
{
	public:
	RegistryClassesRoot()
	{
		m_key = HKEY_CLASSES_ROOT;
	}
	~RegistryClassesRoot()
	{
		m_key = 0;
	}
};

class RegistryLocalMachine : public Registry
{
	public:
	RegistryLocalMachine()
	{
		m_key = HKEY_LOCAL_MACHINE;
	}
	~RegistryLocalMachine()
	{
		m_key = 0;
	}
};

class RegistryCurrentUser : public Registry
{
	public:
	RegistryCurrentUser()
	{
		m_key = HKEY_CURRENT_USER;
	}
	~RegistryCurrentUser()
	{
		m_key = 0;
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

template <class RESULTTYPE>
ReadSuccess Registry::readValue( HKEY key, const char *var, RESULTTYPE *result, long perm )
{
	typedef ResultTraits<RESULTTYPE>	tResultTraits;

	tResultTraits	valueBuff;
	ReadSuccess		retCode = rsNotFound;

	if( !tResultTraits::s_fixBufferSize )
	{
		size_t size = getValueSize( key, var );
		if( size )
			valueBuff.setBufferSize(size+1);	// reserve 1 byte extra space for the trailing 0 byte
		else
		{
			*result = RESULTTYPE();
			retCode = rsOK;
		}
	}
	size_t valueSize = valueBuff.size();
	if( valueSize )
	{
		RegistryType	typeVar = queryValue( key, var, valueBuff.getBuffer(), &valueSize );

		if( typeVar == rtENV && rtSTRING == tResultTraits::s_registryType )
		{
			valueSize = valueBuff.expand( valueSize );
			typeVar = rtSTRING;
		}

		if( valueSize > 0
		&&  typeVar == tResultTraits::s_registryType )
		{
			if( !tResultTraits::s_fixBufferSize )
			{
				valueBuff.setActSize(valueSize);
				retCode = rsOK;
			}
			else if(valueSize == valueBuff.size() )
			{
				retCode = rsOK;
			}
			else
			{
				retCode = rsBadSize;
			}
			if( retCode == rsOK )
			{
				*result = valueBuff.GetValue();
			}
		}
		else if( var && typeVar == rtMISSING )
		{
			Registry	subKey;
			if( subKey.openKey2( key, var, perm ) == ERROR_SUCCESS )
			{
				retCode = readValue(subKey.m_key, nullptr, result, perm );
			}
		}
		else if( typeVar == rtERROR )
			retCode = rsError;
		else
			retCode = rsBadType;
	}

	return retCode;
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

} // namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif //  GAK_WINLIB_REGISTRY_H