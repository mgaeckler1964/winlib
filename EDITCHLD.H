/*
		Project:		Windows Class Library
		Module: 		EDITCHLD.H
		Description:	Declaration of EditChild a MDI-Child with Editcontrol
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
	gak::STRING	m_fileName;
	std::time_t	m_modifyTime;
	bool		m_autoReload;
	bool		m_withBOM;

	time_t getModifyTime() const
	{
		if( !m_fileName.isEmpty() )
		{
			struct stat	statbuf;

			if( !strStat( m_fileName, &statbuf ) )
/***/			return statbuf.st_mtime;
			else
/*@*/			return 0;
		}
		else
/*@*/		return 0;
	}

	virtual void handleTimer();
	public:
	EditChild() : ManagerChild( new MemoControl )
	{
		m_modifyTime = 0;
		m_withBOM = m_autoReload = false;
	}

	MemoControl *getControl() 
	{
		return static_cast<MemoControl*>(m_control);
	}

	bool &withBOM()
	{
		return m_withBOM;
	}
	void enableAutoReload()
	{
		m_autoReload = true;
		setTimer(1000);
	}
	void disableAutoReload()
	{
		m_autoReload = false;
		removeTimer();
	}
	bool autoReload() const
	{
		return m_autoReload;
	}
	private:
	void reloadFile( bool restorePosition )
	{
		size_t start, end, line, textLength;
		if( restorePosition )
		{
			line = getControl()->getFirstVisible();
			getSelection(&start, &end);
			textLength = getControl()->getTextLength();
		}
		EDIT::EditCharset	cs1 = getControl()->getCharset();
		getControl()->readFromFile( m_fileName );
		EDIT::EditCharset	cs2 = getControl()->getCharset();
		if( cs1 != EDIT::csUTF8 && cs2 == EDIT::csUTF8 )
		{
			withBOM() = true;
		}
		if( restorePosition )
		{
			getControl()->setFirstVisible(line);
			if( start != end || end <  textLength )
			{
				setSelection(start, end);
			}
			else
			{
				textLength = getControl()->getTextLength();
				setSelection(textLength, textLength);
			}
		}

		storeModifyTime();
	}
	public:
	void reloadFile()
	{
		reloadFile(true);
	}
	void readFromFile( const char *fileName )
	{
		m_fileName = fileName;
		reloadFile(false);
	}
	bool saveToFile( const char *fileName=nullptr )
	{
		bool errorFlag;

		if( fileName )
		{
			m_fileName = fileName;
			setTitle( fileName );
		}
		errorFlag = getControl()->saveToFile( m_fileName, m_withBOM );

		storeModifyTime();

		return errorFlag;
	}
	bool checkModifyTime()
	{
		return (m_modifyTime != 0 && m_modifyTime != getModifyTime()) ? true : false;
	}
	void storeModifyTime()
	{
		m_modifyTime = getModifyTime();
	}
	void setFont( Font &font )
	{
		static_cast<EDIT*>(m_control)->setFont( font );
	}
	void setTabWidth( unsigned int width )
	{
		static_cast<EDIT*>(m_control)->setTabWidth( width );
	}
	void setEditable()
	{
		static_cast<EDIT*>(m_control)->setEditable();
	}
	void setReadonly()
	{
		static_cast<EDIT*>(m_control)->setReadonly();
	}
	void setBackgroundColorByRef( COLORREF color )
	{
		static_cast<EDIT*>(m_control)->setBackgroundColorByRef(color);
	}
	bool isEditable() const
	{
		return static_cast<EDIT*>(m_control)->isEditable();
	}
	bool isChanged() const
	{
		return static_cast<EDIT*>(m_control)->isChanged();
	}
	void setChanged()
	{
		static_cast<EDIT*>(m_control)->setChanged();
	}

	void setLineEnd( RLINE_ENDS newLineEnd )
	{
		static_cast<EDIT*>(m_control)->setLineEnd(newLineEnd);
	}
	RLINE_ENDS getLineEnd() const
	{
		return static_cast<EDIT*>(m_control)->getLineEnd();
	}
