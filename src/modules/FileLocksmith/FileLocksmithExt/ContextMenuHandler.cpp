// ContextMenuHandler.cpp : Implementation of CContextMenuHandler

#include "pch.h"
#include "ContextMenuHandler.h"

#include <Settings.h>
#include <trace.h>

#include <common/themes/icon_helpers.h>
#include <common/utils/process_path.h>
#include <common/utils/resources.h>
#include <common/utils/HDropIterator.h>
#include <common/utils/package.h>

extern HINSTANCE g_hInst_imageResizer;

CContextMenuHandler::CContextMenuHandler()
{
}

CContextMenuHandler::~CContextMenuHandler()
{
}

HRESULT CContextMenuHandler::Initialize(_In_opt_ PCIDLIST_ABSOLUTE pidlFolder, _In_opt_ IDataObject* pdtobj, _In_opt_ HKEY hkeyProgID)
{
    return S_OK;
}

HRESULT CContextMenuHandler::QueryContextMenu(_In_ HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    return S_OK;
}

HRESULT CContextMenuHandler::GetCommandString(UINT_PTR idCmd, UINT uType, _In_ UINT* pReserved, LPSTR pszName, UINT cchMax)
{
    if (idCmd == 0)
    {
        if (uType == GCS_VERBW)
        {
            wcscpy_s((LPWSTR)pszName, cchMax, L"FileLocksmithVerb");
        }
    }
    else
    {
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT CContextMenuHandler::InvokeCommand(_In_ CMINVOKECOMMANDINFO* pici)
{
    MessageBoxW(NULL, L"File", L"Locksmith", MB_OK);
    return S_OK;
}

HRESULT __stdcall CContextMenuHandler::GetTitle(IShellItemArray* /*psiItemArray*/, LPWSTR* ppszName)
{
    return SHStrDup(L"File Locksmith", ppszName);
}

HRESULT __stdcall CContextMenuHandler::GetIcon(IShellItemArray* /*psiItemArray*/, LPWSTR* ppszIcon)
{
    // Since ImageResizer is registered as a COM SurrogateServer the current module filename would be dllhost.exe. To get the icon we need the path of PowerToys.ImageResizerExt.dll, which can be obtained by passing the HINSTANCE of the dll
    std::wstring iconResourcePath = get_module_filename(g_hInst_imageResizer);
    iconResourcePath += L",-";
    iconResourcePath += std::to_wstring(IDI_RESIZE_PICTURES);
    return SHStrDup(iconResourcePath.c_str(), ppszIcon);
}

HRESULT __stdcall CContextMenuHandler::GetToolTip(IShellItemArray* /*psiItemArray*/, LPWSTR* ppszInfotip)
{
    *ppszInfotip = nullptr;
    return E_NOTIMPL;
}

HRESULT __stdcall CContextMenuHandler::GetCanonicalName(GUID* pguidCommandName)
{
    *pguidCommandName = __uuidof(this);
    return S_OK;
}

HRESULT __stdcall CContextMenuHandler::GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState)
{
    if (!CSettingsInstance().GetEnabled())
    {
        *pCmdState = ECS_HIDDEN;
        return S_OK;
    }
    // Hide if the file is not an image
    *pCmdState = ECS_HIDDEN;
    // Suppressing C26812 warning as the issue is in the shtypes.h library
#pragma warning(suppress : 26812)
    PERCEIVED type;
    PERCEIVEDFLAG flag;
    IShellItem* shellItem;
    //Check extension of first item in the list (the item which is right-clicked on)
    psiItemArray->GetItemAt(0, &shellItem);
    LPTSTR pszPath;
    // Retrieves the entire file system path of the file from its shell item
    shellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
    LPTSTR pszExt = PathFindExtension(pszPath);

    // TODO: Instead, detect whether there's a WIC codec installed that can handle this file
    AssocGetPerceivedType(pszExt, &type, &flag, NULL);

    CoTaskMemFree(pszPath);
    // If selected file is an image...
    if (type == PERCEIVED_TYPE_IMAGE)
    {
        *pCmdState = ECS_ENABLED;
    }
    return S_OK;
}

HRESULT __stdcall CContextMenuHandler::GetFlags(EXPCMDFLAGS* pFlags)
{
    *pFlags = ECF_DEFAULT;
    return S_OK;
}

HRESULT __stdcall CContextMenuHandler::EnumSubCommands(IEnumExplorerCommand** ppEnum)
{
    *ppEnum = nullptr;
    return E_NOTIMPL;
}

// psiItemArray contains the list of files that have been selected when the context menu entry is invoked
HRESULT __stdcall CContextMenuHandler::Invoke(IShellItemArray* psiItemArray, IBindCtx* /*pbc*/)
{
    Trace::Invoked();
    HRESULT hr = ResizePictures(nullptr, psiItemArray);
    Trace::InvokedRet(hr);
    return hr;
}
