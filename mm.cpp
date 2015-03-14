#include<cmath>

float sep;
float oldTheta;

struct State
{
	float X=0;
	float Y=0;
	float theta=0;
}

State pos;

void setSeparation(float dist)
{
	sep=dist;
}

position getGlobalPosition()
{
	return pos;
}

void updateGlobalPosition(float dR, float dL)
{
	oldTheta=pos.theta;
	pos.theta+=float( (dR-dL)/sep );
	pos.X+=float( std::cos( (pos.theta+oldTheta)/2 )*(dR+dL)/2 );
	pos.Y+=float( std::sin( (pos.theta+oldTheta)/2 )*(dR+dL)/2 );
}