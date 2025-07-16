/*
		Project:		Windows Class Library
		Module: 		EDITCHLD.H
		Description:	Declaration of EditChild a MDI-Child with Editcontrol
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

#ifndef EDIT_CHILD_H
#define EDIT_CHILD_H

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/string.h>
#include <gak/strFiles.h>

#include <winlib/manager.h>

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

class EditChild : public ManagerChild
{
	STRING	fileName;
	time_t	modifyTime;

	private:

	time_t getModifyTime( void ) const
	{
		if( !fileName.isEmpty() )
		{
			struct stat	statbuf;

			if( !strStat( fileName, &statbuf ) )
/***/			return statbuf.st_mtime;
			else
/*@*/			return 0;
		}
		else
/*@*/		return 0;
	}

	public:
	EditChild() : ManagerChild( new MemoControl )
	{
		modifyTime = 0;
	}

	MemoControl *getControl( void ) 
	{
		return static_cast<MemoControl*>(control);
	}

	void reloadFile( void )
	{
		getControl()->readFromFile( fileName );

		storeModifyTime();
	}
	void readFromFile( const char *fileName )
	{
		this->fileName = fileName;
		reloadFile();
	}
	bool saveToFile( const char *fileName = NULL )
	{
		bool errorFlag;

		if( fileName )
		{
			this->fileName = fileName;
			setTitle( fileName );
		}
		errorFlag = getControl()->saveToFile( this->fileName );

		storeModifyTime();

		return errorFlag;
	}
	bool checkModifyTime( void )
	{
		return (modifyTime != 0 && modifyTime != getModifyTime()) ? true : false;
	}
	void storeModifyTime( void )
	{
		modifyTime = getModifyTime();
	}
	void setFont( Font &font )
	{
		static_cast<EDIT*>(control)->setFont( font );
	}
	void setTabWidth( unsigned int width )
	{
		static_cast<EDIT*>(control)->setTabWidth( width );
	}
	void setEditable( void )
	{
		static_cast<EDIT*>(control)->setEditable();
	}
	void setReadonly( void )
	{
		static_cast<EDIT*>(control)->setReadonly();
	}
	bool isEditable( void ) const
	{
		return static_cast<EDIT*>(control)->isEditable();
	}
	bool isChanged( void ) const
	{
		return static_cast<EDIT*>(control)->isChanged();
	}
	void setChanged( void )
	{
		static_cast<EDIT*>(control)->setChanged();
	}

	void setLineEnd( RLINE_ENDS newLineEnd )
	{
		static_cast<EDIT*>(control)->setLineEnd(newLineEnd);
	}
	RLINE_ENDS getLineEnd( void ) const
	{
		return static_cast<EDIT*>(control)->getLineEnd();
	}
/*
	void setOemConvert( void ) const
	{
		static_cast<EDIT*>(control)->setOemConvert();
	}
	void clearOemConvert( void ) const
	{
		static_cast<EDIT*>(control)->clearOemConvert();
	}
*/
	void setCharset( enum EDIT::editCharset newCharset )
	{
		static_cast<EDIT*>(control)->setCharset(newCharset);
	}
	void convertCharset( enum EDIT::editCharset newCharset )
	{
		static_cast<EDIT*>(control)->convertCharset(newCharset);
	}

	bool isOemConvert( void ) const
	{
		static_cast<EDIT*>(control)->isOemConvert();
	}
	enum EDIT::editCharset getCharset( void ) const
	{
		return static_cast<EDIT*>(control)->getCharset();
	}

	size_t searchText(
		const char	*text,
		size_t		startPos,
		bool		wholeWord,
		bool		matchCase,
		bool		downSearch,
		const char	*replaceText=NULL,
		bool		all = false )
	{
		return static_cast<EDIT*>(control)->searchText(
			text, startPos, wholeWord, matchCase, downSearch, replaceText, all
		);
	}

	void replaceSelection( const char *replace )
	{
		static_cast<EDIT*>(control)->replaceSelection( replace );
	}
	STRING getFileName( void ) const
	{
		return fileName;
	}
	void cut( void )
	{
		static_cast<EDIT*>(control)->cut();
	}
	void copy( void )
	{
		static_cast<EDIT*>(control)->copy();
	}
	void paste( void )
	{
		static_cast<EDIT*>(control)->paste();
	}
	void clear( void )
	{
		static_cast<EDIT*>(control)->clear();
	}
	void selectAll( void )
	{
		static_cast<EDIT*>(control)->selectAll();
	}
	void deleteLine( void )
	{
		static_cast<EDIT*>(control)->deleteLine();
	}
	void undo( void )
	{
		static_cast<EDIT*>(control)->undo();
	}
	void print( PrintDevice *device ) const
	{
		STRING	fileName = getFileName();

		static_cast<EDIT*>(control)->print( fileName, device );
	}
	void setLineBreak( void )
	{
		static_cast<EDIT*>(control)->setLineBreak();
	}
	void clrLineBreak( void )
	{
		static_cast<EDIT*>(control)->clrLineBreak();
	}
	bool isLineBreak( void ) const
	{
		return static_cast<EDIT*>(control)->isLineBreak();
	}
	void setSelection( size_t startPos, size_t endPos )
	{
		static_cast<EDIT*>(control)->setSelection( startPos, endPos );
	}
	STRING getSelection( void ) const
	{
		return static_cast<EDIT*>(control)->getSelection();
	}
	void getSelection( size_t *startPos, size_t *endPos = NULL ) const
	{
		static_cast<EDIT*>(control)->getSelection( startPos, endPos );
	}
	void getCursorPosition( size_t *line, size_t *col ) const
	{
		static_cast<EDIT*>(control)->getCursorPosition( line, col );
	}
	operator STRING( void ) const
	{
		return STRING(*static_cast<EDIT*>(control));
	}
	STRING getText( void ) const
	{
		return control->getText();
	}
	void setText( const STRING &text )
	{
		control->setText( text );
	}
	void setTitle( const STRING &text )
	{
		ManagerChild::setText( text );
	}
	void setText( int textId )
	{
		control->setText( textId );
	}
	void setTitle( int textId )
	{
		ManagerChild::setText( textId );
	}

	void indent( void )
	{
		static_cast<EDIT*>(control)->indent();
	}
	void unindent( void )
	{
		static_cast<EDIT*>(control)->unindent();
	}

	void setAutoIndent( void )
	{
		static_cast<EDIT*>(control)->setAutoIndent();
	}
	void clrAutoIndent( void )
	{
		static_cast<EDIT*>(control)->clrAutoIndent();
	}
};

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
