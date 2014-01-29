/*
 * ResourceManagerNode.hh
 *
 *  Created on: Jan 13, 2014
 *      Author: ebke
 */

#ifndef RESOURCEMANAGERNODE_HH_
#define RESOURCEMANAGERNODE_HH_

#include "BaseNode.hh"

namespace ACG {
namespace SceneGraph {

/**
 * The sole purpose of this node is to guarantee deletion
 * of a resource the lifecycle of which should be bound to
 * the parent node.
 *
 * A use case would be a MeshNodeT<TriMesh> which displays
 * a TriMesh that is not needed anywhere else. Add a
 * PtrResourceManagerNodeT encapsulating that mesh as a child
 * node to the MeshNode.
 */
template<typename T>
class PtrResourceManagerNodeT : public ACG::SceneGraph::BaseNode {
    ACG_CLASSNAME(PtrResourceManagerNodeT);

    public:
        PtrResourceManagerNodeT(
                T* resource, BaseNode* _parent=0,
                std::string _name="<unknown>")
            : BaseNode(_parent, _name), resource(resource) {}

        PtrResourceManagerNodeT(
                BaseNode* _parent=0, std::string _name="<unknown>")
            : BaseNode(_parent, _name), resource(0) {}

        ~PtrResourceManagerNodeT() {
            delete resource;
        }

        /**
         * Deletes the currently managed resource (if any)
         * and takes ownership of the newly supplied one (if any).
         */
        void reset(T* new_resource = 0) {
            delete resource;
            resource = new_resource;
        }

    private:
        T* resource;
};

/**
 * This class does the same as PtrResourceManagerNodeT but uses
 * delete[] instead of delete.
 */
template<typename T>
class ArrayResourceManagerNodeT : public ACG::SceneGraph::BaseNode {
    ACG_CLASSNAME(ArrayResourceManagerNodeT);

    public:
        ArrayResourceManagerNodeT(
                T* resource, BaseNode* _parent=0,
                std::string _name="<unknown>")
            : BaseNode(_parent, _name), resource(resource) {}

        ArrayResourceManagerNodeT(
                BaseNode* _parent=0, std::string _name="<unknown>")
            : BaseNode(_parent, _name), resource(0) {}

        ~ArrayResourceManagerNodeT() {
            delete[] resource;
        }

        /**
         * Deletes the currently managed resource (if any)
         * and takes ownership of the newly supplied one (if any).
         */
        void reset(T* new_resource = 0) {
            delete[] resource;
            resource = new_resource;
        }

    private:
        T* resource;
};

} /* namespace SceneGraph */
} /* namespace ACG */
#endif /* RESOURCEMANAGERNODE_HH_ */
