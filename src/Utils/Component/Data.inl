#include <map>
#include "WidgetCreator.h"
#include "Data.h"
#include "BaseObject.h"
#include "WidgetCreator.inl"

template<typename T>
Data<T>::Data(T defaultValue, std::string name, BaseObject* parent)
: _synchronized(true)
, value(defaultValue)
, data_name(name)
{
    if (parent)
        parent->addData(this);
    /*
ghMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

// if (ghMutex == NULL) 
//	 Broodwar->printf("bad mutex creation");
        */
}
 

template< typename T>
Data<T>::~Data()
{
    //CloseHandle(ghMutex);
}

template< typename T>
T& Data<T>::operator*()
{
    return value;
}

template< typename T>
T* Data<T>::operator->()
{
    return &value;
}

template< typename T>
const T& Data<T>::operator*() const
{
    return value;
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
    /*
	DWORD dwCount = 0;
	DWORD dwWaitResult = WaitForSingleObject( 
            ghMutex,    // handle to mutex
            INFINITE);  // no time-out interval
 
        switch (dwWaitResult) 
        {
            // The thread got ownership of the mutex
            case WAIT_OBJECT_0: 
                __try { 
                    // TODO: Write to the database
                    printf("Thread %d writing to database...\n", 
                            GetCurrentThreadId());
                    dwCount++;
                } 

                __finally { 
                    // Release ownership of the mutex object
                    if (! ReleaseMutex(ghMutex)) 
                    { 
                        // Handle error.
                    } 
                } 
                break; 

            // The thread got ownership of an abandoned mutex
            // The database is in an indeterminate state
            case WAIT_ABANDONED: 
                return FALSE; 
        }
*/
	return &value;
}


template< typename T>
void Data<T>::endEdit()
{
	// lacher le mutex
        //ReleaseMutex(ghMutex);
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

template< typename T>
QWidget* Data<T>::createWidget(QWidget* parent)
{
    return WidgetCreator<T>::create(this, parent);
}