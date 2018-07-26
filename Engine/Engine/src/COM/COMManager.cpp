#include "COMManager.h"
#include <atlbase.h>

std::optional<std::string> COM::GetOpenFileDialog(LPWSTR title, LPWSTR open)
{
	g_openFileDialog->SetOkButtonLabel(open);
	g_openFileDialog->SetTitle(title);

	HRESULT hr = g_openFileDialog->Show(NULL);

	if (SUCCEEDED(hr))
	{
		std::string finalPath;
		MicrosoftPointer<IShellItem> selectedItem;
		g_openFileDialog->GetResult(&selectedItem);

		PWSTR path;
		selectedItem->GetDisplayName(SIGDN_FILESYSPATH, &path);

		USES_CONVERSION;
		LPSTR pathA = W2A(path);
		finalPath = std::string(pathA);

		WCHAR pathToCurrent[MAX_PATH];
		GetModuleFileName(NULL, pathToCurrent, MAX_PATH);
		LPSTR executableFilePath = W2A(pathToCurrent);
		std::string exePath = std::string(executableFilePath);
		std::size_t lastAppearence = exePath.rfind('\\');
		exePath.erase(exePath.begin() + lastAppearence + 1, exePath.end());

		bool inExeFolder = true;
		for (std::size_t i = 0; i < exePath.size(); ++i)
		{
			if (exePath[i] != finalPath[i])
			{
				inExeFolder = false;
				break;
			}
		}
		if (inExeFolder)
			finalPath.erase(finalPath.begin(), finalPath.begin() + exePath.length());
		
		return finalPath;
	}


	return std::nullopt;
}
