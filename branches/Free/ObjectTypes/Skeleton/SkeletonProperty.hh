#ifndef SKELETONPROPERTY_HH
#define SKELETONPROPERTY_HH

//== INCLUDES =================================================================

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cassert>

#include "SkeletonBaseProperty.hh"

//== CLASS DEFINITION =========================================================

/** \class SkeletonPropertyT
 *
 *  \brief Default property class for any type T.
 *
 *  The default property class for any type T.
 *
 *  The property supports persistence if T is a "fundamental" type:
 *  - integer fundamental types except bool:
 *    char, short, int, long, long long (__int64 for MS VC++) and
 *    their unsigned companions.
 *  - float fundamentals except <tt>long double</tt>:
 *    float, double
 */

template<class T>
class SkeletonPropertyT: public SkeletonBaseProperty {
public:

	typedef T 										Value;
	typedef std::vector<T> 							vector_type;
	typedef T 										value_type;
	typedef typename vector_type::reference 		reference;
	typedef typename vector_type::const_reference 	const_reference;

public:

	/// Default constructor
	SkeletonPropertyT(const std::string& _name = "<unknown>") :
		SkeletonBaseProperty(_name) {
	}

	/// Copy constructor
	SkeletonPropertyT(const SkeletonPropertyT& _rhs) :
		SkeletonBaseProperty(_rhs), data_(_rhs.data_) {
	}

public:
	// inherited from SkeletonBaseProperty
	virtual void reserve(size_t _n) {
		data_.reserve(_n);
	}
	virtual void resize(size_t _n) {
		data_.resize(_n, Value());
	}
	virtual void clear() {
		data_.clear();
		vector_type().swap(data_);
	}
	virtual void push_back() {
		data_.push_back(T());
	}
	virtual void swap(size_t _i0, size_t _i1) {
		std::swap(data_[_i0], data_[_i1]);
	}

	void delete_element(size_t _idx) {
	    data_.erase(data_.begin() + _idx);
	}

	virtual void stats(std::ostream& _ostr) const {
		for(typename vector_type::const_iterator it = data_.begin();
			it != data_.end(); ++it) {
				_ostr << *it << " ";
		}
	}

public:

	virtual size_t n_elements() const {
		return data_.size();
	}
	virtual size_t element_size() const {
		return sizeof(T);
	}

#ifndef DOXY_IGNORE_THIS
	struct plus {
		size_t operator ()(size_t _b, const T& _v) {
			return _b + sizeof(T);
		}
	};
#endif

	virtual size_t size_of() const {
		if (element_size() != SkeletonBaseProperty::UnknownSize)
			return this->SkeletonBaseProperty::size_of(n_elements());
		return std::accumulate(data_.begin(), data_.end(), 0, plus());
	}

	virtual size_t size_of(size_t _n_elem) const {
		return this->SkeletonBaseProperty::size_of(_n_elem);
	}

public:
	// data access interface

	/// Get pointer to array (does not work for T==bool)
	const T* data() const {

		if (data_.empty())
			return 0;

		return &data_[0];
	}

	/// Get reference to property vector (be careful, improper usage, e.g. resizing, may crash)
	vector_type& data_vector() {

		return data_;
	}

	/// Access the i'th element. No range check is performed!
	reference operator[](int _idx) {
		assert(size_t(_idx) < data_.size());
		return data_[_idx];
	}

	/// Const access to the i'th element. No range check is performed!
	const_reference operator[](int _idx) const {
		assert(size_t(_idx) < data_.size());
		return data_[_idx];
	}

	/// Make a copy of self.
	SkeletonPropertyT<T>* clone() const {
		SkeletonPropertyT<T>* p = new SkeletonPropertyT<T> (*this);
		return p;
	}

private:

	vector_type data_;
};

//-----------------------------------------------------------------------------


/** \class SkeletonPropertyT<bool>

 Property specialization for bool type.

 */
template<>
class SkeletonPropertyT<bool> : public SkeletonBaseProperty {
public:

	typedef std::vector<bool> 				vector_type;
	typedef bool 							value_type;
	typedef vector_type::reference 			reference;
	typedef vector_type::const_reference 	const_reference;

public:

	SkeletonPropertyT(const std::string& _name = "<unknown>") :
		SkeletonBaseProperty(_name) {
	}

public:
	// inherited from SkeletonBaseProperty

	virtual void reserve(size_t _n) {
		data_.reserve(_n);
	}
	virtual void resize(size_t _n) {
		data_.resize(_n);
	}
	virtual void clear() {
		data_.clear();
		vector_type().swap(data_);
	}
	virtual void push_back() {
		data_.push_back(bool());
	}
	virtual void swap(size_t _i0, size_t _i1) {
		bool t(data_[_i0]);
		data_[_i0] = data_[_i1];
		data_[_i1] = t;
	}

	void delete_element(size_t _idx) {
        data_.erase(data_.begin() + _idx);
    }

public:

//	virtual void set_persistent(bool _yn) {
//		check_and_set_persistent<bool> (_yn);
//	}

	virtual size_t n_elements() const {
		return data_.size();
	}
	virtual size_t element_size() const {
		return SkeletonBaseProperty::UnknownSize;
	}
	virtual size_t size_of() const {
		return size_of(n_elements());
	}
	virtual size_t size_of(size_t _n_elem) const {
		return _n_elem / 8 + ((_n_elem % 8) != 0);
	}

