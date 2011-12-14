//=============================================================================
//
//  CLASS SkeletonBaseKernel
//
//=============================================================================


#ifndef SKELETONBASEKERNEL_HH
#define SKELETONBASEKERNEL_HH

//== INCLUDES =================================================================

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
// --------------------
#include "SkeletonProperty.hh"
#include "SkeletonPropertyContainer.hh"

//== CLASS DEFINITION =========================================================

/// This class provides the basic property management like adding/removing
/// properties and access to properties.
/// All operations provided by %SkeletonBaseKernel need at least a property handle
/// (JPropHandleT, SPropHandleT) which keeps the data type of the property, too.

class SkeletonBaseKernel {
public:
	//-------------------------------------------- constructor / destructor

	SkeletonBaseKernel() {
	}

	virtual ~SkeletonBaseKernel() {
		jprops_.clear();
		sprops_.clear();
	}

public:
	//-------------------------------------------------- add new properties

	template<class T>
	void add_property(JPropHandleT<T>& _ph,
			const std::string& _name = "<jprop>") {
		_ph = JPropHandleT<T> (jprops_.add(T(), _name));
		jprops_.resize(jointCount());
	}

	template<class T>
	void add_property(SPropHandleT<T>& _ph,
			const std::string& _name = "<sprop>") {
		_ph = SPropHandleT<T> (sprops_.add(T(), _name));
		sprops_.resize(1);
	}

public:
	//--------------------------------------------------- remove properties

	template<typename T>
	void remove_property(JPropHandleT<T>& _ph) {
		if (_ph.is_valid())
			jprops_.remove(_ph);
		_ph.reset();
	}

	template<typename T>
	void remove_property(SPropHandleT<T>& _ph) {
		if (_ph.is_valid())
			sprops_.remove(_ph);
		_ph.reset();
	}

	//--------------------------------------------------- remove single elements

	void remove_jprop_element(size_t _idx) {
	    for(prop_iterator p_it = jprops_.begin(); p_it != jprops_.end(); ++p_it) {
	        (*p_it)->delete_element(_idx);
	    }
	}

public:
	//------------------------------------------------ get handle from name

	template<class T>
	bool get_property_handle(JPropHandleT<T>& _ph, const std::string& _name) const {
		return (_ph = JPropHandleT<T> (jprops_.handle(T(), _name))).is_valid();
	}

	template<class T>
	bool get_property_handle(SPropHandleT<T>& _ph, const std::string& _name) const {
		return (_ph = SPropHandleT<T> (sprops_.handle(T(), _name))).is_valid();
	}

public:
	//--------------------------------------------------- access properties

	template<class T>
	SkeletonPropertyT<T>& property(JPropHandleT<T> _ph) {
		return jprops_.property(_ph);
	}
	template<class T>
	const SkeletonPropertyT<T>& property(JPropHandleT<T> _ph) const {
		return jprops_.property(_ph);
	}

	template<class T>
	SkeletonPropertyT<T>& property(SPropHandleT<T> _ph) {
		return sprops_.property(_ph);
	}
	template<class T>
	const SkeletonPropertyT<T>& property(SPropHandleT<T> _ph) const {
		return sprops_.property(_ph);
	}

public:
	//-------------------------------------------- access property elements

	template<class T>
	typename JPropHandleT<T>::reference property(JPropHandleT<T> _ph,
			unsigned int _jIdx) {
		return jprops_.property(_ph)[_jIdx];
	}

	template<class T>
	typename JPropHandleT<T>::const_reference property(JPropHandleT<T> _ph,
			unsigned int _jIdx) const {
		return jprops_.property(_ph)[_jIdx];
	}

	template<class T>
	typename SPropHandleT<T>::reference property(SPropHandleT<T> _ph) {
		return sprops_.property(_ph)[0];
	}

	template<class T>
	typename SPropHandleT<T>::const_reference property(SPropHandleT<T> _ph) const {
		return sprops_.property(_ph)[0];
	}

