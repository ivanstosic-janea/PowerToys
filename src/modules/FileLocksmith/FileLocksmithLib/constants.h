#pragma once
#include <string_view>

namespace FileLocksmithConstants
{
    // Name of the powertoy module.
    inline constexpr std::wstring_view ModuleKey = L"File Locksmith";

    // Name of the FileLocksmith save folder.
    inline constexpr std::wstring_view ModuleSaveFolderKey = L"File Locksmith";
    inline constexpr std::wstring_view ModulePackageDisplayName = L"FileLocksmithContextMenu";
}
