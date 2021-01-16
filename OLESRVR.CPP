/*
		Project:		Windows Class Library
		Module:			olesrvr.cpp
		Description:	ole related functions (ole server)
		Author:			Martin Gäckler
		Address:		Landsberger Str. 68, D-886938 Schondorf, Germany
		Phone:			0049 - 08192 - 999 098
		Telefax:		0049 - 08192 - 999 099
		Date:			13. 03. 1993
		History:

*/

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib\olesrvr.h>

// --------------------------------------------------------------------- //
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

int				OLE_SERVER_WINDOW::usageCount    = 0;
OLESERVERVTBL	OLE_SERVER_WINDOW::oleServerVTBL;

// --------------------------------------------------------------------- //
// ----- static datas -------------------------------------------------- //
// --------------------------------------------------------------------- //

#if 0
static OLEDOCVTBL		oleDocumentVTBL;
static OLEOBJECTVTPL	oleObjectVTBL:
#endif

static LIST_CONTAINER	oleServerList;

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

static OLE_SERVER_WINDOW *findOleServerWindow( LPOLESERVER oleServer )
{
	OLE_SERVER_WINDOW	*window;
	LIST_CURSOR			searchBuffer;

	window = (OLE_SERVER_WINDOW *)oleServerList.getFirst( &searchBuffer );
	if( window ) do
	{
		if( window->getServer() == oleServer )
			break;

		window = (OLE_SERVER_WINDOW *)oleServerList.getNext( &searchBuffer );
	}
	while( window );

	return window;
}

/*
	Ole Server Functions
*/
extern "C"
OLESTATUS FAR PASCAL srvCreate( LPOLESERVER		lpolesrvr,
								LHSERVERDOC		lhdoc,
								OLE_LPCSTR		lpszClassName,
								OLE_LPCSTR		lpszDoc,
								LPOLESERVERDOC	FAR *lplpoledoc )
{
	OLE_SERVER_WINDOW	*oleServerWindow;
#if defined( __SMALL__ ) || defined( __MEDIUM__ )
	char	className[128],
			docName[256];

	lstrcpy( className, lpszClassName );
	lstrcpy( docName, lpszDoc );
#else
#	define className	lpszClassName
#	define docName		lpszDoc
#endif


	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Create( lhdoc,
										className,
										docName,
										lplpoledoc );
	else
		return OLE_ERROR_NEW;
}

extern "C"
OLESTATUS FAR PASCAL srvCreateFromTemplate( LPOLESERVER		lpolesrvr,
											LHSERVERDOC		lhdoc,
											OLE_LPCSTR		lpszClassName,
											OLE_LPCSTR		lpszDoc,
											OLE_LPCSTR		lpszTemplate,
											LPOLESERVERDOC	FAR *lplpoledoc )
{
	OLE_SERVER_WINDOW	*oleServerWindow;
#if defined( __SMALL__ ) || defined( __MEDIUM__ )
	char	className[128],
			docName[256],
			templateName[256];

	lstrcpy( className, lpszClassName );
	lstrcpy( docName, lpszDoc );
	lstrcpy( templateName, lpszTemplate );
#else
#	define className	lpszClassName
#	define docName		lpszDoc
#	define templateName	lpszTemplate
#endif

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->CreateFromTemplate( lhdoc,
													className,
													docName,
													templateName,
													lplpoledoc );
	else
		return OLE_ERROR_NEW;
}

extern "C"
OLESTATUS FAR PASCAL srvOpen( LPOLESERVER		lpolesrvr,
							  LHSERVERDOC		lhdoc,
							  OLE_LPCSTR		lpszDoc,
							  LPOLESERVERDOC	FAR *lplpoledoc )
{
	OLE_SERVER_WINDOW	*oleServerWindow;
#if defined( __SMALL__ ) || defined( __MEDIUM__ )
	char	docName[256];

	lstrcpy( docName, lpszDoc );
#else
#	define docName		lpszDoc
#endif

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Open( lhdoc,
									  docName,
									  lplpoledoc );
	else
		return OLE_ERROR_OPEN;
}


extern "C"
OLESTATUS FAR PASCAL srvEdit( LPOLESERVER		lpolesrvr,
							  LHSERVERDOC		lhdoc,
							  OLE_LPCSTR		lpszClassName,
							  OLE_LPCSTR		lpszDoc,
							  LPOLESERVERDOC	FAR *lplpoledoc )
{
	OLE_SERVER_WINDOW	*oleServerWindow;
#if defined( __SMALL__ ) || defined( __MEDIUM__ )
	char	className[128],
			docName[256];

	lstrcpy( className, lpszClassName );
	lstrcpy( docName, lpszDoc );
#else
#	define className	lpszClassName
#	define docName		lpszDoc
#endif

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Edit( lhdoc,
									  className,
									  docName,
									  lplpoledoc );
	else
		return OLE_ERROR_EDIT;
}