/*
	void setOemConvert() const
	{
		static_cast<EDIT*>(m_control)->setOemConvert();
	}
	void clearOemConvert() const
	{
		static_cast<EDIT*>(m_control)->clearOemConvert();
	}
*/
	void setCharset( EDIT::EditCharset newCharset )
	{
		static_cast<EDIT*>(m_control)->setCharset(newCharset);
	}
	void convertCharset( EDIT::EditCharset newCharset )
	{
		static_cast<EDIT*>(m_control)->convertCharset(newCharset);
	}

	bool isOemConvert() const
	{
		static_cast<EDIT*>(m_control)->isOemConvert();
	}
	enum EDIT::EditCharset getCharset() const
	{
		return static_cast<EDIT*>(m_control)->getCharset();
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
		return static_cast<EDIT*>(m_control)->searchText(
			text, startPos, wholeWord, matchCase, downSearch, replaceText, all
		);
	}

	void replaceSelection( const char *replace )
	{
		static_cast<EDIT*>(m_control)->replaceSelection( replace );
	}
	const STRING &getFileName() const
	{
		return m_fileName;
	}
	void cut()
	{
		static_cast<EDIT*>(m_control)->cut();
	}
	void copy()
	{
		static_cast<EDIT*>(m_control)->copy();
	}
	void paste()
	{
		static_cast<EDIT*>(m_control)->paste();
	}
	void clear()
	{
		static_cast<EDIT*>(m_control)->clear();
	}
	void selectAll()
	{
		static_cast<EDIT*>(m_control)->selectAll();
	}
	void deleteLine()
	{
		static_cast<EDIT*>(m_control)->deleteLine();
	}
	void undo()
	{
		static_cast<EDIT*>(m_control)->undo();
	}
	void print( PrintDevice *device ) const
	{
		STRING	fileName = getFileName();

		static_cast<EDIT*>(m_control)->print( fileName, device );
	}
	void setLineBreak()
	{
		static_cast<EDIT*>(m_control)->setLineBreak();
	}
	void clrLineBreak()
	{
		static_cast<EDIT*>(m_control)->clrLineBreak();
	}
	bool isLineBreak() const
	{
		return static_cast<EDIT*>(m_control)->isLineBreak();
	}
	void setSelection( size_t startPos, size_t endPos )
	{
		static_cast<EDIT*>(m_control)->setSelection( startPos, endPos );
	}
	STRING getSelection() const
	{
		return static_cast<EDIT*>(m_control)->getSelection();
	}
	void getSelection( size_t *startPos, size_t *endPos = NULL ) const
	{
		static_cast<EDIT*>(m_control)->getSelection( startPos, endPos );
	}
	void getCursorPosition( size_t *line, size_t *col ) const
	{
		static_cast<EDIT*>(m_control)->getCursorPosition( line, col );
	}
	operator STRING() const
	{
		return STRING(*static_cast<EDIT*>(m_control));
	}
	STRING getText() const
	{
		return m_control->getText();
	}
	void setText( const STRING &text )
	{
		m_control->setText( text );
	}
	void setTitle( const STRING &text )
	{
		ManagerChild::setText( text );
	}
	void setText( int textId )
	{
		m_control->setText( textId );
	}
	void setTitle( int textId )
	{
		ManagerChild::setText( textId );
	}

	void indent()
	{
		static_cast<EDIT*>(m_control)->indent();
	}
	void unindent()
	{
		static_cast<EDIT*>(m_control)->unindent();
	}

	void setAutoIndent()
	{
		static_cast<EDIT*>(m_control)->setAutoIndent();
	}
	void clrAutoIndent()
	{
		static_cast<EDIT*>(m_control)->clrAutoIndent();
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
