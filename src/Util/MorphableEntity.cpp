///
///
///
///

#include "MorphableEntity.h"

#define N_DEFAULT_DELAY 15

MorphableEntity::MorphableEntity()
{
	m_bIsMorphable = false;
	m_nBaseDelay = 0;
	m_nCurDelay = 0;

	m_shapeStart = Shape();
}

MorphableEntity::MorphableEntity(const Shape &start)
{
	m_bIsMorphable = false;
	m_nBaseDelay = N_DEFAULT_DELAY;
	m_nCurDelay = N_DEFAULT_DELAY;

	m_shapeStart = Shape(start);
}

MorphableEntity::MorphableEntity(const std::vector<Shape> &morphs)
{

}

void MorphableEntity::setStartMorph(const Shape &start) { m_shapeStart = start; }
void MorphableEntity::addMorph(const Shape &toAdd) { m_vecMorphs.push_back(toAdd); }

bool MorphableEntity::getIsMorphable() {return m_bIsMorphable; }
void MorphableEntity::setIsMorphable(bool change) {m_bIsMorphable = change; }

int MorphableEntity::getBaseDelay() { return m_nBaseDelay; }
void MorphableEntity::setBaseDelay(int delay) { m_nBaseDelay = delay; }

int MorphableEntity::getCurDelay() { return m_nCurDelay; }
void MorphableEntity::setCurDelay(int delay) { m_nCurDelay = delay; }

std::string MorphableEntity::toString()
{
	return "MorphableEntity:\n\tMorph Options: " +
		m_vecMorphs.size() +
		std::string("\n\tBase Delay: ") + m_nBaseDelay + "\n" +
		"\n\tCurrent Delay: " + m_nCurDelay + "\n";
}