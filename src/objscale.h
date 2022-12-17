/*
Objscale headerfile for compatibility/integration to the objscale module: https://github.com/Tralenor/mod-objscale
The scaling values or only persistently saved if the objscale module is used.
*/

#ifndef OBJSCALE_H
#define OBJSCALE_H

#include "DataMap.h"

class Objscale : public DataMap::Base
{
public:
    Objscale() {};
    Objscale(float scale) : scale(scale) {};
    float scale = -1.0f;
};

#endif
