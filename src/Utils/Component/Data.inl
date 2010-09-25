#include <map>
#include "Data.h"
#include "BaseObject.h"
#ifdef BW_QT_DEBUG
#include "WidgetCreator.h"
#include "WidgetCreator.inl"
#endif

template<typename T>
Data<T>::Data(T defaultValue, std::string name, BaseObject* parent)
: _synchronized(true)
, value(defaultValue)
, data_name(name)
{
    if (parent)
        parent->addData(this);
}
 

template< typename T>
Data<T>::~Data()
{
}

template< typename T>
T& Data<T>::operator*()
{
    return value;
}

template< typename T>
const T& Data<T>::operator*() const
{
    return value;
}

template< typename T>
T* Data<T>::operator->()
{
    return &value;
}

template< typename T>
const T* Data<T>::operator->() const
{
    return &value;
}


template< typename T>
bool Data<T>::is_sychronized()
{
    return _synchronized;
}

template< typename T>
void Data<T>::synchronized()
{
    _synchronized = true;
}


template< typename T>
T* Data<T>::beginEdit()
{
	return &value;
}


template< typename T>
void Data<T>::endEdit()
{
    _synchronized = false;
}


template< typename T>
const std::string& Data<T>::getName() const
{
	return data_name;
}


template< typename T>
std::ostream& Data<T>::operator <<(std::ostream& os) const
{
	os << value;
	return os;
}


template< typename T>
std::istream& Data<T>::operator >>(const std::istream& is) const
{
	is >> value;
	return is;
}

#ifdef BW_QT_DEBUG
template< typename T>
QWidget* Data<T>::createWidget(QWidget* parent)
{
    return WidgetCreator<T>::create(this, parent);
}
#endif