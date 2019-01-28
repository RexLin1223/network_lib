#pragma once
#include <boost/thread/mutex.hpp>

namespace kt
{
	namespace log
	{
		template <typename T> class Singleton;

		template <typename T>
		class SingletonAccess
		{
		public:
			T* operator->()
			{
				return t_;
			}

		private:
			friend class Singleton<T>;
			static boost::mutex mutex_;
			boost::mutex::scoped_lock lock_;
			T* t_;
			SingletonAccess(T* t)
				: t_(t)
				, lock_(mutex_)
			{
			}

			SingletonAccess& operator=(const SingletonAccess&);
			SingletonAccess(const SingletonAccess&);
		};

		template <typename T>
		class Singleton
		{
		public:
			static SingletonAccess<T> Get()
			{
				static T t;
				return SingletonAccess<T>(&t);
			}
		};

		template <class T> boost::mutex SingletonAccess<T>::mutex_;

	}
}

