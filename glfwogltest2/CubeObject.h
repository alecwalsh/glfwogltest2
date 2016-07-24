#pragma once
#include "GameObject.h"

class CubeObject : public GameObject
{
public:
	//TODO: Manually call parent constructor
	using GameObject::GameObject;
	virtual void Tick();
};