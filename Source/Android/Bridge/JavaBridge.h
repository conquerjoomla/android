#pragma once

#ifdef ANDROID

#include <Project64-core/Plugins/PluginClass.h>
#include <jni.h>

class JavaBridge : 
	public RenderWindow
{
public:
	JavaBridge (JavaVM* vm);

    //Render window functions
	void GfxThreadInit();
    void GfxThreadDone();
    void SwapWindow();

private:
	JavaBridge(void);		                    // Disable default constructor
	JavaBridge(const JavaBridge&);				// Disable copy constructor
    JavaBridge& operator=(const JavaBridge&);	// Disable assignment

	JavaVM* m_vm;
};

#endif