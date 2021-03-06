/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreSDLWindow.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreImageCodec.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreGLPixelFormat.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#   include <windows.h>
#   include <wingdi.h>
#   include <GL/gl.h>
#   define GL_GLEXT_PROTOTYPES
#   include "glprocs.h"
#   include <GL/glu.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#   include <GL/gl.h>
#   include <GL/glu.h>

#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <OpenGL/gl.h>
#   define GL_EXT_texture_env_combine 1
#   include <OpenGL/glext.h>
#   include <OpenGL/glu.h>
#endif

namespace Ogre {

    SDLWindow::SDLWindow() :
        mWindow(NULL), mActive(false), mClosed(false)
    {
    LogManager::getSingleton().logMessage("[*]  Create SDL ", LML_TRIVIAL);
    }

    SDLWindow::~SDLWindow()
    {

     LogManager::getSingleton().logMessage("[*] Destroy SDL  ", LML_TRIVIAL);
    }

	void SDLWindow::create(const String& name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams)
    {
    LogManager::getSingleton().logMessage("[*]  Create  Window", LML_TRIVIAL);

		int colourDepth = 32;
		String title = name;
		if(miscParams)
		{
			// Parse miscellenous parameters
			NameValuePairList::const_iterator opt;
			// Bit depth
			opt = miscParams->find("colourDepth");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
				colourDepth = StringConverter::parseUnsignedInt(opt->second);
			// Full screen antialiasing
			opt = miscParams->find("FSAA");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
			{
				size_t fsaa_x_samples = StringConverter::parseUnsignedInt(opt->second);
				if(fsaa_x_samples>1) {
					// If FSAA is enabled in the parameters, enable the MULTISAMPLEBUFFERS
					// and set the number of samples before the render window is created.
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,fsaa_x_samples);
				}
			}
			// Window title
			opt = miscParams->find("title");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
				title = opt->second;
		}

        LogManager::getSingleton().logMessage("SDLWindow::create", LML_TRIVIAL);

        int flags = SDL_WINDOW_OPENGL  | SDL_WINDOW_RESIZABLE;

        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        // request good stencil size if 32-bit colour
        if (colourDepth == 32)
        {
            SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
        }

        if (fullScreen)
            flags |= SDL_WINDOW_FULLSCREEN;

        LogManager::getSingleton().logMessage("Create window", LML_TRIVIAL);
        mWindow = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,flags);
        //screen = SDL_SetVideoMode(width, height, colourDepth, flags);
        if (!mWindow)
        {
            LogManager::getSingleton().logMessage(LML_CRITICAL,String("Could not create window: ") + SDL_GetError());
            exit(1);
        }
        LogManager::getSingleton().logMessage("screen is valid", LML_TRIVIAL);


        mContext = SDL_GL_CreateContext( mWindow );

        mName = name;

        mWidth = width;
        mHeight = height;

        mActive = true;


        if (!fullScreen)
            SDL_SetWindowTitle(mWindow,title.c_str());

        glXGetVideoSyncSGI = (int (*)(unsigned int *))SDL_GL_GetProcAddress("glXGetVideoSyncSGI");
        glXWaitVideoSyncSGI = (int (*)(int, int, unsigned int *))SDL_GL_GetProcAddress("glXWaitVideoSyncSGI");
    }

    void SDLWindow::destroy(void)
    {

    LogManager::getSingleton().logMessage("[*]  Destroy SDL Window", LML_TRIVIAL);

        if(mWindow)
        {
        SDL_GL_DeleteContext(mContext);
        SDL_DestroyWindow( mWindow );
        }
     	mWindow = NULL;



        mActive = false;

        Root::getSingleton().getRenderSystem()->detachRenderTarget( this->getName() );

    }

    bool SDLWindow::isActive() const
    {
        return mActive;
    }

    bool SDLWindow::isClosed() const
    {
        return mClosed;
    }

    void SDLWindow::reposition(int left, int top)
    {
        // XXX FIXME
        if (mWindow)
         SDL_SetWindowPosition(mWindow,left,top);
    }

    void SDLWindow::resize(unsigned int width, unsigned int height)
    {
    /*
        SDL_Surface* screen;
        int flags = SDL_OPENGL | SDL_HWPALETTE | SDL_RESIZABLE;
        SDL_res
*/
        if (mWindow)
        SDL_SetWindowSize(mWindow,width,height);
        LogManager::getSingleton().logMessage("Updating window", LML_TRIVIAL);
        /*
        screen = SDL_SetVideoMode(width, height, mScreen->format->BitsPerPixel, flags);
        if (!screen)
        {
            LogManager::getSingleton().logMessage(LML_CRITICAL,
                String("Could not make screen: ") + SDL_GetError());
            exit(1);
        }
        LogManager::getSingleton().logMessage("screen is valid", LML_TRIVIAL);
        mScreen = screen;
*/

        mWidth = width;
        mHeight = height;

        for (ViewportList::iterator it = mViewportList.begin();
             it != mViewportList.end(); ++it)
        {
            (*it).second->_updateDimensions();
        }
    }

    void SDLWindow::swapBuffers(bool waitForVSync)
    {
        if ( waitForVSync && glXGetVideoSyncSGI && glXWaitVideoSyncSGI )
        {
            unsigned int retraceCount;
            glXGetVideoSyncSGI( &retraceCount );
            glXWaitVideoSyncSGI( 2, ( retraceCount + 1 ) & 1, &retraceCount);
        }

	    SDL_GL_SwapWindow( mWindow );
        // XXX More?
    }

	void SDLWindow::copyContentsToMemory(const PixelBox &dst, FrameBuffer buffer)
	{
		if ((dst.left < 0) || (dst.right > mWidth) ||
			(dst.top < 0) || (dst.bottom > mHeight) ||
			(dst.front != 0) || (dst.back != 1))
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
						"Invalid box.",
						"SDLWindow::copyContentsToMemory" );
		}

		if (buffer == FB_AUTO)
		{
			buffer = mIsFullScreen? FB_FRONT : FB_BACK;
		}

		GLenum format = Ogre::GLPixelUtil::getGLOriginFormat(dst.format);
		GLenum type = Ogre::GLPixelUtil::getGLOriginDataType(dst.format);

		if ((format == GL_NONE) || (type == 0))
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
						"Unsupported format.",
						"SDLWindow::copyContentsToMemory" );
		}

		glReadBuffer((buffer == FB_FRONT)? GL_FRONT : GL_BACK);
		glReadPixels((GLint)dst.left, (GLint)dst.top,
					 (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
					 format, type, dst.data);

		//vertical flip
		{
			size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.format);
			size_t height = dst.getHeight();
			uchar *tmpData = new uchar[rowSpan * height];
			uchar *srcRow = (uchar *)dst.data, *tmpRow = tmpData + (height - 1) * rowSpan;

			while (tmpRow >= tmpData)
			{
				memcpy(tmpRow, srcRow, rowSpan);
				srcRow += rowSpan;
				tmpRow -= rowSpan;
			}
			memcpy(dst.data, tmpData, rowSpan * height);

			delete [] tmpData;
		}
	}
}
