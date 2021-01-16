/*
		Project:		Windows Class Library
		Module:			olesvr.h
		Description:	Defines an OLE server (not complete)
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

#ifndef OLE_SERVER_H
#define OLE_SERVER_H

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

#include <windows.h>
#include <ole.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak\list.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -a4
#	pragma option -pc
#endif

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class OLE_SERVER_DOCUMENT
{
	private:
	static
	int					usageCount;
	static
	OLESERVERDOCVTBL	oleDocumentVTBL;
};

class OLE_SERVER_WINDOW : public DOUBLE_LIST
{
	private:
	static
	int				usageCount;
	static
	OLESERVERVTBL	oleServerVTBL;

	BOOL			registered;
	OLESERVER		server;
	LHSERVER		serverHandle;

	public:
	OLE_SERVER_WINDOW()
	{
		registered = FALSE;
	}
	registerServer( HINSTANCE hInstance, const char *className );
	const OLESERVER *getServer( void ) const
	{
		return &server;
	};

	// The server functions (they must be defined by the derived classes)
	virtual OLESTATUS Create(				LHSERVERDOC,
											const char *,
											const char *,
											LPOLESERVERDOC FAR * ) = 0;
	virtual OLESTATUS CreateFromTemplate(	LHSERVERDOC,
											const char *,
											const char *,
											const char *,
											LPOLESERVERDOC FAR * ) = 0;
	virtual OLESTATUS Open(					LHSERVERDOC,
											const char *,
											LPOLESERVERDOC FAR * ) = 0;
	virtual OLESTATUS Edit(					LHSERVERDOC,
											const char *,
											const char *,
											LPOLESERVERDOC FAR *)  = 0;
	virtual OLESTATUS Execute(				HGLOBAL )              = 0;
	virtual OLESTATUS Exit( 				void )                 = 0;
	virtual OLESTATUS Release( 				void )                 = 0;

	// the document functions
};


#ifdef __BORLANDC__
#	pragma option -p.
#	pragma option -a.
#endif

#endif