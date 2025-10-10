/*
		Project:		Windows Class Library
		Module:			menu.h
		Description:	Define menu bars
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

#ifndef WINLIB_MENU_H
#define WINLIB_MENU_H

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

#include <Windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/xml.h>

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

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class Menu
{
	HMENU	m_hMenu;

	Menu( const Menu &src );
	const Menu & operator = ( const Menu &src );

	bool setMenu( HMENU newMenu )
	{
		if( m_hMenu )
			DestroyMenu( m_hMenu );
		m_hMenu = newMenu;

		return newMenu == nullptr; 
	}

	virtual HMENU createMenu();

	HMENU createMenu( gak::xml::Element *root );

	Menu( HMENU menu )
	{
		m_hMenu = menu;
	}
	public:
	Menu()
	{
		m_hMenu = nullptr;
	}
	~Menu()
	{
		if( m_hMenu )
			DestroyMenu( m_hMenu );
	}
	Menu *getSubMenu( int pos )const
	{
		return new Menu( GetSubMenu( getMenu(), pos ) );
	}
	HMENU getMenu() const
	{
		return m_hMenu;
	}
	bool load( HINSTANCE hInstance, const char *menuName )
	{
		return setMenu( LoadMenu( hInstance, menuName ) );
	}
	bool load( HINSTANCE hInstance, int menuId )
	{
		return load( hInstance, MAKEINTRESOURCE( menuId ) );
	}
	bool load( gak::xml::Element *resource )
	{
		if( !resource )
			return true;

		return setMenu( createMenu( resource ) );
	}
	void enableMenuItem( WORD item ) const
	{
		EnableMenuItem( getMenu(), item, MF_BYCOMMAND|MF_ENABLED );
	}
	void disableMenuItem( WORD item ) const
	{
		EnableMenuItem( getMenu(), item, MF_BYCOMMAND|MF_GRAYED );
	}
	void enableMenuItem( WORD item, bool flag ) const
	{
		EnableMenuItem( getMenu(), item, flag ? MF_BYCOMMAND|MF_ENABLED : MF_BYCOMMAND|MF_GRAYED );
	}
	void checkMenuItem( WORD item ) const
	{
		CheckMenuItem( getMenu(), item, MF_BYCOMMAND|MF_CHECKED );
	}
	void uncheckMenuItem( WORD item ) const
	{
		CheckMenuItem( getMenu(), item, MF_BYCOMMAND|MF_UNCHECKED );
	}
	void addMenuItem( int popupPos, int id, const char *text, int flags = MF_STRING ) const
	{
		HMENU	subMenu = GetSubMenu( getMenu(), popupPos );

		AppendMenu( subMenu, flags, id, text );
	}
	void removeMenuItem( int popupPos, int id ) const
	{
		HMENU	subMenu = GetSubMenu( getMenu(), popupPos );

		RemoveMenu( subMenu, id, MF_BYCOMMAND );
	}
};

class PopupMenu : public Menu
{
	virtual HMENU createMenu();
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

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif