/*
		Project:		Windows Class Library
		Module: 		STDDLG.H
		Description:	Windows Standard Dialogs
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

#ifndef STD_DLG_H
#define STD_DLG_H

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

#include <string.h>

#include <windows.h>
#include <commdlg.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <winlib/callwin.h>
#include <winlib/device.h>
#include <winlib/controlw.h>
#include <winlib/winapp.h>

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

template <typename IdType>
struct RessourceIDs
{
	RessourceIDs(IdType);
};

template <>
struct RessourceIDs<int>
{
	int		m_curID;

	RessourceIDs(int curID) : m_curID(curID) {}
	int getNextID( void )
	{
		return m_curID++;
	}
};

template <>
struct RessourceIDs<const int*>
{
	const int	*m_curID;

	RessourceIDs(const int *curID) : m_curID(curID) {}
	int getNextID( void )
	{
		return *m_curID++;
	}
};

class FileDialog : public BasicWindow
{
	protected:
	OPENFILENAME		m_ofn;
	gak::Buffer<char>	m_fileBuffer;
	STRING				m_title,
						m_filter,
						m_directory,
						m_regName;

	protected:
	FileDialog();

	template <typename IdType>
	void init( const BasicWindow *parent, int titleId, IdType filterId, int numFilter );
	void storeDirectory()
	{
		if( bool(m_fileBuffer) && *m_fileBuffer )
		{
			m_directory = (char*)m_fileBuffer;
			size_t slashPos = m_directory.searchRChar( DIRECTORY_DELIMITER );
			if( slashPos != -1 )
			{
				m_directory.cut( slashPos );
				m_ofn.lpstrInitialDir = (const char *)m_directory;
				if( !m_regName.isEmpty() )
					appObject->WriteProfile( false, "lastDirectory", m_regName, m_directory );
			}
			else
			{
				m_directory = (const char *)NULL;
				m_ofn.lpstrInitialDir = NULL;
			}
		}
		else
		{
			m_directory = (const char *)NULL;
			m_ofn.lpstrInitialDir = NULL;
		}
	}
	public:
	long getFilterIndex() const
	{
		return m_ofn.nFilterIndex;
	}
	void setFilterIndex( long idx )
	{
		m_ofn.nFilterIndex = idx;
	}

	const char *getFilename() const
	{
		return m_fileBuffer;
	}
	void setFilename( const STRING &filename )
	{
		size_t	len = filename.strlen();

		if( len > m_ofn.nMaxFile-1 )
		{
			m_fileBuffer.resize( 2*strlen( filename ) );
			m_ofn.lpstrFile	= m_fileBuffer;
			m_ofn.nMaxFile	= DWORD(2*strlen( filename )-1);
		}
		if( len )
		{
			size_t	slashPos = filename.searchRChar( DIRECTORY_DELIMITER );
			if( slashPos != -1 )
			{
				m_directory = filename.leftString( slashPos );
				m_ofn.lpstrInitialDir = (const char *)m_directory;
				strcpy( m_fileBuffer, filename + (size_t)(slashPos + 1) );
			}
			else
			{
				m_directory = (const char *)NULL;
				m_ofn.lpstrInitialDir = NULL;
				strcpy( m_fileBuffer, filename );
			}
		}
		else
		{
			*m_fileBuffer = 0;
			m_directory = (const char *)NULL;
			m_ofn.lpstrInitialDir = NULL;
		}
	}
	const STRING &getDirectory() const
	{
		return m_directory;
	}
	void setDirectory( const STRING &newDirectory )
	{
		m_directory = newDirectory;
		m_ofn.lpstrInitialDir = (const char *)m_directory;
	}
	void setDefaultDirectory( const STRING &regName )
	{
		HKEY	shellFolderKey;
		DWORD	type, size;
		char	path[1024];
		long	regResult;

		regResult = RegOpenKey(
			HKEY_CURRENT_USER,
			"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
			&shellFolderKey
		);
		if( regResult == ERROR_SUCCESS )
		{
			size = sizeof( path );
			type = REG_SZ;
			regResult = RegQueryValueEx(
				shellFolderKey, regName,
				NULL,
				&type, (LPBYTE)path, &size
			);
			if( regResult == ERROR_SUCCESS && type == REG_SZ )
			{
				setDirectory( path );
			}
			RegCloseKey( shellFolderKey );
		}
	}
	void setLastDirectory( const STRING &regName )
	{
		m_regName = regName;
		STRING lastDirectory = appObject->GetProfile( "lastDirectory", regName, "" );
		if( !lastDirectory.isEmpty() )
			setDirectory( lastDirectory );
		else
			setDefaultDirectory( regName );
	}
	void setPersonalFolder()
	{
		setLastDirectory( "Personal" );
	}
	void setPersonalVideos()
	{
		setLastDirectory( "My Video" );
	}
	void setPersonalMusic()
	{
		setLastDirectory( "My Music" );
	}
	void setPersonalPictures()
	{
		setLastDirectory( "My Pictures" );
	}

};

class OpenFileDialog : public FileDialog
{
	public:
	template <typename IdType>
	bool create(
		const BasicWindow	*parent,
		int					titleId = 0,
		IdType				filterId = 0,
		int					numIds = 1,
		unsigned long		flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY 
	);
};

class SaveFileAsDialog : public FileDialog
{
	ComboBox	  		filterList;
	EditControl	  		filenameEdit;
	gak::ArrayOfStrings	defaultExtensions;

	public:
	void handleMessage(
		UINT uiMsg,		// message identifier
		WPARAM wParam,	// message parameter
		LPARAM lParam 	// message parameter
	);

	SaveFileAsDialog();
	~SaveFileAsDialog();
	template <typename IdType>
	bool create(
		const BasicWindow	*parent,
		int					titleId = 0,
		IdType				filterId = 0,
		int					numIds = 1 
	);

	void addDefaultExtension( const STRING &ext )
	{
		defaultExtensions += ext;
	}
};

class ColorDialog
{
	private:
	CHOOSECOLOR m_cc;
	COLORREF	m_palette[16];

	public:
	ColorDialog()
	{
		memset( &m_cc, 0, sizeof( m_cc ) );
		m_cc.lStructSize = sizeof( m_cc );
	}

	bool create( const BasicWindow *parent, COLORREF *result );

	bool createWithPalette(
		const BasicWindow	*parent,
		COLORREF			*value,
		const char			*paletteName = NULL 
	);
};

class SearchDialog : public BasicWindow
{
	protected:
	FINDREPLACE 	m_findInfo;
	char			m_findText[256];

	protected:
	SearchDialog();
	public:
	static SearchDialog *create()
	{
		return new SearchDialog;
	}
	bool create( const CallbackWindow *parent, const char *text, bool wholeWord, bool matchCase, bool downSearch );
	const char *getFindText() const
	{
		return m_findText;
	}
};

class ReplaceDialog : protected SearchDialog
{
	private:
	char			m_replaceText[256];

	private:
	ReplaceDialog();
	public:
	static ReplaceDialog *create()
	{
		return new ReplaceDialog;
	}
	bool create(
		const CallbackWindow	*parent,
		const char				*findText,
		const char				*replaceText,
		bool					wholeWord,
		bool					matchCase
	);
	const char *getReplaceText() const
	{
		return m_replaceText;
	}
};

class PrinterDialog
{
	private:
	PRINTDLG	m_printDlg;

	public:
	PrinterDialog()
	{
		memset( &m_printDlg, 0, sizeof( m_printDlg ) );
		m_printDlg.lStructSize = sizeof( m_printDlg );
	}
	bool	create( const BasicWindow *parent );
	PrintDevice	*getPrinter() const
	{
		PrintDevice *printer = new PrintDevice( m_printDlg.hDC );
		return printer;
	}

};

class FontDialog
{
	friend class Font;

	CHOOSEFONT	m_fontDlg;

	FontDialog( LOGFONT *font, unsigned long Flags )
	{
		memset( &m_fontDlg, 0, sizeof( m_fontDlg ) );
		m_fontDlg.lStructSize = sizeof( m_fontDlg );

		m_fontDlg.Flags = Flags;
		m_fontDlg.lpLogFont = font;
	}

	bool create( const BasicWindow *parent );
};


template <typename IdType>
void FileDialog::init( const BasicWindow *parent, int titleId, IdType filterID, int numFilter )
{
	m_title = titleId ? appObject->loadString( titleId ) : NULL_STRING;
	m_filter = NULL_STRING;

	if( filterID )
	{
		RessourceIDs<IdType>	ressourceID( filterID );
		while( numFilter )
		{
			m_filter += appObject->loadString( ressourceID.getNextID() );
			numFilter--;
		}
		if( !m_filter.isEmpty() )
		{
			char delimiter = m_filter.lastChar();
			for( char *cp=const_cast<char*>(static_cast<const char *>(m_filter)); *cp ; cp++ )
			{
				if( *cp == delimiter )
				{
					*cp = 0;
				}
			}
		}
	}

	m_ofn.hwndOwner		= parent->handle();
	m_ofn.lpstrTitle	= m_title.isNullPtr() ? (const char *)NULL : (const char *)m_title;
	m_ofn.lpstrFilter	= m_filter.isNullPtr() ? (const char *)NULL : (const char *)m_filter;
}

template <typename IdType>
bool OpenFileDialog::create(
	const BasicWindow	*parent,
	int 				titleId,
	IdType				filterID,
	int 				numIds,
	DWORD				flags 
)
{
	FileDialog::init( parent, titleId, filterID, numIds );

	m_ofn.Flags = flags;

	bool okClicked = bool(GetOpenFileName( &m_ofn ));
	if( okClicked )
	{
		storeDirectory();
	}

	return okClicked;
}

template <typename IdType>
bool SaveFileAsDialog::create( 
	const BasicWindow	*parent,
	int					titleId,
	IdType				filterID,
	int					numIds 
)
{
	FileDialog::init( parent, titleId, filterID, numIds );

	m_ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_LONGNAMES;
	if( defaultExtensions.size() )
	{
		m_ofn.Flags |= OFN_ENABLEHOOK;
		m_ofn.lpfnHook = OFNHookProc;
		m_ofn.lCustData = (long)this;
	}

	bool	okClicked = (bool)GetSaveFileName( &m_ofn );
	if( okClicked )
	{
		storeDirectory();
	}

	return okClicked;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
