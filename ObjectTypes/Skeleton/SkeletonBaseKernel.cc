#include "SkeletonBaseKernel.hh"

void SkeletonBaseKernel::property_stats(std::ostream& _ostr) const {
	const SkeletonPropertyContainer::Properties& jps =
			jprops_.properties();
	const SkeletonPropertyContainer::Properties& sps =
			sprops_.properties();

	SkeletonPropertyContainer::Properties::const_iterator it;

	_ostr << jprops_.size() << " jprops:\n";
	for (it = jps.begin(); it != jps.end(); ++it) {
		*it == NULL ? (void) (_ostr << "[deleted]" << "\n") : (*it)->stats(
				_ostr);
	}
	_ostr << sprops_.size() << " sprops:\n";
	for (it = sps.begin(); it != sps.end(); ++it) {
		*it == NULL ? (void) (_ostr << "[deleted]" << "\n") : (*it)->stats(
				_ostr);
	}
}

void SkeletonBaseKernel::jprop_stats(std::string& _string) const {
	_string.clear();

	SkeletonPropertyContainer::Properties::const_iterator it;
	const SkeletonPropertyContainer::Properties& jps =
			jprops_.properties();
	for (it = jps.begin(); it != jps.end(); ++it)
		if (*it == NULL)
			_string += "[deleted] \n";
		else {
			_string += (*it)->name();
			_string += "\n";
		}
}

void SkeletonBaseKernel::sprop_stats(std::string& _string) const {
	_string.clear();

	SkeletonPropertyContainer::Properties::const_iterator it;
	const SkeletonPropertyContainer::Properties& sps =
			sprops_.properties();
	for (it = sps.begin(); it != sps.end(); ++it)
		if (*it == NULL)
			_string += "[deleted] \n";
		else {
			_string += (*it)->name();
			_string += "\n";
		}

}

void SkeletonBaseKernel::jprop_stats(std::ostream& _ostr) const {
	SkeletonPropertyContainer::Properties::const_iterator it;
	const SkeletonPropertyContainer::Properties& jps =
			jprops_.properties();
	for (it = jps.begin(); it != jps.end(); ++it)
		*it == NULL ? (void) (_ostr << "[deleted]" << "\n") : (*it)->stats(
				_ostr);
}

void SkeletonBaseKernel::sprop_stats(std::ostream& _ostr) const {
	SkeletonPropertyContainer::Properties::const_iterator it;
	const SkeletonPropertyContainer::Properties& sps =
			sprops_.properties();
	for (it = sps.begin(); it != sps.end(); ++it)
		*it == NULL ? (void) (_ostr << "[deleted]" << "\n") : (*it)->stats(
				_ostr);
}