OLESTATUS FAR PASCAL srvExecute( LPOLESERVER	lpolesrvr,
								 HGLOBAL		hCommands )
{
	OLE_SERVER_WINDOW	*oleServerWindow;

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Execute( hCommands );
	else
		return OLE_ERROR_EDIT;
}

extern "C"
OLESTATUS FAR PASCAL srvExit( LPOLESERVER lpolesrvr )
{
	OLE_SERVER_WINDOW	*oleServerWindow;

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Exit();
	else
		return OLE_ERROR_OPEN;
}

extern "C"
OLESTATUS FAR PASCAL srvRelease( LPOLESERVER lpolesrvr )
{
	OLE_SERVER_WINDOW	*oleServerWindow;

	oleServerWindow = findOleServerWindow( lpolesrvr );
	if( oleServerWindow )
		return oleServerWindow->Release();
	else
		return OLE_ERROR_OPEN;
}

#if 0
/*
	Ole Document Functions
*/

static docClose()
{
}

static docGetObject()
{
}

static docExecute()
{
}

static docRelease()
{
}

static docSave()
{
}

static docSetColorScheme()
{
}

static docSetDocDimensions()
{
}

static docSetHostNames()
{
}

/*
	Ole Object Functions
*/

static objDoVerb()
{
}

static objEnumFormats()
{
}

static objGetData()
{
}

static objQueryProtocol()
{
}

static objRelease()
{
}

static objSetBounds()
{
}

static objSetColorScheme()
{
}

static objSetData()
{
}

static objSetTargetDevice()
{
}

static objShow()
{
}

#endif

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

BOOL OLE_SERVER_WINDOW::registerServer( HINSTANCE	hInstance,
										const char	*oleClass )
{
	// Server Functions
	if( !usageCount++ )
	{
		oleServerVTBL.Create             = srvCreate;
		oleServerVTBL.CreateFromTemplate = srvCreateFromTemplate;
		oleServerVTBL.Open               = srvOpen;
		oleServerVTBL.Edit               = srvEdit;
		oleServerVTBL.Execute            = srvExecute;
		oleServerVTBL.Exit               = srvExit;
		oleServerVTBL.Release            = srvRelease;
    }

#if 0
	// Document Functions
	oleDocumentVTBL.Close            = MakeProcInstance( docClose,
    													 hInstance );
	oleDocumentVTBL.GetObject        = MakeProcInstance( docGetObject,
    													 hInstance );
	oleDocumentVTBL.Execute          = MakeProcInstance( docExecute,
    													 hInstance );
	oleDocumentVTBL.Release          = MakeProcInstance( docRelease,
    													 hInstance );
	oleDocumentVTBL.Save             = MakeProcInstance( docSave,
    													 hInstance );
	oleDocumentVTBL.SetColorScheme   = MakeProcInstance( docSetColorScheme,
    													 hInstance );
	oleDocumentVTBL.SetDocDimensions = MakeProcInstance( docSetDocDimensions,
    													 hInstance );
	oleDocumentVTBL.SetHostNames     = MakeProcInstance( docSetHostNames,
														 hInstance );

	// Object Functions
	oleObjectVTBL.DoVerb             = MakeProcInstance( objDoVerb,
														 hInstance );
	oleObjectVTBL.EnumFormats        = MakeProcInstance( objEnumFormats,
														 hInstance );
	oleObjectVTBL.GetData            = MakeProcInstance( objGetData,
														 hInstance );
	oleObjectVTBL.QueryProtocol      = MakeProcInstance( objQueryProtocol,
														 hInstance );
	oleObjectVTBL.Release            = MakeProcInstance( objRelease,
														 hInstance );
	oleObjectVTBL.SetBounds          = MakeProcInstance( objSetBounds,
														 hInstance );
	oleObjectVTBL.SetColorScheme     = MakeProcInstance( objSetColorScheme,
														 hInstance );
	oleObjectVTBL.SetData            = MakeProcInstance( objSetData,
														 hInstance );
	oleObjectVTBL.SetTargetDevice    = MakeProcInstance( objSetTargetDevice,
														 hInstance );
	oleObjectVTBL.Show               = MakeProcInstance( objShow,
														 hInstance );
#endif

	server.lpvtbl = &oleServerVTBL;

	if( !registered )
	{
		oleServerList.appendObject( this );
		registered = TRUE;
	}
	return OleRegisterServer( oleClass,
							  &server,
							  &serverHandle,
							  hInstance,
							  OLE_SERVER_SINGLE ) == OLE_OK ? FALSE
															: TRUE;
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

