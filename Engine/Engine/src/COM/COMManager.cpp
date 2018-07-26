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
		
		return finalPath;
	}


	return std::nullopt;
}
