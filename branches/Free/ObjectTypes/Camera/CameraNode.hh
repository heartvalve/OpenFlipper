//=============================================================================
//
//  CLASS CameraNode
//
//=============================================================================


#ifndef ACG_CAMERAVIS_NODE_HH
#define ACG_CAMERAVIS_NODE_HH

//== INCLUDES =================================================================


#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <string>
#include <vector>

#include <ACG/Math/QuaternionT.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class CameraNode CameraNode.hh <ACG/Scenegraph/CameraNode.hh>

 This class is able to render spheres colorcoding the vertex CameraVis
 **/

class DLLEXPORT CameraNode: public BaseNode {

public:

    /// Default constructor.
    CameraNode( BaseNode* _parent = 0, std::string _name = "<CameraVis>");

    /// Destructor
    virtual ~CameraNode();

    ACG_CLASSNAME( CameraNode);

    /// return available draw modes
    DrawModes::DrawMode availableDrawModes() const;

    /// update bounding box
    void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);

    /// drawing
    void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

    /// picking
    void pick(GLState& _state, PickTarget _target);

    /// set model view matrix
    void setModelView(ACG::GLMatrixd _modelView) { modelView_ = _modelView; };

    /// Set projection Matrix ( used to calculate frustum ... )
    void setProjection(ACG::GLMatrixd _projection) { projection_ = _projection; };

    /// Set viewport size
    void setSize(int _w, int _h) { width_ = _w; height_ = _h; }

    /// Return encoded view string
    void setEncodedView(QString _encodedView) { encodedView_ = _encodedView; };

    /// Set scene radius
    void setSceneRadius(double _sceneRadius) { sceneRadius_ = _sceneRadius; };

    /// Return encoded view string
    QString getEncodedView() { return encodedView_; };

    /// Set if viewing frustum should be shown or not
    void showFrustum(bool _showFrustum) { showFrustum_ = _showFrustum; }

    /// Returns true if camera renders its whole frustum
    bool showFrustum() { return showFrustum_; }

private:

    void updateBoundingBoxes(GLState& _state, GLMatrixd& _modelview);

    OpenMesh::Vec3f bbmin_;
    OpenMesh::Vec3f bbmax_;

    ACG::GLMatrixd modelView_;
    ACG::GLMatrixd projection_;

    int width_, height_;

    double fovy_;

    double half_height_;
    double half_width_;

    double far_half_height_;
    double far_half_width_;

    double sceneRadius_;
    double aspectRatio_;

    GLUquadric* upQuadric_;
    GLUquadric* rightQuadric_;
    GLUquadric* viewQuadric_;

    QString encodedView_;

    bool showFrustum_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG

//=============================================================================
#endif // ACG_CameraVis_NODE_HH
//=============================================================================
