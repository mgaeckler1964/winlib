/*
		Project:		Windows Class Library
		Module:			RAW_DATA.H
		Description:	Used to store application specific data in a windows 
						clipboard
		Author:			Martin Gäckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Germany, Munich ``AS IS''
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

#ifndef MEMORY_H
#define MEMORY_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <stdio.h>

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/gaklib.h>
#include <gak/array.h>
#include <gak/memoryStream.h>

#include <WINLIB\BASICWIN.H>


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
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

template <class OBJ> 
class WindowsAllocator
{
	public:
	struct MemHandle
	{
		HGLOBAL	memHandle;
		OBJ		*data;
		
		MemHandle( HGLOBAL newHandle=0 ) : memHandle( newHandle )
		{
			data = newHandle ? static_cast<OBJ*>(GlobalLock( newHandle )) : NULL;
		}
		operator bool ( void ) const
		{
			return data != 0;
		}
#if 1
		// TODO remove and add realloc to allocator
		OBJ &operator [] ( size_t idx )
		{
			return data[idx];
		}
#endif
	};
	static MemHandle alloc( size_t count )
	{
		return MemHandle( GlobalAlloc( GMEM_MOVEABLE, count*sizeof( OBJ ) ) );
	}
	static void release( const MemHandle &data )
	{
		HGLOBAL	memHndl = getHandle( data );
		OBJ		*ptr = getData( data );
		if( memHndl )
		{
			if( ptr )
				GlobalUnlock( memHndl );
			GlobalFree( memHndl );
		}
	}
	static HGLOBAL getHandle( const MemHandle &data )
	{
		return data.memHandle;
	}
	static OBJ *getData( const MemHandle &data )
	{
		return data.data;
	}
};

template<class OBJ>
class BasicWindowsBuffer : public gak::Array<OBJ, WindowsAllocator<OBJ> >
{
	friend class Clipboard;
};

typedef BasicWindowsBuffer<char>					WindowsBuffer;
typedef gak::BasicOMemoryStream< WindowsBuffer >	oWindowsStream;
typedef gak::BasicIMemoryStream< WindowsBuffer >	iWindowsStream;

template <typename OBJ>
class BasicClipboardMemory
{
	private:
	HGLOBAL			memHndl;
	const OBJ		*data;
	size_t			blockSize;

	const OBJ *getDataBuffer( void )
	{
		if( !data && memHndl )
			data = static_cast<const OBJ *>(GlobalLock( memHndl ));

		return data;
	}
	BasicClipboardMemory( const BasicClipboardMemory &source );
	const BasicClipboardMemory & operator = ( const BasicClipboardMemory &source );

	public:
	typedef OBJ value_type;
	size_t size( void ) const
	{
		return blockSize;
	}

	BasicClipboardMemory( int clipFormat )
	{
		memHndl   = GetClipboardData( clipFormat );
		data = NULL;
		blockSize = GlobalSize( memHndl );
	}
	~BasicClipboardMemory()
	{
		if( data && memHndl )
			GlobalUnlock( memHndl );
	}
	bool is_open( void ) const
	{
		return memHndl ? true : false;
	}
	const OBJ &operator [] ( size_t idx )
	{
		return getDataBuffer()[idx];
	}
	void push_back( const OBJ &/*c*/ )
	{
		throw gak::WriteError();
	}
};

typedef BasicClipboardMemory<char>					ClipboardMemory;
typedef gak::BasicIMemoryStream< ClipboardMemory >	iClipboardStream;


class Clipboard
{
	bool open;

	Clipboard( const Clipboard &source );
	const Clipboard & operator = ( const Clipboard &source );

	public:
	Clipboard( const BasicWindow *owner, int clipFormat = 0 )
	{
		if( !clipFormat || hasFormat( clipFormat ) )
			open = OpenClipboard( owner->handle() ) ? true : false;
		else
			open = false;
	}
	~Clipboard()
	{
		if( open )
			CloseClipboard();
	}
	bool openClipboard( const BasicWindow *owner )
	{
		if( !open )
			open = OpenClipboard( owner->handle() ) ? true : false;
		return open;
	}

	operator bool ( void ) const
	{
		return open;
	}

	static int registerFormat( const char *name )
	{
		return RegisterClipboardFormat( name );
	}
	static bool hasFormat( int clipFormat )
	{
		return IsClipboardFormatAvailable( clipFormat ) ? true : false;
	}
	static void clear( void )
	{
		EmptyClipboard();
	}
	template <class OBJ>
	static void putToClipBoard( BasicWindowsBuffer<OBJ> &buffer, int clipFormat )
	{
		HGLOBAL	memHndl = WindowsAllocator<OBJ>::getHandle( buffer.getDataHandle() );
		OBJ		*data = buffer.getDataBuffer();
		if( memHndl )
		{
			if( data )
				GlobalUnlock( memHndl );
			memHndl = GlobalReAlloc( memHndl, buffer.size() * sizeof( OBJ ), 0 );
			SetClipboardData( clipFormat, memHndl );
			buffer.forget();
		}
	};
};

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
