#pragma once
#include <windows.h>
#include "BaseData.h"
#include <iostream>
#include <string>

class BaseObject;

template<class T>
class Data: public BaseData
{
public:
    Data(T defaultValue, std::string name, BaseObject* parent);
	~Data();

	const T& getValue() const;
	T* beginEdit();
	void endEdit();

	const std::string& getName() const;

	std::ostream& operator <<(const std::ostream& os) const;
	std::istream& operator >>(const std::istream& is) const;

    bool is_sychronized()
    {
        return _synchronized;
    }

    void synchronized()
    {
        _synchronized = true;
    }


private:
    bool _synchronized;
	T value;
//	HANDLE ghMutex;
	std::string data_name;
};

// cadeau Bousket
//static Data<T>& initData(T* defaultValue, std::string name, BaseObject* parent)
//{
//    return Data<T>(defaultValue, name, parent);
//}