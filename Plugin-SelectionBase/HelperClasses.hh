/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#ifndef HELPERCLASSES_HH_INCLUDED
#define HELPERCLASSES_HH_INCLUDED

#include <OpenFlipper/common/DataTypes.hh>

#include <QAction>
#include <QPushButton>
#include <QGridLayout>

class HandleAction : public QAction {
    
    public:
        /// Default constructor
        HandleAction(QIcon _icon, QString _description, QObject* _parent = 0, DataType _objectTypeRestriction = DATA_ALL) :
            QAction(_icon, _description, _parent),
            associatedTypes_(0u),
            objectTypeRestriction_(_objectTypeRestriction) {}
       
        /// Default destructor
        ~HandleAction() {}
        
        /// Get/Set selection environment handle name
        void selectionEnvironmentHandle(QString _handle) { selectionEnvHandle_ = _handle; }
        QString selectionEnvironmentHandle() { return selectionEnvHandle_; }
        
        /// Get/Set selection mode handle name
        void selectionModeHandle(QString _handle) { selectionModeHandle_ = _handle; }
        QString selectionModeHandle() { return selectionModeHandle_; }
        
        /// Get/Set associated primitive types
        void addAssociatedType(unsigned int _associatedType) { associatedTypes_ |= _associatedType; }
        void removeAssociatedType(unsigned int _associatedType) { associatedTypes_ &= ~_associatedType; }
        bool isAssociated(unsigned int _type, bool _associatedWithAll = false) {

            if(_associatedWithAll && _type != 0u)
                return ((associatedTypes_ | _type) == associatedTypes_);
            else
                return ((associatedTypes_ & _type) != 0);
        }
        
        void objectTypeRestriction(const DataType& _type) {
            objectTypeRestriction_ = _type;
        }

        const DataType& objectTypeRestriction() const {
            return objectTypeRestriction_;
        }

    private:
        QString selectionEnvHandle_;
        QString selectionModeHandle_;
        
        unsigned int associatedTypes_;

        DataType objectTypeRestriction_;
};

class PrimitiveAction : public QAction {
    
    public:
        /// Default constructor
        PrimitiveAction(QIcon _icon, QString _description, QObject* _parent = 0) :
            QAction(_icon, _description, _parent),
            primitiveType_(0),
            selectionEnvironmentHandle_("")
        {};
       
        /// Default destructor
        ~PrimitiveAction() {};
        
        /// Get/Set primitive type
        void primitiveType(unsigned int _type) { primitiveType_ = _type; }
        unsigned int primitiveType() { return primitiveType_; }
        
        /// Get/Set associated selection environment handle
        void selectionEnvironmentHandle(const QString _handle) {
            selectionEnvironmentHandle_ = _handle;
        }
        QString selectionEnvironmentHandle() const {
            return selectionEnvironmentHandle_;
        }
        
    private:
        unsigned int primitiveType_;
        QString selectionEnvironmentHandle_;
};

class ActionButton : public QPushButton {
    Q_OBJECT
    
    public:
        /// Default constructor
        ActionButton(QAction* _action, QWidget* _parent = 0) :
            QPushButton(_parent), action_(_action) {
            // Initialize button with action parameters
            setCheckable(true);
            setIcon(action_->icon());
            setIconSize(QSize(48,48));
            setToolTip(action_->text());
            
            connect(action_, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
            connect(this, SIGNAL(toggled(bool)), action_, SLOT(setChecked(bool)));
        };
        
        /// Default destructor
        ~ActionButton() {};

    private slots:

        // Avoid back sending of signal (this results in an infinite loop)
        void setChecked(bool _checked) {
            blockSignals(true);
            QPushButton::setChecked(_checked);
            blockSignals(false);
        }

        void changeRegistered() {
            setChecked(action_->isChecked());
        }
        
    private:
        QAction* action_;
};

class FillingLayout : public QGridLayout {
    public:
        /// Default Constructor
        FillingLayout(int _numColumns) :
            QGridLayout(),
            currentRow_(0),
            currentColumn_(0),
            numColumns_(_numColumns) {};
            
        void addWidget(QWidget* _widget) {
            
            QGridLayout::addWidget(_widget, currentRow_, currentColumn_);
            
            currentColumn_++;
            if(currentColumn_ == numColumns_) {
                currentColumn_ = 0;
                currentRow_++;
            }
        };
        
    private:
        unsigned int currentRow_;
        unsigned int currentColumn_;
        unsigned int numColumns_;
};

#endif // HELPERCLASSES_HH_INCLUDED
