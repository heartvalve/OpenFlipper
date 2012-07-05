#ifndef QTPLANESELECT_HH
#define QTPLANESELECT_HH

#include <ACG/GL/GLState.hh>
#include <ACG/Math/VectorT.hh>

#include <ACG/Scenegraph/TransformNode.hh>
#include <ACG/Scenegraph/LineNode.hh>
#include <ObjectTypes/Plane/PlaneNode.hh>

#include <qnamespace.h>
#include <qgl.h>
#include <QObject>

#include <list>

class QtPlaneSelect : public QObject
{
    Q_OBJECT

    typedef ACG::Vec2i              Vec2i;
    typedef ACG::Vec2f              Vec2f;
    typedef ACG::Vec3d              Vec3d;

    /*******************************************************************************
             Initialization and deinitialization
     *******************************************************************************/
    public:
        QtPlaneSelect( ACG::GLState& glState );
        ~QtPlaneSelect();

    
    /*******************************************************************************
             Public slots.
     *******************************************************************************/
    public slots:

        /*******************************************************************************
                     Call this to notify the scissor about mouse actions.
         *******************************************************************************/
        void slotMouseEvent(QMouseEvent* _event);

        /*******************************************************************************
                     Call this to notify the scissor about mouse actions.
         *******************************************************************************/
        void slotKeyReleaseEvent(QKeyEvent* _event);

    /*******************************************************************************
             Signals ("callbacks")
     *******************************************************************************/
    signals:
        void signalTriggerCut( );
        void updateViewProxy();


    /*******************************************************************************
            Public accessors.
     *******************************************************************************/
    public:
        Vec3d                getNormal( ) { return normal; }
        Vec3d                getSourcePoint( ) { return sourcePoint3D; }
        unsigned int         getNode() { return nodeIdx_; };
        unsigned int         getTargetIndex() { return targetIdx_; };

    
    /*******************************************************************************
             Members
     *******************************************************************************/
    protected:
        ACG::GLState&       glState;
        Vec3d               sourcePoint3D;
        Vec3d               normal;
        unsigned int        nodeIdx_;
        unsigned int        targetIdx_;
        bool                isDragging;

        PlaneNode*         planeNode_;

    private:
        Plane plane_;
};



#endif // QTPLANESELECT_HH
