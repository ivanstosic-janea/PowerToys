#pragma once

#include "pch.h"

class CSettings
{
public:
    CSettings();

    inline bool GetEnabled()
    {
        Reload();
        return settings.enabled;
    }

    inline void SetEnabled(bool enabled)
    {
        settings.enabled = enabled;
        Save();
    }

    void Save();
    void Load();

private:
    struct Settings
    {
        bool enabled{ true };
    };

    void Reload();
    void ParseJson();

    Settings settings;
    std::wstring jsonFilePath;
    FILETIME lastLoadedTime;
};

CSettings& CSettingsInstance();
