/*
		Project:		Windows Class Library
		Module:			olesvr.h
		Description:	Defines an OLE server (not complete)
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

#include <memory>

#include <windows.h>
#include <ole2.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/string.h>
#include <WINLIB/DEVICE.H>
#include <WINLIB/colors.h>
#include <WINLIB/STREDIT.H>

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

#	define __RPC__in
#	define __RPC__out
#	define __RPC__in_opt
#	define __RPC__deref_out_opt
#	define __in
#	define __null
#	define __in_opt
#	define __deref_out
#	define __out
#	define __out_opt
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

static const int WIDTH = 3000;		// for the meta file
static const int HEIGHT = 1000;

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class OleBaseDocument : public IPersistStorage, public IOleObject, public IDataObject, public IViewObject2
{
	LONG	m_refCount;
	GUID	m_guid;

	unsigned long		m_viewAspects;
	unsigned long		m_viewAdvf;

	IDataAdviseHolder	*m_pDataAdviseHolder;
	IOleAdviseHolder	*m_pOleAdviseHolder;
	IOleClientSite		*m_pOleClientSite;
	IAdviseSink			*m_pViewAdviseSink;
	protected:

	IDataAdviseHolder *getDataAdviseHolder() const
	{
		return m_pDataAdviseHolder;
	}
	void clean()
	{
		if( m_pDataAdviseHolder )	
		{
			m_pDataAdviseHolder->Release();
			m_pDataAdviseHolder = nullptr;
		}
		if( m_pOleAdviseHolder )	
		{
			m_pOleAdviseHolder->Release();
			m_pOleAdviseHolder = nullptr;
		}
		if( m_pOleClientSite )		
		{
			m_pOleClientSite->Release();
			m_pOleClientSite = nullptr;
		}
		if( m_pViewAdviseSink )		
		{
			m_pViewAdviseSink->Release();
			m_pViewAdviseSink = nullptr;
		}
	}

	void OnDataModified()
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::OnDataModified");
		if ( m_pDataAdviseHolder )
		{
			doLogPositionEx( gakLogging::llInfo );
			m_pDataAdviseHolder->SendOnDataChange( this, 0, 0 );
		}
		if (m_pOleClientSite)
		{
			// Fordert den Container auf, unser IPersistStorage::Save aufzurufen
			m_pOleClientSite->SaveObject();
		}
		if( m_pOleAdviseHolder )
		{
			m_pOleAdviseHolder->SendOnSave();
		}
		if (m_pViewAdviseSink) 
		{
			m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
		}
	}


	public:
    OleBaseDocument(GUID guid) : m_guid(guid), m_refCount(1), m_viewAspects(0), m_viewAdvf(0), m_pDataAdviseHolder(nullptr), m_pOleAdviseHolder(nullptr), m_pOleClientSite(nullptr), 
		m_pViewAdviseSink(nullptr)
	{}
	~OleBaseDocument()
	{
		clean();
	}
    
	// ==========================================================
	// IUnknown 
	// ==========================================================
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleBaseDocument::QueryInterface");
		doLogPositionEx(gakLogging::llInfo);
		if (riid == IID_IUnknown)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = this;
		}
		else if (riid == IID_IPersistStorage)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = static_cast<IPersistStorage*>(this);
		}
		else if (riid == IID_IPersist)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = static_cast<IPersist*>(this);
		}
		else if (riid == IID_IOleObject)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = static_cast<IOleObject*>(this);
		}
		else if (riid == IID_IDataObject)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = static_cast<IDataObject*>(this);
		}
		else if (riid == IID_IViewObject || riid == IID_IViewObject2)
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = static_cast<IViewObject2*>(this);
		}
		else
		{
			doLogPositionEx(gakLogging::llInfo);
			*ppvObject = nullptr;
			return E_NOINTERFACE;
		}
		doLogPositionEx(gakLogging::llInfo);
		AddRef();
		return S_OK;
	}
	STDMETHODIMP_(ULONG) AddRef() 
	{ 
		doEnterFunctionEx(gakLogging::llInfo, "MyOleBaseDocument::AddRef");
		return InterlockedIncrement(&m_refCount); 
	}
	STDMETHODIMP_(ULONG) Release() 
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleBaseDocument::Release");
		LONG res = InterlockedDecrement(&m_refCount);
		if (res == 0) 
			delete this;
		return res;
	}


	// ==========================================================
	// IPersist
	// ==========================================================
	virtual HRESULT STDMETHODCALLTYPE GetClassID( 
	/* [out] */ __RPC__out CLSID *pClassID)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::GetClassID");
		*pClassID = m_guid;
		return S_OK;
	}

	// ==========================================================
	// IPersistStorage
	// ==========================================================

	virtual HRESULT STDMETHODCALLTYPE Load( 
	/* [unique][in] */ __RPC__in_opt IStorage *pStgLoad)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::Load");

		if (!pStgLoad) 
			return E_POINTER;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Save( 
	/* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
	/* [in] */ BOOL /*fSameAsLoad*/)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::Save");

		if (!pStgSave) return E_POINTER;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE IsDirty()
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE InitNew( 
	/* [unique][in] */ __RPC__in_opt IStorage * /*pStg*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return 0;
	}

	// Load and Save remain pure virtual

	virtual HRESULT STDMETHODCALLTYPE SaveCompleted( 
	/* [unique][in] */ __RPC__in_opt IStorage * /*pStgNew*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE HandsOffStorage()
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	// ==========================================================
	// IOleObject       
	// ==========================================================

	virtual HRESULT STDMETHODCALLTYPE DoVerb( 
	/* [in] */ LONG /*iVerb*/,
	/* [unique][in] */ __RPC__in_opt LPMSG /*lpmsg*/,
	/* [unique][in] */ __RPC__in_opt IOleClientSite *pActiveSite,
	/* [in] */ LONG /*lindex*/,
	/* [in] */ __RPC__in HWND /*hwndParent*/,
	/* [unique][in] */ __RPC__in_opt LPCRECT /*lprcPosRect*/)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::DoVerb");

		SetClientSite(pActiveSite);

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetClientSite( 
	/* [unique][in] */ __RPC__in_opt IOleClientSite *pClientSite)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::SetClientSite");
		if (m_pOleClientSite) 
		{
			m_pOleClientSite->Release();
		}
		m_pOleClientSite = pClientSite; 
		if (pClientSite)
		{
			pClientSite->AddRef();
		}
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetClientSite( 
	/* [out] */ __RPC__deref_out_opt IOleClientSite ** /*ppClientSite*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetHostNames( 
	/* [in] */ __RPC__in LPCOLESTR /*szContainerApp*/,
	/* [unique][in] */ __RPC__in_opt LPCOLESTR /*szContainerObj*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

        
	virtual HRESULT STDMETHODCALLTYPE Close( 
	/* [in] */ DWORD dwSaveOption)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::Close");

		// Wenn der Container uns sagt, wir sollen speichern, falls wir dirty sind
		if (dwSaveOption == OLECLOSE_SAVEIFDIRTY && m_pOleClientSite )
		{
			m_pOleClientSite->SaveObject();
		}

		// Dem OLE-Handler mitteilen, dass der Server sich schließt
		if (m_pOleAdviseHolder)
		{
			m_pOleAdviseHolder->SendOnClose();
		}
		clean();
		CoDisconnectObject(static_cast<IOleObject*>(this), 0);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetMoniker( 
	/* [in] */ DWORD /*dwWhichMoniker*/,
	/* [unique][in] */ __RPC__in_opt IMoniker * /*pmk*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMoniker( 
	/* [in] */ DWORD /*dwAssign*/,
	/* [in] */ DWORD /*dwWhichMoniker*/,
	/* [out] */ __RPC__deref_out_opt IMoniker ** /*ppmk*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE InitFromData( 
	/* [unique][in] */ __RPC__in_opt IDataObject * /*pDataObject*/,
	/* [in] */ BOOL /*fCreation*/,
	/* [in] */ DWORD /*dwReserved*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetClipboardData( 
	/* [in] */ DWORD /*dwReserved*/,
	/* [out] */ __RPC__deref_out_opt IDataObject ** /*ppDataObject*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumVerbs( 
	/* [out] */ __RPC__deref_out_opt IEnumOLEVERB **ppEnumOleVerb)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::EnumVerbs");
		if( ppEnumOleVerb )
			*ppEnumOleVerb = nullptr;

		return OLE_S_USEREG;
	}

	virtual HRESULT STDMETHODCALLTYPE Update( void)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE IsUpToDate( void)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserClassID( 
	/* [out] */ __RPC__out CLSID * /*pClsid*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserType( 
	/* [in] */ DWORD /*dwFormOfType*/,
	/* [out] */ __RPC__deref_out_opt LPOLESTR * /*pszUserType*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetExtent( 
	/* [in] */ DWORD /*dwDrawAspect*/,
	/* [in] */ __RPC__in SIZEL * /*psizel*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetExtent( 
	/* [in] */ DWORD /*dwDrawAspect*/,
	/* [out] */ __RPC__out SIZEL * /*psizel*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Advise( 
	/* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
	/* [out] */ __RPC__out DWORD *pdwConnection)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::Advise");

		// Falls noch kein Holder existiert, von Windows erstellen lassen
		if (!m_pOleAdviseHolder)
		{
			CreateOleAdviseHolder(&m_pOleAdviseHolder);
		}

		if (m_pOleAdviseHolder)
		{
			// Registrierung an Windows-Helfer delegieren
			return m_pOleAdviseHolder->Advise(pAdvSink, pdwConnection);
		}
		return E_FAIL;
	}

	virtual HRESULT STDMETHODCALLTYPE Unadvise( 
	/* [in] */ DWORD /*dwConnection*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumAdvise( 
	/* [out] */ __RPC__deref_out_opt IEnumSTATDATA ** /*ppenumAdvise*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMiscStatus( 
	/* [in] */ DWORD /*dwAspect*/,
	/* [out] */ __RPC__out DWORD * /*pdwStatus*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetColorScheme( 
	/* [in] */ __RPC__in LOGPALETTE * /*pLogpal*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}



	// ==========================================================
	// IDataObject
	// ==========================================================

	virtual HRESULT STDMETHODCALLTYPE DAdvise( 
	/* [in] */ __RPC__in FORMATETC *pFormatetc,
	/* [in] */ DWORD advf,
	/* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
	/* [out] */ __RPC__out DWORD *pdwConnection)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::DAdvise");

		// Falls noch kein Holder existiert, von Windows erstellen lassen
		if (!m_pDataAdviseHolder )
		{
			CreateDataAdviseHolder(&m_pDataAdviseHolder);
		}

		if (m_pDataAdviseHolder)
		{
			// Registrierung an Windows-Helfer delegieren
			return m_pDataAdviseHolder->Advise(this, pFormatetc, advf, pAdvSink, pdwConnection);
		}
		return E_FAIL;
	}

	virtual HRESULT STDMETHODCALLTYPE DUnadvise( 
	/* [in] */ DWORD dwConnection)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::DUnadvise");
		if (m_pDataAdviseHolder)
			return m_pDataAdviseHolder->Unadvise(dwConnection);
		return E_FAIL;	
	}

	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise( 
	/* [out] */ __RPC__deref_out_opt IEnumSTATDATA ** /*ppenumAdvise*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere( 
	/* [unique][in] */ FORMATETC * /*pformatetc*/,
	/* [out][in] */ STGMEDIUM * /*pmedium*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryGetData( 
	/* [unique][in] */ __RPC__in_opt FORMATETC * /*pformatetc*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc( 
	/* [unique][in] */ __RPC__in_opt FORMATETC * /*pformatectIn*/,
	/* [out] */ __RPC__out FORMATETC * /*pformatetcOut*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData( 
	/* [unique][in] */ FORMATETC * /*pformatetcIn*/,
	/* [out] */ STGMEDIUM * /*pmedium*/)
	{
		doLogPositionEx( gakLogging::llInfo );

		return DV_E_FORMATETC;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData( 
	/* [unique][in] */ FORMATETC * /*pformatetc*/,
	/* [unique][in] */ STGMEDIUM * /*pmedium*/,
	/* [in] */ BOOL /*fRelease*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc( 
	/* [in] */ DWORD /*dwDirection*/,
	/* [out] */ __RPC__deref_out_opt IEnumFORMATETC ** ppenumFormatEtc)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::EnumFormatEtc");
		if( ppenumFormatEtc )
			*ppenumFormatEtc = nullptr;
		return OLE_S_USEREG;
	}

	// ==========================================================
	// IViewObject2
	// ==========================================================
	virtual HRESULT STDMETHODCALLTYPE GetExtent( 
	/* [in] */ DWORD /*dwDrawAspect*/,
	/* [in] */ LONG /*lindex*/,
	/* [unique][in] */ __RPC__in_opt DVTARGETDEVICE * /*ptd*/,
	/* [out] */ __RPC__out LPSIZEL /*lpsizel*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return E_NOTIMPL;
	}

	// ==========================================================
	// IViewObject
	// ==========================================================
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Draw( 
	/* [annotation][in] */ 
	__in  DWORD /*dwDrawAspect*/,
	/* [annotation][in] */ 
	__in  LONG /*lindex*/,
	/* [annotation][unique][in] */ 
	__null  void * /*pvAspect*/,
	/* [annotation][unique][in] */ 
	__in_opt  DVTARGETDEVICE * /*ptd*/,
	/* [annotation][in] */ 
	__in_opt  HDC /*hdcTargetDev*/,
	/* [annotation][in] */ 
	__in  HDC /*hdcDraw*/,
	/* [annotation][in] */ 
	__in_opt  LPCRECTL /*lprcBounds*/,
	/* [annotation][unique][in] */ 
	__in_opt  LPCRECTL /*lprcWBounds*/,
	/* [annotation][in] */ 
	__in_opt  BOOL ( STDMETHODCALLTYPE * /*pfnContinue*/ )( 
	ULONG_PTR /*dwContinue*/),
	/* [annotation][in] */ 
	__in  ULONG_PTR /*dwContinue*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetColorSet( 
	/* [annotation][in] */ 
	__in  DWORD /*dwDrawAspect*/,
	/* [annotation][in] */ 
	__in  LONG /*lindex*/,
	/* [annotation][unique][in] */ 
	__null  void * /*pvAspect*/,
	/* [annotation][unique][in] */ 
	__in_opt  DVTARGETDEVICE * /*ptd*/,
	/* [annotation][in] */ 
	__in_opt  HDC /*hicTargetDev*/,
	/* [annotation][out] */ 
	__deref_out  LOGPALETTE ** /*ppColorSet*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Freeze( 
	/* [annotation][in] */ 
	__in  DWORD /*dwDrawAspect*/,
	/* [annotation][in] */ 
	__in  LONG /*lindex*/,
	/* [annotation][unique][in] */ 
	__null  void * /*pvAspect*/,
	/* [annotation][out] */ 
	__out  DWORD * /*pdwFreeze*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Unfreeze( 
	/* [in] */ DWORD /*dwFreeze*/)
	{
		doLogPositionEx( gakLogging::llInfo );
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetAdvise( 
	/* [in] */ DWORD aspects,
	/* [in] */ DWORD advf,
	/* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::SetAdvise");

		// Alten Zeiger sauber freigeben
		if (m_pViewAdviseSink)
		{
			m_pViewAdviseSink->Release();
			m_pViewAdviseSink = nullptr;
		}

		m_viewAspects = aspects;
		m_viewAdvf = advf;
		m_pViewAdviseSink = pAdvSink;

		// Neuen Zeiger festhalten
		if (m_pViewAdviseSink)
		{
			m_pViewAdviseSink->AddRef();
		}

		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetAdvise( 
	/* [annotation][unique][out] */ 
	__out_opt  DWORD *pAspects,
	/* [annotation][unique][out] */ 
	__out_opt  DWORD *pAdvf,
	/* [annotation][out] */ 
	__deref_out  IAdviseSink **ppAdvSink)
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleBaseDocument::GetAdvise");

		if (pAspects)
			*pAspects = m_viewAspects;
		if (pAdvf) 
			*pAdvf = m_viewAdvf;

		if (ppAdvSink)
		{
			*ppAdvSink = m_pViewAdviseSink;
			if (*ppAdvSink) 
				(*ppAdvSink)->AddRef();
		}
		return S_OK;
	}
};

