#pragma once
#ifndef NULL
#define NULL 0
#endif

template <class T>
class CSingleton
{
public :

    //----------------------------------------------------------------
    // Return the unique instance of the class
    //----------------------------------------------------------------
    static T& Instance()
    {
        if (!Inst)
            Inst = new T;

        return *Inst;
    }

    //----------------------------------------------------------------
    // Destroy the unique instance of the class
    //----------------------------------------------------------------
    static void Destroy()
    {
        delete Inst;
        Inst = NULL;
    }

protected :

    //----------------------------------------------------------------
    // Constructor by default
    //----------------------------------------------------------------
    CSingleton() {}

    //----------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------
    ~CSingleton() {}

private :

    //----------------------------------------------------------------
    // Member data
    //----------------------------------------------------------------
    static T* Inst; // Instance de la classe

    //----------------------------------------------------------------
    // Copy forbiden
    //----------------------------------------------------------------
    CSingleton(CSingleton&);
    void operator =(CSingleton&);
};

//----------------------------------------------------------------
// Declaration of the static variable
//----------------------------------------------------------------
template <class T> T* CSingleton<T>::Inst = NULL;
