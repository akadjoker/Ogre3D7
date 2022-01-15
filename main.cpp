#include <iostream>
#include "Ogre.h"
#include "OgreConfigFile.h"
//#include "OgreStaticPluginLoader.h"

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>

#include "OgreGLPlugin.h"
#include "OgreParticleFXPlugin.h"
#include "OgreOctreePlugin.h"
#include "OgreBspSceneManagerPlugin.h"
#include "OgrePCZPlugin.h"
//#include "OgreOctreeZonePlugin.h"

using namespace std;

/* Plugin=RenderSystem_GL
 Plugin=Plugin_ParticleFX
 Plugin=Plugin_BSPSceneManager
 Plugin=Plugin_PCZSceneManager
 Plugin=Plugin_OctreeZone
 Plugin=Plugin_OctreeSceneManager*/

Ogre::Root*                 mRoot;
Ogre::Camera*               mCamera;
Ogre::SceneManager*         mSceneMgr;
Ogre::RenderWindow*         mWindow;
Ogre::GLPlugin* mGLPlugin;
Ogre::ParticleFXPlugin* mParticleFXPlugin;


Ogre::OctreePlugin* mOctreePlugin;
Ogre::BspSceneManagerPlugin* mBSPPlugin;
Ogre::PCZPlugin* mPCZPlugin;
//Ogre::OctreeZonePlugin* mOctreeZonePlugin;

int main()
{
    cout << "Hello world!" << endl;

    mRoot = new Ogre::Root();


mGLPlugin = OGRE_NEW Ogre::GLPlugin();
mRoot->installPlugin(mGLPlugin);
mOctreePlugin = OGRE_NEW Ogre::OctreePlugin();
mRoot->installPlugin(mOctreePlugin);
mBSPPlugin = OGRE_NEW Ogre::BspSceneManagerPlugin();
mRoot->installPlugin(mBSPPlugin);
mParticleFXPlugin = OGRE_NEW Ogre::ParticleFXPlugin();
mRoot->installPlugin(mParticleFXPlugin);
mPCZPlugin = OGRE_NEW Ogre::PCZPlugin();
mRoot->installPlugin(mPCZPlugin);
//mOctreeZonePlugin = OGRE_NEW Ogre::OctreeZonePlugin();
//mRoot->installPlugin(mOctreeZonePlugin);


    Ogre::ConfigFile cf;
    cf.load("/media/djoker/code/linux/cpp/ogre/ogre-1-7-0/build/bin/resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;


            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(                archName, typeName, secName);
        }
    }
      if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");


    } else
    {
    OGRE_DELETE mGLPlugin;
    OGRE_DELETE mParticleFXPlugin;
//    OGRE_DELETE mOctreeZonePlugin;
	OGRE_DELETE mPCZPlugin;
    OGRE_DELETE mBSPPlugin;
	OGRE_DELETE mParticleFXPlugin;
	OGRE_DELETE mOctreePlugin;
    OGRE_DELETE mRoot;
    return 0;
    }

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();






    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setPosition(Ogre::Vector3(0,0,80));
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));



    mRoot->startRendering();

    bool gameLoop=true;
    while(gameLoop)
    {

    mRoot->renderOneFrame();
    }



    OGRE_DELETE mGLPlugin;
    OGRE_DELETE mParticleFXPlugin;
//    OGRE_DELETE mOctreeZonePlugin;
	OGRE_DELETE mPCZPlugin;
    OGRE_DELETE mBSPPlugin;
	OGRE_DELETE mParticleFXPlugin;
	OGRE_DELETE mOctreePlugin;
    OGRE_DELETE mRoot;


  return 0;
}
