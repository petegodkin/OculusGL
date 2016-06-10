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
	MorphableEntity(const MeshSet* shape, glm::vec3 pos);
	MorphableEntity(const MeshSet* shape, glm::vec3 pos, glm::vec3 orient);
	MorphableEntity(std::vector<const MeshSet *> morphs);

	void setStartMorph(const MeshSet *start);
	void addMorph(const MeshSet *toAdd);

	float getBoundingSphereRadius();
	void calcBiggestRadius(bool isSingleAdd = false);

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

	const MeshSet *m_curShape;
	std::vector<const MeshSet *> m_vecMorphs;

	int m_nBaseDelay;
	int m_nCurDelay;
	int m_nCurMorph;
	bool m_bIsVisible = false;
	bool m_bIsMorphable;
	float m_biggestRadius;

};

#endif