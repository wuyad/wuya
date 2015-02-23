#ifndef __WUYA_REF_COUNT_H__
#define __WUYA_REF_COUNT_H__

namespace wuya
{
	/**
	 * 保存引用计数并提供增加、减少引用计数的方法
	 *
	 * @author Scott Meyers
	 * @version 1.0.0
	 */
	class ref_count {
	public:
		void add_ref();
		void rel_ref();
		void set_unshareable();
		bool is_shareable() const;
		bool is_shared() const;
	protected:
		ref_count();
		ref_count(const ref_count& /*rhs*/);
		ref_count& operator=(const ref_count&	/*rhs*/);
		virtual ~ref_count();
	private:
		int  count_;
		bool shareable_;
	};

	/**
	 * 辅助引用计数类的灵巧指针
	 *
	 * @author Scott Meyers
	 * @version 1.0.0
	 */
	template<class T>
	class rc_ptr {
	public:
		rc_ptr(T* ptr = 0);
		rc_ptr(const rc_ptr& rhs);
		~rc_ptr();
		rc_ptr& operator=(const rc_ptr& rhs);
		T* operator->() const;
		T& operator*() const;
		bool is_null() const;
		bool is_valid() const;
	private:
		T* pointee_;
		void init();
	};


	/**
	 * 辅助引用计数类的灵巧指针，负责托管对像的删除，支持写时复制
	 *
	 * @author Scott Meyers
	 */
	template<class T>
	class rci_ptr {
	public:
		rci_ptr(T* ptr = 0);
		rci_ptr(const rci_ptr& rhs);
		~rci_ptr();
		rci_ptr& operator=(const rci_ptr& rhs);
		const T* operator->() const;
		const T& operator*() const;
		T* operator->() ;
		T& operator*() ;
		ref_count& get_rc_object();
		bool is_null() const;
		bool is_valid() const;
		void make_copy();
	private:
		struct count_holder: public ref_count {
			~count_holder();
			T* pointee_;
		};

		count_holder* counter_;
		void init();
	};
}

//.............................实现部分.............................//
namespace wuya{
	inline void ref_count::add_ref() {
		++count_;
	}
	inline void ref_count::rel_ref() {
		if (--count_ == 0)
			delete this;
	}
	inline void ref_count::set_unshareable() {
		shareable_ = false;
	}
	inline bool ref_count::is_shareable() const {
		return shareable_;
	}
	inline bool ref_count::is_shared() const {
		return count_ > 1;
	}
	inline ref_count::ref_count() : count_(0), shareable_(true) {
	}
	inline ref_count::ref_count(const ref_count& /*rhs*/) : count_(0), shareable_(true) {
	}
	inline ref_count& ref_count::operator=(const ref_count&	/*rhs*/) {
		return *this;
	}
	inline ref_count::~ref_count() {
	}


	template<class T>
	rc_ptr<T>::rc_ptr(T* ptr) : pointee_(ptr) {
		init();
	}
	template<class T>
	rc_ptr<T>::rc_ptr(const rc_ptr& rhs) : pointee_(rhs.pointee_) {
		init();
	}
	template<class T>
	rc_ptr<T>::~rc_ptr() {
		if (pointee_) pointee_->rel_ref();
	}
	template<class T>
	rc_ptr<T>& rc_ptr<T>::operator=(const rc_ptr& rhs) {
		if (pointee_ != rhs.pointee_) {
			T* old_pointee = pointee_;
			pointee_ = rhs.pointee_;
			init();
			if (old_pointee)
				old_pointee->rel_ref();
		}
		return *this;
	}
	template<class T>
	T* rc_ptr<T>::operator->() const {
		return pointee_;
	};
	template<class T>
	T& rc_ptr<T>::operator*() const {
		return *pointee_;
	};
	template<class T>
	bool rc_ptr<T>::is_null() const {
		return pointee_==NULL;
	}
	template<class T>
	bool rc_ptr<T>::is_valid() const {
		return pointee_!=NULL;
	}
	template<class T>
	void rc_ptr<T>::init() {
		if (pointee_ == 0)
			return;

		if (pointee_->is_shareable() == false)
			pointee_ = new T(*pointee_);

		pointee_->add_ref();
	}


	template<class T>
	rci_ptr<T>::rci_ptr(T* ptr) :
	counter_(new count_holder) {
		counter_->pointee_ = ptr;
		init();
	}
	template<class T>
	rci_ptr<T>::rci_ptr(const rci_ptr& rhs) : counter_(rhs.counter_) {
		init();
	}
	template<class T>
	rci_ptr<T>::~rci_ptr() {
		counter_->rel_ref();
	}
	template<class T>
	rci_ptr<T>& rci_ptr<T>::operator=(const rci_ptr<T>& rhs) {
		if (counter_ != rhs.counter_) {
			counter_->rel_ref();
			counter_ = rhs.counter_;
			init();
		}
		return *this;
	}
	template<class T>
	ref_count& rci_ptr<T>::get_rc_object() {
		return *counter_;
	}
	template<class T>
	bool rci_ptr<T>::is_null() const {
		return counter_==NULL?true:counter_->pointee_==NULL;
	}
	template<class T>
	bool rci_ptr<T>::is_valid() const {
		return counter_==NULL?false:counter_->pointee_!=NULL;
	}
	template<class T>
	rci_ptr<T>::count_holder::~count_holder() {
		delete pointee_;
	}
	template<class T>
	void rci_ptr<T>::init() {
		if (counter_->is_shareable() == false) {
			T* pOldValue = counter_->pointee_;
			counter_ = new count_holder;
			counter_->pointee_ = new T(*pOldValue);
		}
		counter_->add_ref();
	}
	template<class T>
	const T* rci_ptr<T>::operator->() const {
		return counter_->pointee_;
	}
	template<class T>
	T* rci_ptr<T>::operator->() {
		make_copy();
		return counter_->pointee_;
	}
	template<class T>
	const T& rci_ptr<T>::operator*() const {
		return *(counter_->pointee_);
	}
	template<class T>
	T& rci_ptr<T>::operator*() {
		make_copy();
		return *(counter_->pointee_);
	}

	template<class T>
	void rci_ptr<T>::make_copy() {
		if (counter_->is_shared()) {
			T *old_value = counter_->pointee_;
			counter_->rel_ref();
			counter_ = new count_holder;
			counter_->pointee_ = new T(*old_value);
			counter_->add_ref();
		}
	}
}
#endif