template <class DocT>
class OleClassFactory : public IClassFactory
{
	GUID			m_guid;
	DWORD			m_registerID;
	LONG			m_refCount;
	public:
	OleClassFactory() : m_registerID(0), m_refCount(1) {}
	
	LONG getRegisterID() const
	{
		return m_registerID;
	}
	void setRegister( long registerID, GUID guid )
	{
		m_registerID = registerID;
		m_guid = guid;
	}

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) 
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleClassFactory::QueryInterface");
		if (riid == IID_IUnknown || riid == IID_IClassFactory) 
		{
			*ppvObject = static_cast<IClassFactory*>(this);
			AddRef();
			doLogPositionEx(gakLogging::llInfo);
			return S_OK;
		}
		doLogPositionEx(gakLogging::llInfo);
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}
	STDMETHODIMP_(ULONG) AddRef() 
	{ 
		doEnterFunctionEx(gakLogging::llInfo, "OleClassFactory::AddRef");
		return InterlockedIncrement(&m_refCount); 
	}
	STDMETHODIMP_(ULONG) Release() 
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleClassFactory::Release");
		LONG res = InterlockedDecrement(&m_refCount);
//		if (res == 0) 
//			delete this;
		return res;
    }

	// IClassFactory - Das moderne "srvCreate"
	STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) 
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleClassFactory::CreateInstance");
		if (pUnkOuter != nullptr) 
			return CLASS_E_NOAGGREGATION;
        
		// Neues Dokumenten-Objekt instanziieren
		DocT* pDoc = new (std::nothrow) DocT( m_guid );
		if( !pDoc )
			return E_OUTOFMEMORY;
        
		// Prüfen, ob das angeforderte Interface unterstützt wird
		HRESULT hr = pDoc->QueryInterface(riid, ppvObject);
		pDoc->Release();	// query interface increments the ref count if successful
							// => decrement since the constructor sets the ref count to
							// 1
		return hr;
	}

	STDMETHODIMP LockServer(BOOL /*fLock*/) 
	{
		doEnterFunctionEx(gakLogging::llInfo, "OleClassFactory::LockServer");
		// Hier erhöht/verringert man normalerweise den globalen Lock-Count der App
		return S_OK;
	}
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

DWORD initOleServer(STRING *guidStr, GUID *guid, IClassFactory *factory);
void exitOleServer( DWORD registerID );
void registerOleServer(const gak::STRING &guidStr, const gak::STRING &descr, const char *document="Document" );

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