

#include "C_Application.h"
#include "C_Graphics.h"
#include "I_EngineInterface.h"
#include "UtilityMacros.h"
#include <sstream>


class TestSystem : public I_EngineSystem {
public:
	std::string keyData;
	std::stringstream ss;

	bool fadeOut;
	float fade;

public:

	TestSystem() {
		fade = 1.0f;
		fadeOut = false;
		this->setVisibility(true);
		this->d_bIsPaused = false;
		G_Engine->getRenderList().push_back(this);
		G_Engine->getUpdateList().push_back(this);
	}

	virtual void onRender() {
		G_Graphics->drawText(keyData, D3DXCOLOR(.5f, 0.0f, 0.2f, fade), 300, 300, 400, 400);
		G_Graphics->drawText(ss.str(), D3DXCOLOR(.5f, 0.0f, 0.2f, 1.0f), 10, 50, 400, 400);
	}

	void onUpdate(const double& elapsedTime) {
		if(fadeOut)
			fade -= elapsedTime;
		else
			fade = 1.0f;
	}

	void flopFade() { 
		fadeOut = !fadeOut; 
	}
};

class KeyboardTest : public I_KeyboardListener {
private:
	TestSystem* t;

public:
	KeyboardTest(TestSystem* t) {this->t = t;}

	void onKeyDown(const unsigned char key) {
		t->keyData.push_back(key);
	}

	void onKeyUp(const unsigned char key) {

	}
};


class MouseTest : public I_MouseListener {
private:
	TestSystem* t;

public:
	MouseTest(TestSystem* t) { this->t = t;}
	void onMove( const short x, const short y ) {
		t->ss.str("");
		t->ss << "MouseX: " << x << " MouseY: " << y;
	}

	void onButtonUp( const MouseButton b, const short x, const short y ) {

	}

	void onButtonDown( const MouseButton b, const short x, const short y ) {
		switch(b) {
		case E_LEFT:
			t->flopFade();
			break;
		case E_MIDDLE:
			t->keyData.clear();
			t->keyData = "Middle Button\nWas Pressed";
			break;
		case E_RIGHT:
			t->keyData.clear();
			t->keyData = "Right Button\nWas Pressed";
			break;
		}
		
	}

	// Negative values for downward movements and positive for upward movements
	void onWheel(const short amt, const short x, const short y) {
		t->keyData.clear();
	}
};


class C_AppInstaller : public I_AppInstaller {
private:
	std::string d_appName;
	EngineSystemPtr d_pTestSystem;

public:

	C_AppInstaller() : d_appName("Test") {
		d_pTestSystem = new TestSystem();
	}

	void setUpFramework() {
		G_Engine->addSystem(d_pTestSystem);
		G_Graphics->setClearColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f));
	}
	bool isFullScreen() { return false; }
	unsigned getWindowHeight() { return 600; }
	unsigned getWindowWidth() { return 800; }
	const std::string& getAppTitle() { return d_appName; }
	MouseListenerPtr getMoustListener() { return new MouseTest((TestSystem*)&(*d_pTestSystem)); }
	KeyboardListenerPtr getKeyboardListener() { return new KeyboardTest((TestSystem*)&(*d_pTestSystem)); }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	G_Application->setAppInstaller(new C_AppInstaller);
	((C_Application*)G_Application)->init(hInstance);
	return ((C_Application*)G_Application)->run();
}
