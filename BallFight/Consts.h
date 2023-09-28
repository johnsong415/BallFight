#pragma once

#include "CommonTypes.h"



const Color COLOR_DARK_RED = { 0.8f, 0.f, 0.f };
const Color COLOR_RED = { 1.0f, 0.f, 0.f };
const Color COLOR_GREEN = { 0.f, 1.0f, 0.f };
const Color COLOR_DARK_GREEN = { 0.f, 0.8f, 0.f };
const Color COLOR_YELLOW = { 1.0f, 0.95f, 0.3f };
const Color COLOR_ORANGE = { 0.95f, 0.8f, 0.1f };
const Color COLOR_DARK_BLUE = { 0.f, 0.f, 0.8f };
const Color COLOR_BLUE = { 0.f, 0.f, 1.0f };
const Color COLOR_PURPLE = { 0.8f, 0.f, 0.8f };
const Color COLOR_BLACK = { 0.f, 0.f, 0.f };
const Color COLOR_WHITE = { 1.0f, 1.0f, 1.0f };
const Color COLOR_DEAD_OBJECT = { 0.40f, 0.40f, 0.40f };

const Color COLOR_DEFAULT = COLOR_WHITE;
const Color COLOR_PLAYER1 = COLOR_DARK_GREEN;
const Color COLOR_PLAYER1_BALL = COLOR_GREEN;
const Color COLOR_PLAYER2 = COLOR_ORANGE;
const Color COLOR_PLAYER2_BALL = COLOR_YELLOW;
const Color COLOR_MONSTER = COLOR_DARK_RED;
const Color COLOR_MONSTER_BALL = COLOR_RED;


const ObjectId INVALID_OBJECT_ID = -1;

const int BALL_CIRCLE_RADIUS = 5;
const int PLAYER_CIRCLE_RADIUS = 10;
const int MONSTER_CIRCLE_RADIUS = 10;

const float PLAYER_SPEED = 0.1f;
const float BALL_SPEED = 0.25f;
const float MONSTER_SPEED = 0.1f;

const int MONSTER_REDIRECT_TIME = 2000;
const int MONSTER_SHOOT_TIME = 2000;
const int MONSTER_OFFSET = 50;

