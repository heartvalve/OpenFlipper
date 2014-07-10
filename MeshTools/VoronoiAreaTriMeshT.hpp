/*
 * VoronoiAreaTriMeshTriMeshT.hpp
 *
 *  Created on: Jul 10, 2014
 *      Author: kremer
 */

#ifndef VORONOIAREATRIMESHT_HPP_
#define VORONOIAREATRIMESHT_HPP_

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

/**
 * \brief Implementation of Voronoi area computation
 * as described in "Discrete Differential-Geometry Operators
 * for Triangulated 2-Manifolds" by M. Meyer et al.
 */

template <class MeshT>
class VoronoiAreaTriMeshT {
public:
	VoronoiAreaTriMeshT(const MeshT& _mesh, bool _obtuse_handling = true) :
	mesh_(_mesh),
	obtuse_handling_(_obtuse_handling) {

		const_cast<MeshT&>(mesh_).add_property(cotan_weights_, "VoronoiAreaTriMeshT::CotanWeights");

		if(obtuse_handling_) {

			const_cast<MeshT&>(mesh_).add_property(obtuse_tag_, "VoronoiAreaTriMeshT::ObtuseTag");

			tag_obtuse_triangles();
		}

		compute_cotan_weights();
	}

	virtual ~VoronoiAreaTriMeshT() {

		if(obtuse_handling_) {
			const_cast<MeshT&>(mesh_).remove_property(obtuse_tag_);
		}

		const_cast<MeshT&>(mesh_).remove_property(cotan_weights_);
	}

	const OpenMesh::EPropHandleT<typename MeshT::Scalar>& cotan_weight_prop() const {
		return cotan_weights_;
	}

	typename MeshT::Scalar get_voronoi_area(const typename MeshT::VertexHandle& _vh) const {

		typename MeshT::Scalar w = 0.0;

		bool obtuse = false;
		if(obtuse_handling_) {
			for(typename MeshT::ConstVertexFaceIter cvf_it = mesh_.cvf_iter(_vh),
							cvf_end = mesh_.cvf_end(_vh); cvf_it != cvf_end; ++cvf_it) {
				if(is_obtuse(*cvf_it)) {
					obtuse = true;
					break;
				}
			}
		}

		for(typename MeshT::ConstVertexOHalfedgeIter cvoh_it = mesh_.cvoh_iter(_vh),
				cvoh_end = mesh_.cvoh_end(_vh); cvoh_it != cvoh_end; ++cvoh_it) {

			if(obtuse) {

				// At least one face in one-ring is obtuse

				const typename MeshT::FaceHandle fh = mesh_.face_handle(*cvoh_it);

				// Voronoi inappropriate
				if(is_obtuse(fh)) {

					w += area(fh) * 0.5;

				} else {

					w += area(fh) * 0.25;
				}

			} else {

				// Appropriate computation of area
				w += voronoi_area(*cvoh_it);
			}
		}

		return w;
	}

private:

	inline typename MeshT::Scalar area(const typename MeshT::FaceHandle& _fh) const {

		typename MeshT::ConstFaceVertexIter cfv_it = mesh_.cfv_iter(_fh);

		const typename MeshT::Point& p0 = mesh_.point(*cfv_it); ++cfv_it;
		const typename MeshT::Point& p1 = mesh_.point(*cfv_it); ++cfv_it;
		const typename MeshT::Point& p2 = mesh_.point(*cfv_it);

		return ((p1 - p0) % (p2 - p0)).norm() * 0.5;
	}

	inline typename MeshT::Scalar voronoi_area(const typename MeshT::HalfedgeHandle& _heh) const {

		const typename MeshT::Normal e = mesh_.point(mesh_.to_vertex_handle(_heh)) -
				mesh_.point(mesh_.from_vertex_handle(_heh));

		return 1.0/8.0 * mesh_.property(cotan_weights_, mesh_.edge_handle(_heh)) * (e | e);
	}

	void tag_obtuse_triangles() {

		for(typename MeshT::ConstFaceIter f_it = mesh_.faces_begin(),
						f_end = mesh_.faces_end(); f_it != f_end; ++f_it) {

			typename MeshT::ConstFaceVertexIter cfv_it = mesh_.cfv_iter(*f_it);

			const typename MeshT::Point& p0 = mesh_.point(*cfv_it); ++cfv_it;
			const typename MeshT::Point& p1 = mesh_.point(*cfv_it); ++cfv_it;
			const typename MeshT::Point& p2 = mesh_.point(*cfv_it);

			const_cast<MeshT&>(mesh_).property(obtuse_tag_, *f_it) = (
					(((p1 - p0)|(p2 - p0)) < 0) ||
					(((p2 - p1)|(p0 - p1)) < 0) ||
					(((p0 - p2)|(p1 - p0)) < 0));
		}
	}

	// Compute cotan weights
	void compute_cotan_weights() const {

		for(typename MeshT::ConstEdgeIter e_it = mesh_.edges_begin(),
				e_end = mesh_.edges_end(); e_it != e_end; ++e_it) {

			const typename MeshT::Scalar w = 0.5 * (cotan_weight(mesh_.halfedge_handle(*e_it, 0)) +
					cotan_weight(mesh_.halfedge_handle(*e_it, 1)));

			const_cast<MeshT&>(mesh_).property(cotan_weights_, *e_it) = w;
		}
	}

	typename MeshT::Scalar cotan_weight(const typename MeshT::HalfedgeHandle& _he) const {

		/*
		 * Compute Voronoi area for triangle incident to _he
		 */
		const typename MeshT::Normal e0 = (mesh_.point(mesh_.to_vertex_handle(_he)) -
				mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(_he)))).normalized();

		const typename MeshT::Normal e1 = (mesh_.point(mesh_.from_vertex_handle(_he)) -
						mesh_.point(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(_he)))).normalized();

		const typename MeshT::Scalar cos_a = (e0 | e1);
		const typename MeshT::Scalar sin_a = (e0 % e1).norm();

		return cos_a / sin_a;
	}

	inline bool is_obtuse(const typename MeshT::FaceHandle& _fh) const {
		return mesh_.property(obtuse_tag_, _fh);
	}

	const MeshT& mesh_;
	const bool obtuse_handling_;

	OpenMesh::FPropHandleT<bool> obtuse_tag_;

	OpenMesh::EPropHandleT<typename MeshT::Scalar> cotan_weights_;
};

#endif /* VORONOIAREATRIMESHT_HPP_ */
