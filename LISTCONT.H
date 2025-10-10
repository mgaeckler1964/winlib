/*
		Project:		Windows Class Library
		Module:			LISTCONT.H
		Description:	Implementation of ListboxContainer
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin Gäckler

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
	STRING		m_value;

	public:
	ListObject()
	{
		m_value = "";
	}
	ListObject( const STRING &value )
	{
		setValue( value );
	}
	virtual ~ListObject() {}

	void setValue( const STRING &value )
	{
		m_value = value;
	}
	operator STRING () const
	{
		return m_value;
	}
};

class ListboxContainer
{
	typedef ListObject		*ListObjectPtr;

	ListBox						*m_control;
	gak::Array<ListObjectPtr>	m_data;

	public:
	ListboxContainer()
	{
		m_control = nullptr;
	}
	ListboxContainer( ListBox *control )
	{
		setControl( control );
	}
	~ListboxContainer();

	void setControl( ListBox *control )
	{
		m_control = control;
	}

	size_t size() const
	{
		return m_data.size();
	}
	ListObjectPtr &operator [] ( size_t pos )
	{
		return m_data[pos];
	}

	void addEntry( ListObjectPtr object )
	{
		m_data += object;
		if( m_control )
			m_control->addEntry( (STRING)*object );
	}
	ListObjectPtr	getSelectedEntry();
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