	//@}

protected:
	//------------------------------------------------- low-level access

public:
	// used by non-native kernel and MeshIO, should be protected

	size_t n_jprops(void) const {
		return jprops_.size();
	}

	size_t n_sprops(void) const {
		return sprops_.size();
	}

	SkeletonBaseProperty* _get_jprop(const std::string& _name) {
		return jprops_.property(_name);
	}

	SkeletonBaseProperty* _get_sprop(const std::string& _name) {
		return sprops_.property(_name);
	}

	SkeletonBaseProperty& _jprop(size_t _idx) {
		return jprops_._property(_idx);
	}
	SkeletonBaseProperty& _sprop(size_t _idx) {
		return sprops_._property(_idx);
	}

	const SkeletonBaseProperty& _jprop(size_t _idx) const {
		return jprops_._property(_idx);
	}
	const SkeletonBaseProperty& _sprop(size_t _idx) const {
		return sprops_._property(_idx);
	}

	size_t _add_jprop(SkeletonBaseProperty* _bp) {
		return jprops_._add(_bp);
	}
	size_t _add_sprop(SkeletonBaseProperty* _bp) {
		return sprops_._add(_bp);
	}

protected:
	// low-level access non-public

	SkeletonBaseProperty& _jprop(unsigned int _h) {
		return jprops_._property(_h);
	}
	SkeletonBaseProperty& _sprop(unsigned int _h) {
		return sprops_._property(_h);
	}

	const SkeletonBaseProperty& _jprop(unsigned int _h) const {
		return jprops_._property(_h);
	}
	const SkeletonBaseProperty& _sprop(unsigned int _h) const {
		return sprops_._property(_h);
	}

public:
	//----------------------------------------------------- element numbers

	virtual unsigned int jointCount() const {
		return 0u;
	}

protected:
	//------------------------------------------- synchronize properties

	void jprops_reserve(unsigned int _n) const {
		jprops_.reserve(_n);
	}
	void jprops_resize(unsigned int _n) const {
		jprops_.resize(_n);
	}
	void jprops_clear() {
		jprops_.clear();
	}
	void jprops_swap(unsigned int _i0, unsigned int _i1) const {
		jprops_.swap(_i0, _i1);
	}
	void jprops_copy(const SkeletonPropertyContainer& _jprops) {
	    jprops_.clear();
	    jprops_ = _jprops;
	}

	void sprops_resize(unsigned int _n) const {
		sprops_.resize(_n);
	}
	void sprops_clear() {
		sprops_.clear();
	}
	void sprops_copy(const SkeletonPropertyContainer& _sprops) {
        sprops_.clear();
        sprops_ = _sprops;
    }

public:

	void property_stats(std::ostream& _ostr = std::clog) const;

	void jprop_stats(std::string& _string) const;
	void sprop_stats(std::string& _string) const;

	void jprop_stats(std::ostream& _ostr = std::clog) const;
	void sprop_stats(std::ostream& _ostr = std::clog) const;

public:

	typedef SkeletonPropertyContainer::iterator         prop_iterator;
	typedef SkeletonPropertyContainer::const_iterator   const_prop_iterator;

	prop_iterator jprops_begin() {
		return jprops_.begin();
	}
	prop_iterator jprops_end() {
		return jprops_.end();
	}
	const_prop_iterator jprops_begin() const {
		return jprops_.begin();
	}
	const_prop_iterator jprops_end() const {
		return jprops_.end();
	}

	prop_iterator sprops_begin() {
		return sprops_.begin();
	}
	prop_iterator sprops_end() {
		return sprops_.end();
	}
	const_prop_iterator sprops_begin() const {
		return sprops_.begin();
	}
	const_prop_iterator sprops_end() const {
		return sprops_.end();
	}

protected:

	SkeletonPropertyContainer jprops_;
	SkeletonPropertyContainer sprops_;
};

//=============================================================================
#endif // SKELETONMESHBASEKERNEL_HH defined
//=============================================================================
