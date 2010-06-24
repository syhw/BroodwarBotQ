#include "Data.h"

template< class T>
Data<T>::Data()
{
    /*
ghMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

// if (ghMutex == NULL) 
//	 Broodwar->printf("bad mutex creation");
        */
}
 

template< class T>
Data<T>::~Data()
{
    //CloseHandle(ghMutex);
}


template< class T>
const T& Data<T>::getValue() const
{
	// verifier que personne n'ecrit en ce moment ?
	return value;
}


template< class T>
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


template< class T>
void Data<T>::endEdit()
{
	// lacher le mutex
        //ReleaseMutex(ghMutex);

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
