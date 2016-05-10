#ifndef ____MorphableEntity__
#define ____MorphableEntity__

#include "Entity.h"
#include <string>
#include <vector>

class Shape;

class MorphableEntity : public Entity
{
public:
	MorphableEntity();
	MorphableEntity(const MorphableEntity &o);
	MorphableEntity(const Shape &start);
	MorphableEntity(const std::vector<Shape> &morphs);

	void setStartMorph(const Shape &start);
	void addMorph(const Shape &toAdd);

	bool getIsMorphable();
	void setIsMorphable(bool change);

	int getBaseDelay();
	void setBaseDelay(int delay);

	int getCurDelay();
	void setCurDelay(int delay);

	//std::string toString();

private:
	Shape m_shapeStart;
	std::vector<Shape> m_vecMorphs;

	int m_nBaseDelay;
	int m_nCurDelay;
	bool m_bIsMorphable = true;

};

#endif