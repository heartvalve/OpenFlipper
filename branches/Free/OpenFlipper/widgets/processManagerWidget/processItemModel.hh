/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 6747 $                                                       *
 *   $Author: kremer $                                                       *
 *   $Date: 2009-08-05 11:36:43 +0200 (Wed, 05 Aug 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef PROCESSITEMMODEL_H
#define PROCESSITEMMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "ProcessItem.hh"

class ProcessItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /// Constructor
    ProcessItemModel ( QObject *_parent = 0 );

    /// Destructor
    ~ProcessItemModel();

//===========================================================================
    /** @name inherited from QAbstractItemModel
      * @{ */
//===========================================================================

public:

    /// Get the data of the corresponding entry
    QVariant data ( const QModelIndex &index, int role ) const;

    /// return the types of the corresponding entry
    Qt::ItemFlags flags ( const QModelIndex &index ) const;

    /// return the header data of the model
    QVariant headerData ( int section,
                          Qt::Orientation orientation,
                          int role = Qt::DisplayRole ) const;

    /// Get the ModelIndex at given row,column
    QModelIndex index ( int row, int column,
                        const QModelIndex &parent = QModelIndex() ) const;

    /// Get the parent ModelIndex
    QModelIndex parent ( const QModelIndex &index ) const;

    /// get the number of rows
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /// get the number of columns
    int columnCount ( const QModelIndex &parent = QModelIndex() ) const;

    /// Set the data at the given index
    bool setData ( const QModelIndex &index, const QVariant &value , int role );

    /** @} */

//===========================================================================
    /** @name Internal DataStructure (the ProcessItem)
      * @{ */
//===========================================================================

public:

    /// Return the ModelIndex corresponding to a given ProcessItem and Column
    QModelIndex getModelIndex ( ProcessItem* _object, int _column );

    /// Check if the given item is the root item
    bool isRoot ( ProcessItem* _item );

    /// Get the ProcessItem corresponding to a given ModelIndex
    ProcessItem *getItem ( const QModelIndex &index ) const;

    /// Get the name of a ProcessItem corresponding to a given ModelIndex
    QString itemName ( const QModelIndex &index ) const;

    /// Get the id of a ProcessItem corresponding to a given ModelIndex
    int itemId ( const QModelIndex &index ) const;

private:

    /// Rootitem of the list
    ProcessItem* rootItem_;

    /** @} */

};

#endif
