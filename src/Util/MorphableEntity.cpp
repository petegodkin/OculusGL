///
///
///
///

#include "MorphableEntity.h"
#include <iostream>

#define N_DEFAULT_DELAY 15

MorphableEntity::MorphableEntity() : Entity()
{
	m_bIsMorphable = false;
	m_nBaseDelay = 0;
	m_nCurDelay = 0;

	m_curShape = nullptr;//new MeshSet();
	m_vecMorphs.push_back(m_curShape);
	init();

}

MorphableEntity::MorphableEntity(const MeshSet* start, glm::vec3 pos) : Entity(start, pos)
{
	m_bIsMorphable = false;
	m_nBaseDelay = N_DEFAULT_DELAY;
	m_nCurDelay = N_DEFAULT_DELAY;

	m_curShape = start;
	m_vecMorphs.push_back(m_curShape);
	init();

}

MorphableEntity::MorphableEntity(const MeshSet* start, glm::vec3 pos, glm::vec3 orient) : Entity(start, pos)
{
	m_bIsMorphable = false;
	m_nBaseDelay = N_DEFAULT_DELAY;
	m_nCurDelay = N_DEFAULT_DELAY;

	m_curShape = start;
	m_vecMorphs.push_back(m_curShape);
	init();
}

MorphableEntity::MorphableEntity(std::vector<const MeshSet *> morphs)
{
	assert(morphs.size() > 0);

	m_vecMorphs = morphs;

	if (morphs.size() > 0)
	{
		m_curShape = m_vecMorphs[m_nCurMorph];
	}
	init();
}

void MorphableEntity::init()
{
	m_bIsMorphable = false;
	m_bIsVisible = false;
	m_nCurMorph = 0;
	m_biggestRadius = 0.0;

	calcBiggestRadius();

	//Entity::setShape(m_curShape);
}

bool MorphableEntity::morph()
{
	//std::cout << "Morphing from original index " << m_nCurMorph << "!" << std::endl;

	bool bSuccess = true;

	if (!m_bIsMorphable)
	{
		return false;
	}

	if (m_nCurMorph + 1 < m_vecMorphs.size())
	{
		m_nCurMorph++;
	}
	else
	{
		assert(m_vecMorphs.size() > 0);
		m_nCurMorph = 0;
	}

	m_curShape = m_vecMorphs[m_nCurMorph];
	setShape(m_curShape);
	m_bIsMorphable = false;

	//std::cout << "Morphed to  " << m_nCurMorph << "!" << std::endl;

	return bSuccess;
}

void MorphableEntity::setStartMorph(const MeshSet *start)
{ 
	m_vecMorphs.push_back(start);
	m_nCurMorph = m_vecMorphs.size() - 1;

	m_curShape = m_vecMorphs[m_nCurMorph];
}

float MorphableEntity::getBoundingSphereRadius()
{
	return m_biggestRadius;

	std::cout << "Getting Biggest Radius: " << m_biggestRadius << std::endl;
}

const MeshSet* MorphableEntity::getCurMesh()
{
	return m_curShape;
}

//Scales each radius based on the scale of the mesh!!!
void MorphableEntity::calcBiggestRadius(bool isSingleAdd)
{

	float curMax = getBoundingSphereRadius();

	if (isSingleAdd)
	{
		const MeshSet *last = m_vecMorphs[m_vecMorphs.size() - 1];
		curMax = std::fmax(last->calcBoundingBox().getRadius() * last->getScale(), curMax);
	}
	else
	{
		for (const MeshSet *mesh : m_vecMorphs)
		{
			curMax = std::fmax(mesh->calcBoundingBox().getRadius() * mesh->getScale(), curMax);
		}
	}

	m_biggestRadius = curMax;

	//std::cout << "CALCULATING BIGGEST RADIUS FOR " << m_vecMorphs[m_nCurMorph]->m_fileName << ": " << m_biggestRadius << std::endl;

}


void MorphableEntity::addMorph(const MeshSet *toAdd) 
{ 
	m_vecMorphs.push_back(toAdd);
	calcBiggestRadius(true);
}

bool MorphableEntity::getIsVisible() { return m_bIsVisible; }
void MorphableEntity::setIsVisible(bool change) { m_bIsVisible = change; }

bool MorphableEntity::getIsMorphable() {return m_bIsMorphable; }
void MorphableEntity::setIsMorphable(bool change) {m_bIsMorphable = change; }

int MorphableEntity::getBaseDelay() { return m_nBaseDelay; }
void MorphableEntity::setBaseDelay(int delay) { m_nBaseDelay = delay; }

int MorphableEntity::getCurDelay() { return m_nCurDelay; }
void MorphableEntity::setCurDelay(int delay) { m_nCurDelay = delay; }

/*std::string MorphableEntity::toString()
{
	return "MorphableEntity:\n\tMorph Options: " +
		m_vecMorphs.size() +
		std::string("\n\tBase Delay: ") + m_nBaseDelay + "\n" +
		"\n\tCurrent Delay: " + m_nCurDelay + "\n";
}*/