/*
		Project:		Windows Class Library
		Module:			LISTCONT.H
		Description:	Implementation of ListboxContainer
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

#ifndef LIST_CONTAINER_H
#define LIST_CONTAINER_H

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak\array.h>

#include <winlib\controlw.h>

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
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class ListObject
{
	STRING		value;

	public:
	ListObject()
	{
		value = "";
	}
	ListObject( const STRING &value )
	{
		setValue( value );
	}
	virtual ~ListObject() {}

	void setValue( const STRING &value )
	{
		this->value = value;
	}
	operator STRING ( void ) const
	{
		return value;
	}
};

class ListboxContainer
{
	typedef ListObject		*ListObjectPtr;

	ListBox						*control;
	gak::Array<ListObjectPtr>	data;

	public:
	ListboxContainer()
	{
		control = NULL;
	}
	ListboxContainer( ListBox *control )
	{
		setControl( control );
	}
	~ListboxContainer();

	void setControl( ListBox *control )
	{
		this->control = control;
	}

	size_t size( void ) const
	{
		return data.size();
	}
	ListObjectPtr &operator [] ( size_t pos )
	{
		return data[pos];
	}

	void addEntry( ListObjectPtr object )
	{
		data += object;
		if( control )
			control->addEntry( (STRING)*object );
	}
	ListObjectPtr	getSelectedEntry( void );
	void removeEntry( ListObjectPtr object );
};

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif

