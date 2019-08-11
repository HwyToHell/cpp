/// sizeable static ID pool, 
/// allocating IDs for each new class instance
/// freeing IDs after class destruction

#pragma once
#include <vector>

class IdPool {
public:
				IdPool();
				~IdPool();
				IdPool(const IdPool& obj);
				IdPool& operator= (const IdPool& obj);
				//IdPool(IdPool&& obj);
				//IdPool& operator= (IdPool&& obj);
	inline int	id() { return m_id; }

	class Init {
	public:
		inline Init(const int maxIDs) {
			for (int i=maxIDs; i>=1; --i) {
				s_idArray.push_back(i);
			};
		}
	};

private:
	int						allocateID();
	bool					freeID(int id);
	
	static std::vector<int>	s_idArray;
	static const int		s_maxIdCount;
	static Init				s_setIdCount;
	int						m_id;
};


/// sizeable ID generator 
/// allocating IDs from defined pool of IDs
/// freeing IDs pushes them back to ID pool
class IdGen {
public:
						IdGen(const size_t maxIdCount);
	size_t				allocID();
	bool				freeID(const size_t id);
	size_t				poolSize();
	void				sortGreater();
private:
	std::vector<size_t>	m_idPool;
	const size_t		m_maxIdCount;
};


/// unique ID generator (unique per process)
class IdUnique {
public:
    size_t              allocID();
private:
    static size_t       s_id;
};
