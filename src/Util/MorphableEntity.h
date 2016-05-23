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
	MorphableEntity(const Shape* shape, glm::vec3 pos);
	MorphableEntity(const Shape* shape, glm::vec3 pos, glm::vec3 orient);
	MorphableEntity(std::vector<const Shape *> morphs);

	void setStartMorph(const Shape *start);
	void addMorph(const Shape *toAdd);

	bool getIsVisible();
	void setIsVisible(bool change);

	bool getIsMorphable();
	void setIsMorphable(bool change);

	int getBaseDelay();
	void setBaseDelay(int delay);

	int getCurDelay();
	void setCurDelay(int delay);

	bool morph();

	//std::string toString();

private:
	void init();

	const Shape *m_curShape;
	std::vector<const Shape *> m_vecMorphs;

	int m_nBaseDelay;
	int m_nCurDelay;
	int m_nCurMorph;
	bool m_bIsVisible = false;
	bool m_bIsMorphable;

};

#endif