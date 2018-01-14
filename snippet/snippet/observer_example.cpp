#include <iostream>
#include "../../inc/observer.h"

using namespace std;

void updateObserver(Observer* pObserver) {
	pObserver->update();
}

class Parameter {
private:
	string mName;
	string mType;
	string mValue;
public:
	Parameter(std::string name = "name", std::string type = "type", std::string value = "value")
		: mName(name), mType(type), mValue(value) {}

	string getName() const { return mName; }
	string getValue() const { return mValue; }
	double getDouble() const { return stod(mValue); }

	bool setValue(std::string& value) {
		mValue = value;
		return true;
	}
};

class ParamEquals : public unary_function<Parameter, bool> {
	string mName;
public: 
	ParamEquals (const string& name) : mName(name) {}
	bool operator() (const Parameter& par) const { 
		return (mName == par.getName());
	}
};



class ConfigSubj : public Subject {
private:
	list<Parameter> mParamList;
	typedef list<Parameter>::iterator Iter_Para;
public:
	ConfigSubj() {
	/* TODO set standard parameters */
		mParamList.push_back(Parameter("framesize_x", "int", "320"));
		mParamList.push_back(Parameter("framesize_y", "int", "240"));
		// region of interest
		mParamList.push_back(Parameter("roi_x", "int", "80"));
		mParamList.push_back(Parameter("roi_y", "int", "80"));
		// track assignment
		mParamList.push_back(Parameter("track_max_confidence", "int", "5"));
	}

	double getDouble(string name) {
		Iter_Para iPara = find_if(mParamList.begin(), mParamList.end(), ParamEquals(name));
		return iPara->getDouble();
	}

	string getParam(string name) {
		list<Parameter>::iterator iParam = find_if(mParamList.begin(), mParamList.end(), ParamEquals(name));
		return (iParam->getValue());
	}

	bool setParam(string name, string value) {
		list<Parameter>::iterator iParam = find_if(mParamList.begin(), mParamList.end(), ParamEquals(name));
		if (iParam == mParamList.end())
			return false;
		iParam->setValue(value);
		return true;
	}

};


class SceneObs : public Observer {
private:
	double mTrackMaxConf;
public:
	SceneObs(ConfigSubj* pSubject) : Observer(pSubject) {}
	void update(){
		mTrackMaxConf = mSubject->getDouble("track_max_confidence");
	}
	void printPara() {
		cout << "SceneObs" << endl;
		cout << "updated: " << mTrackMaxConf << endl;
	}
};


struct Point {
	double x;
	double y;
	Point(double x = 0, double y = 0) : x(x), y(y) {};
};

class FrameHdlObs : public Observer {
private:
	Point mFrameSize;
	Point mRoi;
public:
	FrameHdlObs(ConfigSubj* pSubject) : Observer(pSubject) {}
	void update(){
		mFrameSize.x = mSubject->getDouble("framesize_x");
		mFrameSize.y = mSubject->getDouble("framesize_y");
		mRoi.x = mSubject->getDouble("roi_x");
		mRoi.y = mSubject->getDouble("roi_y");
	}
	void printPara() {
		cout << "FrameHdl" << endl;
		cout << "updated: " << mFrameSize.x << " " << mFrameSize.y << endl;
		cout << "updated: " << mRoi.x << " " << mRoi.y << endl;
	}
};


int main(int argc, char* argv[]) {

	ConfigSubj subj;
	ConfigSubj* pSubj = &subj;
	FrameHdlObs frmHdl(pSubj);
	SceneObs scene(pSubj);
	pSubj->attach(&frmHdl);
	pSubj->attach(&scene);

	cout << "before notify" << endl;
	frmHdl.printPara();
	scene.printPara();

	subj.notifyObservers();
	cout << endl << "after notify" << endl;
	frmHdl.printPara();
	scene.printPara();


	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}