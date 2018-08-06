#pragma once
#ifndef __GameRunnable_h_
#define __GameRunnable_h_

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreBitesConfigDialog.h>
#include <OgreOverlay.h>
#include <OgreTrays.h>
#include <SDL.h>



class GameRunnable : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
public:
	GameRunnable();
	~GameRunnable();

	void setup(void);
	virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
	virtual bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
	virtual bool mousePressed(const OgreBites::MouseButtonEvent &evt);
	virtual bool nextLocation();
	
	virtual void frameRendered(const Ogre::FrameEvent& evt);
	virtual void gridSquareCordFinder(std::string squareName);

	virtual void createSquare(int width, int height, int edgeLength, std::string meshName, bool oddOrEven, Ogre::ColourValue color);
	virtual void createTileMap(void);

	OgreBites::TrayManager*		mTrayMgr;

	Ogre::SceneManager*			mScnMgr;
	Ogre::Camera*				mCam;
	Ogre::RenderWindow*         mWindow;
	Ogre::RaySceneQuery*		mRayScnQuery;
	Ogre::AnimationState*		robotAnimState;
	Ogre::SceneNode*			mUnitNode;
	std::deque<Ogre::Vector3>   mWalkList;
	Ogre::Real					mWalkSpeed;
	Ogre::Vector3				mDirection; //direction the object is moving
	Ogre::Real					mDistance; // distance object has left to travel
	Ogre::Vector3				mDestination; // destination the object is moving towards
	Ogre::Entity*				robotEntity;
	Ogre::SceneNode*			camNode;
};

#endif __GameRunnable_h_