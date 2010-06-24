#pragma once
#include <windows.h>
#include "BaseData.h"
#include <iostream>
#include <string>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

class BaseObject;

template<typename T>
class Data: public BaseData
{
public:
    Data(T defaultValue, std::string name = "baseDataName", BaseObject* parent = 0);
    ~Data();

	//const T& getValue() const;
	T* beginEdit();
	void endEdit();

	const std::string& getName() const;
    QWidget* createWidget(QWidget* parent);

	std::ostream& operator <<(std::ostream& os) const;
	std::istream& operator >>(const std::istream& is) const;

    //T& operator*();

    T* operator->();
    
    //const T& operator*() const;

    T& operator*();

    const T* operator->() const;

    bool is_sychronized();

    void synchronized();


protected:
    bool _synchronized;
	T value;
    T value_ro;
    HANDLE ghMutex;
	std::string data_name;
};
/*
template<typename Key, typename Value>
class Data<std::map<Key, Value> >: public BaseData
{
    typedef std::map<Key, Value> T;
public:
    Data(T defaultValue, std::string name = "baseDataName", BaseObject* parent = 0);
    virtual QWidget* createWidget( QWidget* parent);
};*/

// cadeau Bousket
//static Data<T>& initData(T* defaultValue, std::string name, BaseObject* parent)
//{
//    return Data<T>(defaultValue, name, parent);
//}