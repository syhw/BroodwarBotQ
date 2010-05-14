#pragma once
#include <vector>
#include <iostream>
#include "CustomOStream.h"

class BaseData;


class BaseObject
{
public:
	BaseObject();
	~BaseObject();

	std::string getClassName() const;
	void processStream(std::ostream& out);

  mutable CustomOStream<BaseObject> sendl;
  mutable std::ostringstream serr;
  mutable std::ostringstream sout;

protected:
	std::vector<BaseData*> vData;

  std::string warnings;
  std::string outputs;
	std::string className;
};
