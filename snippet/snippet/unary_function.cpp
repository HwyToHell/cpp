// usage of unary predicate for custom classes to work with stl algorithms
#include "stdafx.h"

using namespace std;


class Parameter {
public:
	string getName() const { return mName;}
	//string mName;
private:
	string mName;
	string mType;
	string mValue;
};

class Param_eq : public unary_function<Parameter, bool> {
	string mName;
public: 
	Param_eq (const string& name) : mName(name) {}
	bool operator() (const Parameter& par) const { 
		return (mName == par.getName());
	}
};


double Config::getDouble(string& name) {
	list<Parameter>::iterator iParam = find_if(mParamList.begin(), mParamList.end(), Param_eq(name));
	double para = iParam->getDouble();
	return para;
}
