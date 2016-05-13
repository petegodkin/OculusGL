///
///
///
///

#include "MorphableEntity.h"

#define N_DEFAULT_DELAY 15

MorphableEntity::MorphableEntity() : Entity()
{
	init();
	m_bIsMorphable = false;
	m_nBaseDelay = 0;
	m_nCurDelay = 0;

	m_curShape = new Shape();
}

MorphableEntity::MorphableEntity(const Shape* start, glm::vec3 pos) : Entity(start, pos)
{
	init();
	m_bIsMorphable = false;
	m_nBaseDelay = N_DEFAULT_DELAY;
	m_nCurDelay = N_DEFAULT_DELAY;

	m_curShape = start;
}

MorphableEntity::MorphableEntity(const Shape* start, glm::vec3 pos, glm::vec3 orient) : Entity(start, pos)
{
	init();
	m_bIsMorphable = false;
	m_nBaseDelay = N_DEFAULT_DELAY;
	m_nCurDelay = N_DEFAULT_DELAY;

	m_curShape = start;
}

MorphableEntity::MorphableEntity(std::vector<const Shape *> morphs)
{
	init();
	m_vecMorphs = morphs;

	if (morphs.size() > 0)
	{
		m_curShape = m_vecMorphs[m_nCurMorph];
	}
}

void MorphableEntity::init()
{
	m_bIsMorphable = false;
	m_bWasSeen = false;
	m_nCurMorph = 0;
}

bool MorphableEntity::morph()
{
	bool bSuccess = true;

	if (m_nCurMorph + 1 < m_vecMorphs.size())
	{
		m_nCurMorph++;
	}
	else
	{
		assert(m_vecMorphs.size() > 1);
		m_nCurMorph = 0;
	}

	m_curShape = m_vecMorphs[m_nCurMorph];

	return bSuccess;
}

void MorphableEntity::setStartMorph(const Shape *start) { m_curShape = start; }
void MorphableEntity::addMorph(const Shape *toAdd) { m_vecMorphs.push_back(toAdd); }

bool MorphableEntity::getWasSeen() { return m_bWasSeen; }
void MorphableEntity::setWasSeen(bool change) { m_bWasSeen = change; }

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