// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <atlfile.h>
#include <atlstr.h>
#include <Shlwapi.h>
#include <shobjidl_core.h>
#include <string>

#include <common/utils/process_path.h>
#include <common/utils/resources.h>
#include <Settings.h>
#include <trace.h>

#include <wrl/module.h>

using namespace Microsoft::WRL;

HINSTANCE g_hInst = 0;

#define BUFSIZE 4096 * 4

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hModule;
        Trace::RegisterProvider();
        break;
    case DLL_PROCESS_DETACH:
        Trace::UnregisterProvider();
        break;
    }
    return TRUE;
}

class __declspec(uuid("A898EA9D-9155-4811-A2B9-B7767D07B84F")) FileLocksmithContextMenuCommand final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand, IObjectWithSite>
{
public:
    virtual const wchar_t* Title() { return L"File Locksmith"; }
    virtual const EXPCMDFLAGS Flags() { return ECF_DEFAULT; }
    virtual const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) { return ECS_ENABLED; }

    // IExplorerCommand
    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* name)
    {
        wchar_t strFileLocksmith[] = L"File Locksmith";

        return SHStrDup(strFileLocksmith, name);
    }

    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon)
    {
        std::wstring iconResourcePath = get_module_folderpath(g_hInst);
        iconResourcePath += L"\\";
        iconResourcePath += L"FileLocksmith.ico";
        return SHStrDup(iconResourcePath.c_str(), icon);
    }

    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip)
    {
        *infoTip = nullptr;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName)
    {
        *guidCommandName = __uuidof(this);
        return S_OK;
    }

    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        if (!CSettingsInstance().GetEnabled())
        {
            *cmdState = ECS_HIDDEN;
        }
        else
        {
            *cmdState = ECS_ENABLED;
        }
        
        return S_OK;
    }

    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept
    {
        Trace::Invoked();
        HRESULT hr = S_OK;

        if (selection)
        {
            MessageBoxW(NULL, L"File Locksmith", L"hello", MB_OK);
        }

        Trace::InvokedRet(hr);
        return hr;
    }

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags)
    {
        *flags = Flags();
        return S_OK;
    }

    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands)
    {
        *enumCommands = nullptr;
        return E_NOTIMPL;
    }

    // IObjectWithSite
    IFACEMETHODIMP SetSite(_In_ IUnknown* site) noexcept
    {
        m_site = site;
        return S_OK;
    }

    IFACEMETHODIMP GetSite(_In_ REFIID riid, _COM_Outptr_ void** site) noexcept
    {
        return m_site.CopyTo(riid, site);
    }

protected:
    ComPtr<IUnknown> m_site;
};

CoCreatableClass(FileLocksmithContextMenuCommand)
    CoCreatableClassWrlCreatorMapInclude(FileLocksmithContextMenuCommand)

        STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}
