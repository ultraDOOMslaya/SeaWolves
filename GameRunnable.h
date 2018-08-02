#pragma once
#ifndef __GameRunnable_h_
#define __GameRunnable_h_

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreBitesConfigDialog.h>
#include <SDL.h>



class GameRunnable : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
	GameRunnable();
	~GameRunnable();

	void setup(void);
	virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
	virtual bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
	void frameRendered(const Ogre::FrameEvent& evt);

	Ogre::SceneManager*			scnMgr;
	Ogre::Camera*				cam;
	Ogre::RenderWindow*         mWindow;
};

#endif __GameRunnable_h_