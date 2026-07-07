#include "stdafx.h"
#include "ToastNotificationHelper.h"

#define _SILENCE_EXPERIMENTAL_COROUTINE_DEPRECATION_WARNINGS
#include <propkey.h>
#include <propvarutil.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <winrt/base.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>
#include <wrl/client.h>

#pragma comment(lib, "windowsapp.lib")
#pragma comment(lib, "runtimeobject.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

using Microsoft::WRL::ComPtr;

namespace
{
	const wchar_t* kAppId = L"C-Send";
	bool g_toastReady = false;

	std::wstring GetModuleDirectory()
	{
		wchar_t path[MAX_PATH] = { 0 };
		if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0) {
			return std::wstring();
		}

		std::wstring filePath(path);
		const size_t pos = filePath.find_last_of(L"\\/");
		if (pos == std::wstring::npos) {
			return std::wstring();
		}
		return filePath.substr(0, pos);
	}

	std::wstring GetShortcutPath()
	{
		PWSTR programsPath = nullptr;
		if (FAILED(SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &programsPath))) {
			return std::wstring();
		}

		std::wstring shortcutPath(programsPath);
		CoTaskMemFree(programsPath);
		shortcutPath += L"\\C-Send.lnk";
		return shortcutPath;
	}

	std::wstring EscapeXml(const std::wstring& text)
	{
		std::wstring escaped;
		escaped.reserve(text.size());
		for (wchar_t ch : text) {
			switch (ch) {
			case L'&': escaped += L"&amp;"; break;
			case L'<': escaped += L"&lt;"; break;
			case L'>': escaped += L"&gt;"; break;
			case L'\"': escaped += L"&quot;"; break;
			case L'\'': escaped += L"&apos;"; break;
			default:
				escaped += ch;
				break;
			}
		}
		return escaped;
	}

	bool EnsureShortcutAndAppId()
	{
		if (g_toastReady) {
			return true;
		}

		winrt::init_apartment(winrt::apartment_type::single_threaded);
		SetCurrentProcessExplicitAppUserModelID(kAppId);

		const std::wstring shortcutPath = GetShortcutPath();
		const std::wstring exePath = [&]() {
			wchar_t buffer[MAX_PATH] = { 0 };
			if (GetModuleFileNameW(nullptr, buffer, MAX_PATH) == 0) {
				return std::wstring();
			}
			return std::wstring(buffer);
		}();

		if (!shortcutPath.empty() && !exePath.empty()) {
			ComPtr<IShellLinkW> shellLink;
			if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink)))) {
				shellLink->SetPath(exePath.c_str());
				shellLink->SetWorkingDirectory(GetModuleDirectory().c_str());
				shellLink->SetDescription(L"C-Send");
				shellLink->SetIconLocation(exePath.c_str(), 0);

				ComPtr<IPropertyStore> propertyStore;
				if (SUCCEEDED(shellLink.As(&propertyStore))) {
					PROPVARIANT pv;
					PropVariantInit(&pv);
					if (SUCCEEDED(InitPropVariantFromString(kAppId, &pv))) {
						propertyStore->SetValue(PKEY_AppUserModel_ID, pv);
						propertyStore->Commit();
					}
					PropVariantClear(&pv);
				}

				ComPtr<IPersistFile> persistFile;
				if (SUCCEEDED(shellLink.As(&persistFile))) {
					persistFile->Save(shortcutPath.c_str(), TRUE);
				}
			}
		}

		g_toastReady = true;
		return true;
	}
}

void ShowClipboardToast(const CString& itemName)
{
	if (!EnsureShortcutAndAppId()) {
		return;
	}

	try {
		CStringW itemNameW(itemName);
		std::wstring title = itemNameW.IsEmpty() ? L"C-Send" : static_cast<LPCWSTR>(itemNameW);
		std::wstring xml = L"<toast><visual><binding template=\"ToastGeneric\"><text>";
		xml += EscapeXml(title);
		xml += L"</text><text>";
        xml += EscapeXml(L"Copied to clipboard");
		xml += L"</text></binding></visual></toast>";

		winrt::Windows::Data::Xml::Dom::XmlDocument doc;
		doc.LoadXml(winrt::hstring(xml));

		auto toast = winrt::Windows::UI::Notifications::ToastNotification(doc);
		auto notifier = winrt::Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier(winrt::hstring(kAppId));
		notifier.Show(toast);
	}
	catch (...) {
	}
}