	virtual void stats(std::ostream& _ostr) const {
		for(vector_type::const_iterator it = data_.begin();
			it != data_.end(); ++it) {
				_ostr << (*it ? "true" : "false") << " ";
		}
	}

public:

	/// Access the i'th element. No range check is performed!
	reference operator[](int _idx) {
		assert(size_t(_idx) < data_.size());
		return data_[_idx];
	}

	/// Const access to the i'th element. No range check is performed!
	const_reference operator[](int _idx) const {
		assert(size_t(_idx) < data_.size());
		return data_[_idx];
	}

	/// Make a copy of self.
	SkeletonPropertyT<bool>* clone() const {
		SkeletonPropertyT<bool>* p = new SkeletonPropertyT<bool> (
				*this);
		return p;
	}

private:

	vector_type data_;
};

//-----------------------------------------------------------------------------


/** \class SkeletonPropertyT<std::string>

 Property specialization for std::string type.
 */
template<>
class SkeletonPropertyT<std::string> : public SkeletonBaseProperty {
public:

	typedef std::string 					Value;
	typedef std::vector<std::string> 		vector_type;
	typedef std::string 					value_type;
	typedef vector_type::reference 			reference;
	typedef vector_type::const_reference 	const_reference;

public:

	SkeletonPropertyT(const std::string& _name = "<unknown>") :
		SkeletonBaseProperty(_name) {
	}

public:
	// inherited from SkeletonBaseProperty

	virtual void reserve(size_t _n) {
		data_.reserve(_n);
	}
	virtual void resize(size_t _n) {
		data_.resize(_n);
	}
	virtual void clear() {
		data_.clear();
		vector_type().swap(data_);
	}
	virtual void push_back() {
		data_.push_back(std::string());
	}
	virtual void swap(size_t _i0, size_t _i1) {
		std::swap(data_[_i0], data_[_i1]);
	}

	virtual void delete_element(size_t _idx) {
        data_.erase(data_.begin() + _idx);
    }

public:

//	virtual void set_persistent(bool _yn) {
//		check_and_set_persistent<std::string> (_yn);
//	}

	virtual size_t n_elements() const {
		return data_.size();
	}
	virtual size_t element_size() const {
		return SkeletonBaseProperty::UnknownSize;
	}
	virtual size_t size_of() const {
		return sizeof(data_);
	}

	virtual size_t size_of(size_t /* _n_elem */) const {
		return SkeletonBaseProperty::UnknownSize;
	}

	virtual void stats(std::ostream& _ostr) const {
		for(vector_type::const_iterator it = data_.begin();
			it != data_.end(); ++it) {
				_ostr << *it << " ";
		}
	}

public:

	const value_type* data() const {
		if (data_.empty())
			return 0;

		return (value_type*) &data_[0];
	}

	/// Access the i'th element. No range check is performed!
	reference operator[](int _idx) {
		assert(size_t(_idx) < data_.size());
		return ((value_type*) &data_[0])[_idx];
	}

	/// Const access the i'th element. No range check is performed!
	const_reference operator[](int _idx) const {
		assert(size_t(_idx) < data_.size());
		return ((value_type*) &data_[0])[_idx];
	}

	SkeletonPropertyT<value_type>* clone() const {
		SkeletonPropertyT<value_type>* p = new SkeletonPropertyT<
				value_type> (*this);
		return p;
	}

private:

	vector_type data_;

};

/// Base property handle.
template<class T>
struct SkeletonBasePropHandleT {

	typedef T 										Value;
	typedef std::vector<T> 							vector_type;
	typedef T 										value_type;
	typedef typename vector_type::reference 		reference;
	typedef typename vector_type::const_reference 	const_reference;

public:
    // Default constructor
    explicit SkeletonBasePropHandleT(int _idx) : idx_(_idx) {};

    SkeletonBasePropHandleT& operator=(int _idx) {
        idx_ = _idx;
        return *this;
    }

    SkeletonBasePropHandleT& operator=(const SkeletonBasePropHandleT& _idx) {
        idx_ = _idx.idx_;
        return *this;
    }

    inline bool is_valid() const { return idx_ != -1; }

    inline bool operator<(const SkeletonBasePropHandleT& _idx) const { return (this->idx_ < _idx.idx_); }

    inline bool operator<(int _idx) const { return idx_ < _idx; }

    inline int idx() const { return idx_; }

    void idx(const int& _idx) { idx_ = _idx; }

    void reset() { idx_ = -1; }

    operator int() const { return idx_; }

private:
	int idx_;
};

/** \ingroup mesh_property_handle_group
 *  Handle representing a joint property
 */
template<class T>
struct JPropHandleT: public SkeletonBasePropHandleT<T> {
	typedef T Value;
	typedef T value_type;

	explicit JPropHandleT(int _idx = -1) :
		SkeletonBasePropHandleT<T> (_idx) {
	}
	explicit JPropHandleT(const SkeletonBasePropHandleT<T>& _b) :
		SkeletonBasePropHandleT<T> (_b) {
	}
};

/** \ingroup mesh_property_handle_group
 *  Handle representing a skeleton property
 */
template<class T>
struct SPropHandleT: public SkeletonBasePropHandleT<T> {
	typedef T Value;
	typedef T value_type;

	explicit SPropHandleT(int _idx = -1) :
		SkeletonBasePropHandleT<T> (_idx) {
	}
	explicit SPropHandleT(const SkeletonBasePropHandleT<T>& _b) :
		SkeletonBasePropHandleT<T> (_b) {
	}
};

//=============================================================================
#endif // SKELETONPROPERTY_HH defined
//=============================================================================
