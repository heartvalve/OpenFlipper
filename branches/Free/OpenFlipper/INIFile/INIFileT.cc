/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS INIFile Templates - IMPLEMENTATION
//
//=============================================================================

#define INIFILE_C

//== INCLUDES =================================================================

#include "INIFile.hh"

//std include
#include <fstream>
#include <iostream>
#include <functional>
#include <sstream>
#include <cstring>
#include <cctype>
//#include <ios>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringList>

//== IMPLEMENTATION ==========================================================

//! Get a Vec_n_i
template < typename VectorT >
bool
INIFile::
get_entryVeci ( VectorT & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of requested vector
  VectorT tmp;
  int dim = tmp.dim();

  if ( list.size() != dim ) {
    std::cerr << "Differet size when reading Vector" << std::endl;
    return false;
  }

  bool ok = true;
  for ( int i = 0 ; i < dim; ++i) {
    bool tmpOk = false;
    _val[i] = (typename VectorT::value_type) list[i].toInt(&tmpOk);
    ok &= tmpOk;
  }

  return ok;
}

// -----------------------------------------------------------------------------

//! Get a Vec_n_d
template < typename VectorT >
bool
INIFile::
get_entryVecd ( VectorT & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of requested vector
  VectorT tmp;
  int dim = tmp.dim();

  if ( list.size() != dim ) {
    std::cerr << "Differet size when reading Vector" << std::endl;
    return false;
  }

  bool ok = true;
  for ( int i = 0 ; i < dim; ++i) {
    bool tmpOk = false;
    _val[i] = (typename VectorT::value_type) list[i].toDouble(&tmpOk);
    ok &= tmpOk;
  }

  return ok;
}

// -----------------------------------------------------------------------------

//! Get a Vec_n_f
template < typename VectorT >
bool
INIFile::
get_entryVecf ( VectorT & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of requested vector
  VectorT tmp;
  int dim = tmp.dim();

  if ( list.size() != dim ) {
    std::cerr << "Differet size when reading Vector" << std::endl;
    return false;
  }

  bool ok = true;
  for ( int i = 0 ; i < dim; ++i) {
    bool tmpOk = false;
    _val[i] = (typename VectorT::value_type) list[i].toFloat(&tmpOk);
    ok &= tmpOk;
  }

  return ok;
}

// -----------------------------------------------------------------------------

//! Addition of a vector of Vec_n_whatever
template < typename VectorT >
void
INIFile::
add_entryVec ( const QString & _section,
               const QString & _key,
               const VectorT  & _value)
{

  // get dimension of stored vectors
  VectorT tmp;
  int dim = tmp.dim();

  QString list;
  for (int j = 0; j < dim; ++j)
    list +=  QString::number( _value[j] ) + ";";

  m_iniData[ _section ][ _key ] = list;
}

// -----------------------------------------------------------------------------

template < typename VectorT >
bool
INIFile::
get_entryVecd ( std::vector< VectorT > & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  _val.clear();

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of stored vectors
  VectorT tmp;
  int dim = tmp.dim();

  bool ok = true;
  for ( int i = 0 ; i < list.size(); )
  {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;

    std::vector<double> tmp;

    for (int j = 0; j < dim; ++j)
    {
      // check data type
      tmp.push_back( list[i].toDouble(&tmpOk) );
      ++i;
    }

    VectorT vec;
    for (int j = 0; j < dim; ++j)
      vec[j] = (typename VectorT::value_type)(tmp[j]);

    _val.push_back(vec);
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------

template < typename VectorT >
bool
INIFile::
get_entryVecf ( std::vector< VectorT > & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  _val.clear();

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of stored vectors
  VectorT tmp;
  int dim = tmp.dim();

  bool ok = true;
  for ( int i = 0 ; i < list.size(); )
  {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;

    std::vector<double> tmp;

    for (int j = 0; j < dim; ++j)
    {
      // check data type
      tmp.push_back( list[i].toFloat(&tmpOk) );
      ++i;
    }

    VectorT vec;
    for (int j = 0; j < dim; ++j)
      vec[j] = (typename VectorT::value_type)(tmp[j]);

    _val.push_back(vec);
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------

template < typename VectorT >
bool
INIFile::
get_entryVeci ( std::vector< VectorT > & _val ,
                const QString & _section,
                const QString & _key ) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  _val.clear();

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  QStringList list = eIter->second.split(';',QString::SkipEmptyParts);

  // get dimension of stored vectors
  VectorT tmp;
  int dim = tmp.dim();

  bool ok = true;
  for ( int i = 0 ; i < list.size(); )
  {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;

    std::vector<double> tmp;

    for (int j = 0; j < dim; ++j)
    {
      // check data type
      tmp.push_back( list[i].toInt(&tmpOk) );
      ++i;
    }

    VectorT vec;
    for (int j = 0; j < dim; ++j)
      vec[j] = (typename VectorT::value_type)(tmp[j]);

    _val.push_back(vec);
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------


template < typename VectorT >
void
INIFile::
add_entryVec ( const QString & _section,
               const QString & _key,
               const std::vector< VectorT > & _value)
{
  QString list;
  typename std::vector< VectorT >::const_iterator viter;

  VectorT tmp;

  for(viter = _value.begin();viter!=_value.end();++viter)
  {
    for (int i = 0; i < tmp.dim(); ++i)
      list += QString::number( (*viter)[i] ) + ";";
  }

  m_iniData[ _section ][ _key ] = list;
}
