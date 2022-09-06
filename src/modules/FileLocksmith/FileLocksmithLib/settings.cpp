#include "pch.h"

#include "settings.h"
#include "constants.h"

#include <common/utils/json.h>
#include <common/SettingsAPI/settings_helpers.h>
#include <filesystem>
#include <commctrl.h>

namespace
{
    const wchar_t c_fileLocksmithDataFilePath[] = L"\\file-locksmith-settings.json";
    const wchar_t c_rootRegPath[] = L"Software\\Microsoft\\FileLocksmith";
    const wchar_t c_enabled[] = L"Enabled";

    bool LastModifiedTime(const std::wstring& filePath, FILETIME* lpFileTime)
    {
        WIN32_FILE_ATTRIBUTE_DATA attr{};
        if (GetFileAttributesExW(filePath.c_str(), GetFileExInfoStandard, &attr))
        {
            *lpFileTime = attr.ftLastWriteTime;
            return true;
        }
        return false;
    }
}

CSettings::CSettings()
{
    std::wstring savePath = PTSettingsHelper::get_module_save_folder_location(FileLocksmithConstants::ModuleSaveFolderKey);
    std::error_code ec;

    jsonFilePath = savePath + std::wstring(c_fileLocksmithDataFilePath);
    Load();
}

void CSettings::Save()
{
    json::JsonObject jsonData;

    jsonData.SetNamedValue(c_enabled, json::value(settings.enabled));

    json::to_file(jsonFilePath, jsonData);
    GetSystemTimeAsFileTime(&lastLoadedTime);
}

void CSettings::Load()
{
    if (!std::filesystem::exists(jsonFilePath))
    {
        Save();
    }
    else
    {
        ParseJson();
    }
}

void CSettings::Reload()
{
    // Load json settings from data file if it is modified in the meantime.
    FILETIME lastModifiedTime{};
    if (LastModifiedTime(jsonFilePath, &lastModifiedTime) &&
        CompareFileTime(&lastModifiedTime, &lastLoadedTime) == 1)
    {
        Load();
    }
}

void CSettings::ParseJson()
{
    auto json = json::from_file(jsonFilePath);
    if (json)
    {
        const json::JsonObject& jsonSettings = json.value();
        try
        {
            if (json::has(jsonSettings, c_enabled, json::JsonValueType::Boolean))
            {
                settings.enabled = jsonSettings.GetNamedBoolean(c_enabled);
            }
        }
        catch (const winrt::hresult_error&)
        {
        }
    }
    GetSystemTimeAsFileTime(&lastLoadedTime);
}

CSettings& CSettingsInstance()
{
    static CSettings instance;
    return instance;
}
