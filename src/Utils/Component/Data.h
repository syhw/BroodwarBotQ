#pragma once
#include "BaseData.h"
#include <iostream>
#include <string>



template< class T>
class Data: public BaseData
{
public:
	Data();
	~Data();

	const std::string& getName() const;

	std::ostream& operator <<(const std::ostream& os) const;
	std::istream& operator >>(const std::istream& is) const;

private:
	T value;
	std::string data_name;
};
