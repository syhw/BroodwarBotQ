#include "Data.h"

template< class T>
Data<T>::Data()
{

}

template< class T>
Data<T>::~Data()
{

}

template< class T>
const std::string& Data<T>::getName() const
{
	return data_name;
}

template< class T>
std::ostream& Data<T>::operator <<(const std::ostream& os) const
{
	//os << value;
	return os;
}

template< class T>
std::istream& Data<T>::operator >>(const std::istream& is) const
{
	//is >> value;
	return is;
}

