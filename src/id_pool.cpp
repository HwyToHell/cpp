/// initialize static variables
#include "stdafx.h"
#include "../inc/id_pool.h"

#include <algorithm>
#include <utility> 
#include <iostream> // TODO DELETE after testing


//////////////////////////////////////////////////////////////////////////////
// ID Pool
//////////////////////////////////////////////////////////////////////////////

// specify maximum number of IDs available
const int IdPool::s_maxIdCount = 20;
std::vector<int> IdPool::s_idArray; // statement must be executed before s_setIdCount is initialized
IdPool::Init IdPool::s_setIdCount(IdPool::s_maxIdCount);



IdPool::IdPool() {
	m_id = allocateID();
	std::cout << "c'tor id: " << m_id << std::endl;
}


IdPool::~IdPool() {
	freeID(m_id);
	std::cout << "d'tor free id: " << m_id << std::endl;
}


IdPool::IdPool(const IdPool& obj) {
	m_id = allocateID();
    (void)obj;
	std::cout << "copy c'tor id: " << m_id << std::endl;
}

	
IdPool& IdPool::operator= (const IdPool& obj) {
	//m_id = obj.m_id;
    (void)obj;
	std::cout << "assignment id: " << m_id << std::endl;
	return *this;
}


/*IdPool::IdPool(IdPool&& obj) {
	m_id = allocateID();
	//std::swap(m_id, obj.m_id);
	std::cout << "move c'tor id: " << m_id << std::endl;
}

	
IdPool& IdPool::operator= (IdPool&& obj) {
	//m_id = obj.m_id;
	std::cout << "move assignment id: " << m_id << std::endl;
	return *this;
}*/


int	IdPool::allocateID() {
	if (s_idArray.empty()) 
		return 0;
	else {
		int id = s_idArray.back();
		s_idArray.pop_back();
		return id;
	}
}


bool IdPool::freeID(int id) {
	if ( (id > 0 ) && (s_idArray.size() < s_maxIdCount) ) {
		s_idArray.push_back(id);
		return true;
	} else {
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// ID Generator
//////////////////////////////////////////////////////////////////////////////

IdGen::IdGen(size_t maxIdCount) : m_maxIdCount(maxIdCount) {
	m_idPool.reserve(m_maxIdCount);
	for (size_t n = m_maxIdCount; n >= 1; --n) 
		m_idPool.push_back(n);
}


size_t IdGen::allocID() {
	if (m_idPool.empty()) 
		return 0;
	else {
		size_t id = m_idPool.back();
		m_idPool.pop_back();
		return id;
	}
}


bool IdGen::freeID(size_t id) {
	if ( m_idPool.size() < m_maxIdCount ) {
		m_idPool.push_back(id);
		return true;
	} else {
		return false;
	}
}

size_t IdGen::poolSize() {
	return m_idPool.size();
}


void IdGen::sortGreater() {
	std::sort(m_idPool.begin(), m_idPool.end(), std::greater<size_t>());
}

//////////////////////////////////////////////////////////////////////////////
// ID Unique Generator
//////////////////////////////////////////////////////////////////////////////

size_t IdUnique::s_id = 0;

size_t IdUnique::allocID() {
    return ++s_id;
}


