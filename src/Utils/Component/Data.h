#pragma once
#include <windows.h>
#include "BaseData.h"
#include <iostream>
#include <string>
#ifdef BW_QT_DEBUG
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#endif

class BaseObject;

template<typename T>
class Data: public BaseData
{
public:
    Data(T defaultValue, std::string name = "baseDataName", BaseObject* parent = 0);
    ~Data();

	T* beginEdit();
	void endEdit();

	const std::string& getName() const;
#ifdef BW_QT_DEBUG
    QWidget* createWidget(QWidget* parent);
#endif

	std::ostream& operator <<(std::ostream& os) const;
	std::istream& operator >>(const std::istream& is) const;

    T& operator*();

    T* operator->();
    
    const T& operator*() const;

    const T* operator->() const;

    bool is_sychronized();

    void synchronized();


protected:
    bool _synchronized;
	T value;
	std::string data_name;
};