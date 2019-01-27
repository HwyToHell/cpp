#include "stdafx.h"
#include <climits>
#include <functional>
#pragma warning(disable: 4482) // MSVC10: enum nonstd extension

#include "D:/Holger/app-dev/video-dev/car-count/include/config.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/frame_handler.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/tracker.h"
#include "D:/Holger/app-dev/video-dev/car-count/include/recorder.h"
//#include "../../inc/program_options.h"
#include "../../inc/opencv/backgroundsubtraction.h"


/// pause console until <enter> has been pressed
bool waitForEnter();


class NoCopy {
// with own id counter
public:
	NoCopy() {
		m_id = getID();
		std::cout << "c'tor id: " << m_id << std::endl;
	}


private:
// copy construction not allowed
	NoCopy(const NoCopy& occ)  {
		//m_id = getID();
		m_id = occ.m_id;
		std::cout << "copy c'tor id: " << m_id << std::endl;
	}

public:
// move construction is allowed
	NoCopy(NoCopy&& occ) {
		//m_id = getID();
		m_id = occ.m_id;
		std::cout << "move c'tor id: " << m_id << std::endl;
	}


	~NoCopy() {
		returnID(m_id);
		std::cout << "d'tor return id: " << m_id << std::endl;
	}

	int			id() { return m_id; }
	
	class		Init {
	public:
		Init() {
			for (int i=9; i>=1; --i) {
				s_idList.push_back(i);
			}
		}
	};

private:
	int getID() {
		if (s_idList.empty()) 
			return 0;
		else {
			int id = s_idList.back();
			s_idList.pop_back();
			return id;
		}
	}

	bool returnID(int id) {
		if ( (id > 0 ) && (s_idList.size() < s_maxNoIDs) ) {
			s_idList.push_back(id);
			return true;
		} else {
			return false;
		}
	}

	static Init				s_initializer;
	static const int		s_maxNoIDs;
	static std::list<int>	s_idList;
	int						m_id;
};

const int NoCopy::s_maxNoIDs = 9;

std::list<int> NoCopy::s_idList;

NoCopy::Init NoCopy::s_initializer;

class NoCopyMember {
public:
	NoCopyMember() {}
	void pushNoCopy(NoCopy& ncpy) {
		m_noCopies.push_back(std::move(ncpy));
	}
	
private:
	std::list<NoCopy> m_noCopies;
};

void printID(NoCopy& ncpy) {
	std::cout << "id: " << ncpy.id() << std::endl;
	return;
}


bool addOcclusion(std::list<Occlusion>& lst) {
	Occlusion occ;
	lst.push_back(occ);
	return true;
}


int add() {
	using namespace std;

	list<Occlusion> l;
	addOcclusion(l);
	cout << "add occlusion 1: " << " id: " << l.back().id() << endl;
	cout << endl;

	// copy construction is allowed
	addOcclusion(l);
	cout << "add occlusion 2: " << " id: " << l.back().id() << endl;
	cout << endl;

	l.clear();
	cout << "list cleared" << endl << endl;

	waitForEnter();
	return 0;
}


int main(int argc, char* argv[]) {
	using namespace std;

	// standard construction is allowed
	NoCopy a;
	cout << "a created" << endl << endl;


	// move construction is allowed
	list<NoCopy> l;
	l.push_back(move(a));
	cout << "a pushed (move)" << endl;
	cout << endl;

	/*
	// copy construction forbidden
	l.push_back(a);
	cout << "a pushed again (copy)" << endl;
	cout << endl;
	*/

	l.push_back(NoCopy());
	cout << "anonymous push (copy or move), creates another id" << endl;
	cout << endl;


	l.clear();
	cout << "list cleared" << endl << endl;

	// push with move -> copy not allowed
	NoCopyMember ncpm;
	ncpm.pushNoCopy(a);


	waitForEnter();
	return 0;
}

	






