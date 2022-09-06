// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <string>

#include <Settings.h>
#include <interface/powertoy_module_interface.h>
#include <trace.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


class FileLocksmithModule : public PowertoyModuleIface
{
private:
    // Enabled by default
    bool m_enabled = true;
    
    std::wstring app_name;
    
    // Contains the non localized key of the powertoy
    std::wstring app_key;

public:

    // Constructor
    FileLocksmithModule()
    {
        m_enabled = CSettingsInstance().GetEnabled();
        // app_name = GET_RESOURCE_STRING(IDS_IMAGERESIZER);
        // app_key = ImageResizerConstants::ModuleKey;
        // LoggerHelpers::init_logger(app_key, L"ModuleInterface", LogSettings::imageResizerLoggerName);
    };

    // Destroy the powertoy and free memory
    virtual void destroy() override
    {
        delete this;
    }

    // Return the localized display name of the powertoy
    virtual const wchar_t* get_name() override
    {
        return app_name.c_str();
    }

    // Return the non localized key of the powertoy, this will be cached by the runner
    virtual const wchar_t* get_key() override
    {
        return app_key.c_str();
    }

    // Return JSON with the configuration options.
    virtual bool get_config(wchar_t* buffer, int* buffer_size) override
    {
        // HINSTANCE hinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);

        // Create a Settings object.
        // PowerToysSettings::Settings settings(hinstance, get_name());
        // settings.set_description(GET_RESOURCE_STRING(IDS_SETTINGS_DESCRIPTION));
        // settings.set_overview_link(L"https://aka.ms/PowerToysOverview_ImageResizer");
        // settings.set_icon_key(L"pt-image-resizer");
        // settings.add_header_szLarge(L"imageresizer_settingsheader", GET_RESOURCE_STRING(IDS_SETTINGS_HEADER_DESCRIPTION), GET_RESOURCE_STRING(IDS_SETTINGS_HEADER));
        // return settings.serialize_to_buffer(buffer, buffer_size);
        return false;
    }

    // Signal from the Settings editor to call a custom action.
    // This can be used to spawn more complex editors.
    virtual void call_custom_action(const wchar_t* action) override {}

    // Called by the runner to pass the updated settings values as a serialized JSON.
    virtual void set_config(const wchar_t* config) override {}

    // Enable the powertoy
    virtual void enable()
    {
        m_enabled = true;
        CSettingsInstance().SetEnabled(m_enabled);

        /*
        if (package::IsWin11OrGreater())
        {
            std::wstring path = get_module_folderpath(g_hInst_imageResizer);
            std::wstring packageUri = path + L"\\ImageResizerContextMenuPackage.msix";

            if (!package::IsPackageRegistered(ImageResizerConstants::ModulePackageDisplayName))
            {
                package::RegisterSparsePackage(path, packageUri);
            }
        }
        */

        Trace::EnableFileLocksmith(m_enabled);
    }

    // Disable the powertoy
    virtual void disable()
    {
        m_enabled = false;
        CSettingsInstance().SetEnabled(m_enabled);
        Trace::EnableFileLocksmith(m_enabled);
    }

    // Returns if the powertoys is enabled
    virtual bool is_enabled() override
    {
        return m_enabled;
    }
};

extern "C" __declspec(dllexport) PowertoyModuleIface* __cdecl powertoy_create()
{
    return new FileLocksmithModule();
}
