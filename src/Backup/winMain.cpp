

#include "C_Application.h"


class C_AppInstaller : public I_AppInstaller {
private:
	std::wstring d_appName;

public:

	C_AppInstaller() : d_appName(L"Test") {}

	void setUpFramework() {}
	bool isFullScreen() { return false; }
	unsigned getWindowHeight() { return 600; }
	unsigned getWindowWidth() { return 800; }
	const std::wstring& getAppTitle() { return d_appName; }
	MouseListenerPtr getMoustListener() { return 0; }
	KeyboardListenerPtr getKeyboardListener() { return 0; }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	G_Application->setAppInstaller(new C_AppInstaller);
	((C_Application*)G_Application)->init(hInstance);
	return ((C_Application*)G_Application)->run();
}
