#pragma once

#include "pch.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class DECLSPEC_UUID("51B4D7E5-7568-4234-B4BB-47FB3C016A69")
    ContextMenuHandler;

#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C __declspec(selectany) const type name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

MIDL_DEFINE_GUID(CLSID, CLSID_ContextMenuHandler, 0xA898EA9D, 0x9155, 0x4811, 0xA2, 0xB9, 0xB7, 0x76, 0x7D, 0x07, 0xB8, 0x4F);

class ATL_NO_VTABLE __declspec(uuid("A898EA9D-9155-4811-A2B9-B7767D07B84F")) CContextMenuHandler :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CContextMenuHandler, &CLSID_ContextMenuHandler>,
    public IShellExtInit,
    public IContextMenu,
    public IExplorerCommand
{
    BEGIN_COM_MAP(CContextMenuHandler)
    COM_INTERFACE_ENTRY(IShellExtInit)
    COM_INTERFACE_ENTRY(IContextMenu)
    COM_INTERFACE_ENTRY(IExplorerCommand)
    END_COM_MAP()
    DECLARE_REGISTRY_RESOURCEID(IDR_CONTEXTMENUHANDLER)
    DECLARE_NOT_AGGREGATABLE(CContextMenuHandler)

public:
    CContextMenuHandler();
    ~CContextMenuHandler();
    HRESULT STDMETHODCALLTYPE Initialize(_In_opt_ PCIDLIST_ABSOLUTE pidlFolder, _In_opt_ IDataObject* pdtobj, _In_opt_ HKEY hkeyProgID);
    HRESULT STDMETHODCALLTYPE QueryContextMenu(_In_ HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uType, _In_ UINT* pReserved, LPSTR pszName, UINT cchMax);
    HRESULT STDMETHODCALLTYPE InvokeCommand(_In_ CMINVOKECOMMANDINFO* pici);

    // Inherited via IExplorerCommand
    virtual HRESULT __stdcall GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName) override;
    virtual HRESULT __stdcall GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon) override;
    virtual HRESULT __stdcall GetToolTip(IShellItemArray* psiItemArray, LPWSTR* ppszInfotip) override;
    virtual HRESULT __stdcall GetCanonicalName(GUID* pguidCommandName) override;
    virtual HRESULT __stdcall GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) override;
    virtual HRESULT __stdcall Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) override;
    virtual HRESULT __stdcall GetFlags(EXPCMDFLAGS* pFlags) override;
    virtual HRESULT __stdcall EnumSubCommands(IEnumExplorerCommand** ppEnum) override;
};

OBJECT_ENTRY_AUTO(__uuidof(ContextMenuHandler), CContextMenuHandler)
