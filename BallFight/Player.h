#pragma once


#include "Object.h"


class Player : public Object
{
public:
	Player(const Coordinate& coords, const Velocity& v);
	~Player();

private:

};
