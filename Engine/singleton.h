#pragma once

template <class T>
class Singleton
{
public:
	static T* GetInstance();
	static bool IsReady();

	inline Singleton();
	inline ~Singleton();

protected:
	static T* _pcInstance;
};

template<class T>
T* Singleton<T>::_pcInstance = nullptr;

template<class T>
Singleton<T>::Singleton()
{
	if(!IsReady())
	{
		_pcInstance = reinterpret_cast<T*>(this);
	}
}

template<class T>
Singleton<T>::~Singleton()
{
	_pcInstance = nullptr;
}

template<class T>
T* Singleton<T>::GetInstance()
{
	return _pcInstance;
}

template<class T>
bool Singleton<T>::IsReady()
{
	return (_pcInstance != nullptr);
}