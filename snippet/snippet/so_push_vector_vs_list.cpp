	#include <iostream>
	#include <vector>
	#include <list>


/// pause console until <enter> has been pressed
bool waitForEnter();


	class IdPool {
	public:
		IdPool()  {
			m_id = allocateID();
			std::cout << "c'tor id: " << m_id << std::endl;
		}

		~IdPool() {
			freeID(m_id);
			std::cout << "d'tor free id: " << m_id << std::endl;
		}
		IdPool(const IdPool& obj) {
			m_id = allocateID();
			std::cout << "copy c'tor id: " << m_id << std::endl;
		}

		class Init {
		public:
			Init(const int maxIDs) {
				for (int i=maxIDs; i>=1; --i) {
					s_idArray.push_back(i);
				};
			}
		};

		int id() { return m_id; }

	private:
		int	allocateID() {
			if (s_idArray.empty()) 
				return 0;
			else {
				int id = s_idArray.back();
				s_idArray.pop_back();
				return id;
			}
		}

		bool freeID(int id) {
			if ( (id > 0 ) && (s_idArray.size() < s_maxIdCount) ) {
				s_idArray.push_back(id);
				return true;
			} else {
				return false;
			}
		}
	
		static std::vector<int>	s_idArray;
		static const size_t		s_maxIdCount;
		static Init				s_setIdCount;
		int						m_id;
	};


	const size_t IdPool::s_maxIdCount = 10;
	std::vector<int> IdPool::s_idArray;
	IdPool::Init IdPool::s_setIdCount(IdPool::s_maxIdCount);


    int main(int argc, char* argv[]) {
		using namespace std;

		cout << endl << "-- push 2 IDs to list --" << endl;
		list<IdPool> listId;
		for (int i = 0; i < 2; ++i) {
			listId.push_back(IdPool());
			cout << "push_back to list id: " << listId.back().id() << endl << endl;
		}

		cout << endl << "-- push 2 IDs to vector --" << endl;
		vector<IdPool> vecId;
		for (int i = 0; i < 2; ++i) {
			vecId.push_back(IdPool());
			cout << "push_back to vector id: " << vecId.back().id() << endl << endl;
		}

		cout << endl << "-- push 2 IDs to preallocated vector --" << endl;
		vector<IdPool> vecIdReserved;
		vecIdReserved.reserve(5);
		for (int i = 0; i < 2; ++i) {
			vecIdReserved.push_back(IdPool());
			cout << "push_back to reserved vector id: " << vecIdReserved.back().id() << endl << endl;
		}

		waitForEnter();
		return 0;
	}

	






