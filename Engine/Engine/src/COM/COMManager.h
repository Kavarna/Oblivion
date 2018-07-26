#pragma once

#include "../common/common.h"

namespace COM
{
	std::optional<std::string> GetOpenFileDialog(LPWSTR title, LPWSTR open);
}
