#ifndef __WUYA_AUTO_DELETE_PTR_H__
#define __WUYA_AUTO_DELETE_PTR_H__

namespace wuya{
	/**
	 * 用于函数返回内部分配的堆内存（通过new XXX形式的分配）的自动释放
	 *
	 * @author wuya
	 * @version 1.0.0
	 */
	template <class T>
	class auto_delete_ptr {
	public:
		auto_delete_ptr(T* t);
		T* operator ->();
		T& operator *();
		T* get();
		~auto_delete_ptr();
	protected:
		T* t_;
	};

	/**
	 * 用于函数返回内部分配的堆内存（通过new XXX[YY]形式的分配）的自动释放
	 *
	 * @author wuya
	 */
	template <class T>
	class auto_delete_array {
	public:
		auto_delete_array(T* t);
		T* operator [](int index);
		~auto_delete_array();
	protected:
		T* t_;
	};
}

//.............................实现部分.............................//
namespace wuya{
	template <class T>
	auto_delete_ptr<T>::auto_delete_ptr(T* t):t_(t) {
	}
	template <class T>
	T* auto_delete_ptr<T>::operator ->() {
		return t_;
	}
	template <class T>
	T& auto_delete_ptr<T>::operator *() {
		return *t_;
	}
	template <class T>
	T* auto_delete_ptr<T>::get() {
		return t_;
	}
	template <class T>
	auto_delete_ptr<T>::~auto_delete_ptr() {
		delete t_;
	}

	template <class T>
	auto_delete_array<T>::auto_delete_array(T* t):t_(t) {
	}
	template <class T>
	T* auto_delete_array<T>::operator [](int index) {
		return t_+index;
	}
	template <class T>
	auto_delete_array<T>::~auto_delete_array() {
		delete [] t_;
	}
}

#endif

