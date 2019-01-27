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
				IdPool(IdPool&& obj);
				IdPool& operator= (IdPool&& obj);
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