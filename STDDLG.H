/*
		Project:		Windows Class Library
		Module: 		STDDLG.H
		Description:	Windows Standard Dialogs
		Author:			Martin Gäckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2021 Martin Gäckler

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
	OPENFILENAME	ofn;
	char			*fileBuffer;
	STRING			title,
					filter,
					directory,
					regName;

	protected:
	FileDialog();
	~FileDialog();

	template <typename IdType>
	void init( const BasicWindow *parent, int titleId, IdType filterId, int numFilter );
	void storeDirectory( void )
	{
		if( fileBuffer && *fileBuffer )
		{
			directory = fileBuffer;
			size_t slashPos = directory.searchRChar( DIRECTORY_DELIMITER );
			if( slashPos != -1 )
			{
				directory.cut( slashPos );
				ofn.lpstrInitialDir = (const char *)directory;
				if( !regName.isEmpty() )
					appObject->WriteProfile( false, "lastDirectory", regName, directory );
			}
			else
			{
				directory = (const char *)NULL;
				ofn.lpstrInitialDir = NULL;
			}
		}
		else
		{
			directory = (const char *)NULL;
			ofn.lpstrInitialDir = NULL;
		}
	}
	public:
	long getFilterIndex( void ) const
	{
		return ofn.nFilterIndex;
	}
	void setFilterIndex( long idx )
	{
		ofn.nFilterIndex = idx;
	}

	const char *getFilename( void ) const
	{
		return fileBuffer;
	}
	void setFilename( const STRING &filename )
	{
		size_t	len = filename.strlen();

		if( len > ofn.nMaxFile-1 )
		{
			fileBuffer		= (char *)realloc( fileBuffer, 2*strlen( filename ) );
			ofn.lpstrFile	= fileBuffer;
			ofn.nMaxFile	= DWORD(2*strlen( filename )-1);
		}
		if( len )
		{
			size_t	slashPos = filename.searchRChar( DIRECTORY_DELIMITER );
			if( slashPos != -1 )
			{
				directory = filename.leftString( slashPos );
				ofn.lpstrInitialDir = (const char *)directory;
				strcpy( fileBuffer, filename + (size_t)(slashPos + 1) );
			}
			else
			{
				directory = (const char *)NULL;
				ofn.lpstrInitialDir = NULL;
				strcpy( fileBuffer, filename );
			}
		}
		else
		{
			*fileBuffer = 0;
			directory = (const char *)NULL;
			ofn.lpstrInitialDir = NULL;
		}
	}
	const STRING &getDirectory( void ) const
	{
		return directory;
	}
	void setDirectory( const STRING &newDirectory )
	{
		directory = newDirectory;
		ofn.lpstrInitialDir = (const char *)directory;
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
		this->regName = regName;
		STRING lastDirectory = appObject->GetProfile( "lastDirectory", regName, "" );
		if( !lastDirectory.isEmpty() )
			setDirectory( lastDirectory );
		else
			setDefaultDirectory( regName );
	}
	void setPersonalFolder( void )
	{
		setLastDirectory( "Personal" );
	}
	void setPersonalVideos( void )
	{
		setLastDirectory( "My Video" );
	}
	void setPersonalMusic( void )
	{
		setLastDirectory( "My Music" );
	}
	void setPersonalPictures( void )
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
	CHOOSECOLOR cc;
	COLORREF	palette[16];

	public:
	ColorDialog()
	{
		memset( &cc, 0, sizeof( cc ) );
		cc.lStructSize = sizeof( cc );
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
	FINDREPLACE 	findInfo;
	char			findText[128];

	protected:
	SearchDialog();
	public:
	static SearchDialog *create( void )
	{
		return new SearchDialog;
	}
	bool create( const CallbackWindow *parent, const char *text, bool wholeWord, bool matchCase, bool downSearch );
	const char *getFindText( void ) const
	{
		return findText;
	}
};

class ReplaceDialog : protected SearchDialog
{
	private:
	char			replaceText[128];

	private:
	ReplaceDialog();
	public:
	static ReplaceDialog *create( void )
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
	const char *getReplaceText( void ) const
	{
		return replaceText;
	}
};

class PrinterDialog
{
	private:
	PRINTDLG	printDlg;

	public:
	PrinterDialog()
	{
		memset( &printDlg, 0, sizeof( printDlg ) );
		printDlg.lStructSize = sizeof( printDlg );
	}
	bool	create( const BasicWindow *parent );
	PrintDevice	*getPrinter( void ) const
	{
		PrintDevice *printer = new PrintDevice( printDlg.hDC );
		return printer;
	}

};

class FontDialog
{
	friend class Font;

	CHOOSEFONT	fontDlg;

	FontDialog( LOGFONT *font, unsigned long Flags )
	{
		memset( &fontDlg, 0, sizeof( fontDlg ) );
		fontDlg.lStructSize = sizeof( fontDlg );

		fontDlg.Flags = Flags;
		fontDlg.lpLogFont = font;
	}

	bool create( const BasicWindow *parent );
};


template <typename IdType>
void FileDialog::init( const BasicWindow *parent, int titleId, IdType filterID, int numFilter )
{
	title = titleId ? appObject->loadString( titleId ) : NULL_STRING;
	filter = NULL_STRING;

	if( filterID )
	{
		RessourceIDs<IdType>	ressourceID( filterID );
		while( numFilter )
		{
			filter += appObject->loadString( ressourceID.getNextID() );
			numFilter--;
		}
		if( !filter.isEmpty() )
		{
			char delimiter = filter.lastChar();
			for( char *cp=const_cast<char*>(static_cast<const char *>(filter)); *cp ; cp++ )
			{
				if( *cp == delimiter )
				{
					*cp = 0;
				}
			}
		}
	}

	ofn.hwndOwner	 = parent->handle();
	ofn.lpstrTitle	 = title.isNullPtr() ? (const char *)NULL : (const char *)title;
	ofn.lpstrFilter  = filter.isNullPtr() ? (const char *)NULL : (const char *)filter;
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

	ofn.Flags = flags;

	bool okClicked = bool(GetOpenFileName( &ofn ));
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

	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_LONGNAMES;
	if( defaultExtensions.size() )
	{
		ofn.Flags |= OFN_ENABLEHOOK;
		ofn.lpfnHook = OFNHookProc;
		ofn.lCustData = (long)this;
	}

	bool	okClicked = (bool)GetSaveFileName( &ofn );
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
