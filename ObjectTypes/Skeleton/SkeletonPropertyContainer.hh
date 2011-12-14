#ifndef SKELETONPROPERTYCONTAINER_HH
#define SKELETONPROPERTYCONTAINER_HH

#include <algorithm>
#include <cassert>

#include "SkeletonProperty.hh"

//== FORWARDDECLARATIONS ======================================================
class BaseKernel;

//== CLASS DEFINITION =========================================================
/// A a container for properties.
class SkeletonPropertyContainer {
public:

	//-------------------------------------------------- constructor / destructor

	SkeletonPropertyContainer() {
	}
	virtual ~SkeletonPropertyContainer() {
		std::for_each(properties_.begin(), properties_.end(), Delete());
	}

	//------------------------------------------------------------- info / access

	typedef std::vector<SkeletonBaseProperty*> Properties;
	const Properties& properties() const {
		return properties_;
	}
	size_t size() const {
		return properties_.size();
	}

	//--------------------------------------------------------- copy / assignment

	SkeletonPropertyContainer(const SkeletonPropertyContainer& _rhs) {
		operator=(_rhs);
	}

	SkeletonPropertyContainer& operator=(const SkeletonPropertyContainer& _rhs) {
		// The assignment below relies on all previous SkeletonBaseProperty* elements having been deleted
		std::for_each(properties_.begin(), properties_.end(), Delete());
		properties_ = _rhs.properties_;
		Properties::iterator p_it = properties_.begin(), p_end =
				properties_.end();
		for (; p_it != p_end; ++p_it)
			if (*p_it)
				*p_it = (*p_it)->clone();
		return *this;
	}

	//--------------------------------------------------------- manage properties

	template<class T>
	SkeletonBasePropHandleT<T> add(const T&, const std::string& _name = "<unknown>") {
		Properties::iterator p_it = properties_.begin(), p_end =
				properties_.end();
		int idx = 0;
		for (; p_it != p_end && *p_it != NULL; ++p_it, ++idx) {
		}
		if (p_it == p_end)
			properties_.push_back(NULL);
		properties_[idx] = new SkeletonPropertyT<T> (_name);
		return SkeletonBasePropHandleT<T> (idx);
	}

	template<class T>
	SkeletonBasePropHandleT<T> handle(const T&, const std::string& _name) const {
		Properties::const_iterator p_it = properties_.begin();
		for (int idx = 0; p_it != properties_.end(); ++p_it, ++idx) {
			if (*p_it != NULL && (*p_it)->name() == _name) {
				return SkeletonBasePropHandleT<T> (idx);
			}
		}
		return SkeletonBasePropHandleT<T> ();
	}

	SkeletonBaseProperty* property(const std::string& _name) const {
		Properties::const_iterator p_it = properties_.begin();
		for (int idx = 0; p_it != properties_.end(); ++p_it, ++idx) {
			if (*p_it != NULL && (*p_it)->name() == _name) //skip deleted properties
			{
				return *p_it;
			}
		}
		return NULL;
	}

	template<class T> SkeletonPropertyT<T>& property(SkeletonBasePropHandleT<T> _h) {
		assert(_h.idx() >= 0 && _h.idx() < (int) properties_.size());
		assert(properties_[_h.idx()] != NULL);
		SkeletonPropertyT<T>* p = dynamic_cast<SkeletonPropertyT<T>*> (properties_[_h.idx()]);
		assert(p != NULL);
		return *p;
	}

	template<class T> const SkeletonPropertyT<T>& property(SkeletonBasePropHandleT<T> _h) const {
		assert(_h.idx() >= 0 && _h.idx() < (int) properties_.size());
		assert(properties_[_h.idx()] != NULL);
		SkeletonPropertyT<T>* p = dynamic_cast<SkeletonPropertyT<T>*> (properties_[_h.idx()]);
		assert(p != NULL);
		return *p;
	}

	template<class T> void remove(SkeletonBasePropHandleT<T> _h) {
		assert(_h.idx() >= 0 && _h.idx() < (int) properties_.size());
		delete properties_[_h.idx()];
		properties_[_h.idx()] = NULL;
	}

	void clear() {
		// Clear properties vector:
		// Replaced the old version with new one
		// which performs a swap to clear values and
		// deallocate memory.

		// Old version (changed 22.07.09) {
		// std::for_each(properties_.begin(), properties_.end(), Delete());
		// }

		std::for_each(properties_.begin(), properties_.end(), ClearAll());
	}

	//---------------------------------------------------- synchronize properties

	void reserve(size_t _n) const {
		std::for_each(properties_.begin(), properties_.end(), Reserve(_n));
	}

	void resize(size_t _n) const {
		std::for_each(properties_.begin(), properties_.end(), Resize(_n));
	}

	void swap(size_t _i0, size_t _i1) const {
		std::for_each(properties_.begin(), properties_.end(), Swap(_i0, _i1));
	}

protected:
	// generic add/get

	size_t _add(SkeletonBaseProperty* _bp) {
		Properties::iterator p_it = properties_.begin(), p_end =
				properties_.end();
		size_t idx = 0;
		for (; p_it != p_end && *p_it != NULL; ++p_it, ++idx) {
		};
		if (p_it == p_end)
			properties_.push_back(NULL);
		properties_[idx] = _bp;
		return idx;
	}

	SkeletonBaseProperty& _property(size_t _idx) {
		assert(_idx < properties_.size());
		assert(properties_[_idx] != NULL);
		SkeletonBaseProperty *p = properties_[_idx];
		assert(p != NULL);
		return *p;
	}

	const SkeletonBaseProperty& _property(size_t _idx) const {
		assert(_idx < properties_.size());
		assert(properties_[_idx] != NULL);
		SkeletonBaseProperty *p = properties_[_idx];
		assert(p != NULL);
		return *p;
	}

	typedef Properties::iterator iterator;
	typedef Properties::const_iterator const_iterator;
	iterator begin() {
		return properties_.begin();
	}
	iterator end() {
		return properties_.end();
	}
	const_iterator begin() const {
		return properties_.begin();
	}
	const_iterator end() const {
		return properties_.end();
	}

	friend class SkeletonBaseKernel;

private:

	//-------------------------------------------------- synchronization functors

#ifndef DOXY_IGNORE_THIS
	struct Reserve {
		Reserve(size_t _n) :
			n_(_n) {
		}
		void operator()(SkeletonBaseProperty* _p) const {
			if (_p)
				_p->reserve(n_);
		}
		size_t n_;
	};

	struct Resize {
		Resize(size_t _n) :
			n_(_n) {
		}
		void operator()(SkeletonBaseProperty* _p) const {
			if (_p)
				_p->resize(n_);
		}
		size_t n_;
	};

	struct ClearAll {
		ClearAll() {
		}
		void operator()(SkeletonBaseProperty* _p) const {
			if (_p)
				_p->clear();
		}
	};

	struct Swap {
		Swap(size_t _i0, size_t _i1) :
			i0_(_i0), i1_(_i1) {
		}
		void operator()(SkeletonBaseProperty* _p) const {
			if (_p)
				_p->swap(i0_, i1_);
		}
		size_t i0_, i1_;
	};

	struct Delete {
		Delete() {
		}
		void operator()(SkeletonBaseProperty* _p) const {
			if (_p)
				delete _p;
			_p = NULL;
		}
	};
#endif

	Properties properties_;
};

#endif //SKELETONPROPERTYCONTAINER_HH
