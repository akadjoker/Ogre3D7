#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include "OgreSDLGLSupport.h"

#include "OgreSDLWindow.h"

using namespace Ogre;

SDLGLSupport::SDLGLSupport()
{
    SDL_LogInfo(0,"[*] SDL GL Support Init ");
}

SDLGLSupport::~SDLGLSupport()
{
SDL_LogInfo(0,"[*] SDL GL Support Free ");

}

void SDLGLSupport::addConfig(void)
{
SDL_LogInfo(0,"[*] SDL GL Display Modes ");

    ConfigOption optFullScreen;
    ConfigOption optVideoMode;
    ConfigOption optFSAA;
	ConfigOption optRTTMode;

    // FS setting possiblities
    optFullScreen.name = "Full Screen";
    optFullScreen.possibleValues.push_back("Yes");
    optFullScreen.possibleValues.push_back("No");
    optFullScreen.currentValue = "Yes";
    optFullScreen.immutable = false;

    // Video mode possiblities
    optVideoMode.name = "Video Mode";
    optVideoMode.immutable = false;


   SDL_DisplayMode current;


  for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
  {

    int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);

    if(should_be_zero != 0)
    {
      OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS, "Could not get display mode for video display " + Ogre::String(SDL_GetError()) + " .", "SDLGLSupport::addConfig" );
    }
    else
     {




              int numDispModes = SDL_GetNumDisplayModes(i);
              if (numDispModes==-1)
              {
              OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS, "Could not get display mode for video display " + Ogre::String(SDL_GetError()) + " .", "SDLGLSupport::addConfig" );
              } else
              {



              for (int x=0;x<numDispModes;x++)
              {


                SDL_DisplayMode  mode;
                SDL_GetDisplayMode(i,x,&mode);
                if (mode.refresh_rate!=60)
                {
                 continue;
                }


                    char szBuf[16];
                    snprintf(szBuf, 16, "%d x %d", mode.w, mode.h);
                    optVideoMode.possibleValues.push_back(szBuf);
                    // Make the first one default
                    if (i == 0)
                    {
                        optVideoMode.currentValue = szBuf;
                    }

                SDL_LogInfo(0,"[*] [%d] %d / %d - %d",x,mode.w,mode.w,mode.refresh_rate);
              }

              }

     }
  }



    /*
    for (int i = 0;i<numDispModes; i++)
    {
        char szBuf[16];

        SDL_DisplayMode  mode;
        SDL_GetDisplayMode(0,i,&mode);


        SDL_LogInfo(0,"[*] [%d] %d / %d - %d",i,mode.w,mode.w,mode.refresh_rate);

		snprintf(szBuf, 16, "%d x %d", mode.w, mode.h);
        optVideoMode.possibleValues.push_back(szBuf);
        // Make the first one default
        if (i == 0)
        {
            optVideoMode.currentValue = szBuf;
        }
    }
*/
    //FSAA possibilities
    optFSAA.name = "FSAA";
    optFSAA.possibleValues.push_back("0");
    optFSAA.possibleValues.push_back("2");
    optFSAA.possibleValues.push_back("4");
    optFSAA.possibleValues.push_back("6");
    optFSAA.currentValue = "0";
    optFSAA.immutable = false;

	optRTTMode.name = "RTT Preferred Mode";
	optRTTMode.possibleValues.push_back("FBO");
	optRTTMode.possibleValues.push_back("PBuffer");
	optRTTMode.possibleValues.push_back("Copy");
	optRTTMode.currentValue = "FBO";
	optRTTMode.immutable = false;


    mOptions[optFullScreen.name] = optFullScreen;
    mOptions[optVideoMode.name] = optVideoMode;
    mOptions[optFSAA.name] = optFSAA;
	mOptions[optRTTMode.name] = optRTTMode;

}

String SDLGLSupport::validateConfig(void)
{
    return String("");
}

RenderWindow* SDLGLSupport::createWindow(bool autoCreateWindow, GLRenderSystem* renderSystem, const String& windowTitle)
{
SDL_LogInfo(0,"[*] create window");
RenderWindow *window = 0;

	if (autoCreateWindow)
    {
        ConfigOptionMap::iterator opt = mOptions.find("Full Screen");
        if (opt == mOptions.end())
            OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Can't find full screen options!", "SDLGLSupport::createWindow");
        bool fullscreen = (opt->second.currentValue == "Yes");

        opt = mOptions.find("Video Mode");
        if (opt == mOptions.end())
            OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Can't find video mode options!", "SDLGLSupport::createWindow");
        String val = opt->second.currentValue;
        String::size_type pos = val.find('x');
        if (pos == String::npos)
            OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Invalid Video Mode provided", "SDLGLSupport::createWindow");


        unsigned int w = StringConverter::parseUnsignedInt(val.substr(0, pos));
        unsigned int h = StringConverter::parseUnsignedInt(val.substr(pos + 1));

		// Parse FSAA config
		NameValuePairList winOptions;
		winOptions["title"] = windowTitle;
        int fsaa_x_samples = 0;
        opt = mOptions.find("FSAA");
        if(opt != mOptions.end())
        {
			winOptions["FSAA"] = opt->second.currentValue;
        }




        opt = mOptions.find("VSync");
        if(opt != mOptions.end())
        {
            SDL_LogInfo(0,"Have VZYNC ");
			winOptions["VSync"] = opt->second.currentValue;
        } else
        {
        SDL_LogInfo(0,"NO vzync ");
        }




//        const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
        window= renderSystem->_createRenderWindow(windowTitle, w, h, fullscreen, &winOptions);
    }

    return window;
}


RenderWindow* SDLGLSupport::newWindow(const String &name, unsigned int width, unsigned int height,
	bool fullScreen, const NameValuePairList *miscParams)
{
    SDL_LogInfo(0,"[*] new window");
	SDLWindow* window = new SDLWindow();
	window->create(name, width, height, fullScreen, miscParams);
	return window;
}

void SDLGLSupport::start()
{

    LogManager::getSingleton().logMessage(
        "******************************\n"
        "*** Starting SDL Subsystem ***\n"
        "******************************");

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_LogInfo(0,"[*] Start SDL");
}

void SDLGLSupport::stop()
{
SDL_LogInfo(0,"[*] Quit SDL");
    LogManager::getSingleton().logMessage(
        "******************************\n"
        "*** Stopping SDL Subsystem ***\n"
        "******************************");

    SDL_Quit();
}

void* SDLGLSupport::getProcAddress(const String& procname)
{
    return SDL_GL_GetProcAddress(procname.c_str());
}
