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

// -----------------------------------------------------------------------------

INIFile::INIFile()
{
  mf_isConnected = false;
}


// -----------------------------------------------------------------------------

INIFile::~INIFile()
{
  disconnect();
}


// -----------------------------------------------------------------------------

bool INIFile::connect( const QString& _filename,
		       const bool _create )
{
  QFile inputFile( _filename );

  if( is_connected() )
    disconnect();

  // open given file for reading
  if ( inputFile.exists() &&  inputFile.open(QIODevice::ReadOnly | QIODevice::Text) )  {

    // Successfull?
    m_filename = _filename;

    // try to parse the INI file
    mf_isConnected = parseFile( inputFile );

    inputFile.close();

  } else if( _create ) {

    // file does not exist yet, but user wants to create it.
    // therefore: try to create file
    QFile outputFile(_filename);

    if ( outputFile.open( QIODevice::Text |
                          QIODevice::WriteOnly |
                          QIODevice::Truncate ) )  {

      mf_isConnected = true;
      m_filename = _filename;
      outputFile.close();

    } else {

      std::cerr << "Unable to create File : " << std::endl;
      mf_isConnected = false;

    }

  } else {
    std::cerr << "Unable to open File : " << std::endl;
    mf_isConnected = false;
  }

  return mf_isConnected;
}



// -----------------------------------------------------------------------------

void INIFile::disconnect()
{
  writeFile();
  mf_isConnected = false;
}


// -----------------------------------------------------------------------------

bool INIFile::parseFile( QFile & _inputStream )
{
  QString line, section;
  bool inSection = false;

  // read file line by line
  while( !_inputStream.atEnd()  ) {

    // get new line
    QByteArray lineBuffer = _inputStream.readLine();
    line = QString(lineBuffer);
    line = line.trimmed();

    if( line.isEmpty() || line[ 0 ] == '#' )
      continue;

    // does line contain the start of a section?
    if( line[ 0 ] == '[' && line[ line.length()-1 ] == ']' ) {

      // yes
      section = line.mid( 1, line.length() - 2 );
      inSection = true;

    } else if( inSection ) {
      // try to split line into a key and a value
      QString key, value;

      int pos;
      pos = line.indexOf( '=' );

      if( pos != -1 )
      {
        key = line.mid( 0, pos );
        key = key.trimmed();
        value = line.mid( pos + 1, line.length() - 1 );
        value = value.trimmed();

        if( key.isEmpty() || value.isEmpty() )
            continue;

        // store value in string-map
        m_iniData[ section ][ key ] = value;
      }

    }
  }

  return true;
}


// -----------------------------------------------------------------------------


bool INIFile::writeFile( void )
{
  if( !mf_isConnected )
      return false;

  // open file for writing
  QFile outputFile(m_filename);

  if ( ! outputFile.open( QIODevice::WriteOnly ) )
    return false;

  QTextStream out(&outputFile);

  // file is open, start writing of data
  SectionMap::const_iterator sIter, sEnd;
  EntryMap::const_iterator eIter, eEnd;

  sEnd = m_iniData.end();
  for( sIter = m_iniData.begin(); sIter != sEnd; ++sIter ) {
    // write name of current section
    out << "[" << sIter->first << "]\n";

    eEnd = sIter->second.end();
    for( eIter = sIter->second.begin(); eIter != eEnd; ++eIter ) {
      out << eIter->first << "=";
      out << eIter->second;
      out << "\n";
    }

    out << "\n\n";
  }

  outputFile.close();

  return true;
}



// -----------------------------------------------------------------------------


bool INIFile::section_exists( const QString & _section ) const
{
  return( m_iniData.find( _section ) != m_iniData.end() );
}


// -----------------------------------------------------------------------------


bool INIFile::entry_exists(const QString & _section, const QString & _key) const
{
  static SectionMap::const_iterator mapIter;

  return( (mapIter = m_iniData.find( _section )) != m_iniData.end()
          && mapIter->second.find( _key ) != mapIter->second.end() );
}


// -----------------------------------------------------------------------------

void INIFile::add_section( const QString & _sectionname )
{
  if( m_iniData.find( _sectionname ) == m_iniData.end() )
      m_iniData[ _sectionname ] = EntryMap();
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key,
			 const QString & _value )
{
  m_iniData[ _section ][ _key ] = _value;
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key,
			 const double & _value)
{
  m_iniData[ _section ][ _key ] = QString::number( _value );
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key,
			 const float & _value)
{
  m_iniData[ _section ][ _key ] = QString::number( _value );
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key ,
			 const int & _value)
{
  m_iniData[ _section ][ _key ] = QString::number( _value );
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key ,
			 const unsigned int & _value)
{
  m_iniData[ _section ][ _key ] = QString::number( _value );
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key ,
			 const bool & _value)
{
  m_iniData[ _section ][ _key ] = (_value ? "true" : "false");
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
			 const QString & _key,
			 const std::vector<float> & _value)
{
  QString list;
  std::vector<float>::const_iterator viter;
  for(viter = _value.begin();viter!=_value.end();++viter)
    list += QString::number( *viter ) + ";";
  m_iniData[ _section ][ _key ] = list;
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
                         const QString & _key,
                         const std::vector<double> & _value)
{
  QString list;
  std::vector<double>::const_iterator viter;
  for(viter = _value.begin();viter!=_value.end();++viter)
    list += QString::number( *viter ) + ";";
  m_iniData[ _section ][ _key ] = list;
}


// -----------------------------------------------------------------------------


void INIFile::add_entry( const QString & _section,
                         const QString & _key,
                         const std::vector<bool> & _value)
{
  QString list;
  std::vector<bool>::const_iterator viter;
  for(viter = _value.begin();viter!=_value.end();++viter){
    if (*viter == true)
      list += "true;";
    else
      list += "false;";
  }
  m_iniData[ _section ][ _key ] = list;
}


// -----------------------------------------------------------------------------

void INIFile::add_entry( const QString & _section,
          const QString & _key,
          const std::vector<int> & _value)
{
  QString list;
  std::vector<int>::const_iterator viter;
  for(viter = _value.begin();viter!=_value.end();++viter)
    list += QString::number( *viter ) + ";";
  m_iniData[ _section ][ _key ] = list;
}

// -----------------------------------------------------------------------------

void INIFile::add_entry( const QString & _section,
          const QString & _key,
          const std::vector<QString> & _value)
{
  QString list;
  std::vector<QString>::const_iterator viter;
  for(viter = _value.begin();viter!=_value.end();++viter) {
    list += *viter + ";";
  }
  m_iniData[ _section ][ _key ] = list;
}

// -----------------------------------------------------------------------------

void INIFile::add_entry( const QString & _section,
          const QString & _key,
          const QStringList & _value)
{
  QString list = _value.join(";");
  m_iniData[ _section ][ _key ] = list;
}


// -----------------------------------------------------------------------------

void INIFile::delete_entry( const QString & _section, const QString & _key )
{
  SectionMap::iterator sIter;
  EntryMap::iterator eIter;

  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
      return;

  if( (eIter = sIter->second.find( _key )) != sIter->second.end() )
      sIter->second.erase( eIter );
}


// -----------------------------------------------------------------------------

void INIFile::delete_section( const QString & _sectionname )
{
  m_iniData.erase( _sectionname );
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( QString & _val,
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

  _val = eIter->second;
  return true;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( double & _val,
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

  bool ok;
  _val = eIter->second.toDouble(&ok);
  return( ok );
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( float & _val,
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

  bool ok;
  _val = eIter->second.toFloat(&ok);
  return( ok );
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( int & _val,
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

  bool ok;
  _val = eIter->second.toInt(&ok);
  return( ok );
}


// -----------------------------------------------------------------------------

bool INIFile::get_entry( unsigned int & _val,
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

  bool ok;
  _val = eIter->second.toUInt(&ok);
  return( ok );
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( bool & _val,
			 const QString & _section,
			 const QString & _key) const
{
  SectionMap::const_iterator sIter;
  EntryMap::const_iterator eIter;

  // does the given section exist?
  if( (sIter = m_iniData.find( _section )) == m_iniData.end() )
    return false;

  // does the given entry exist?
  if( (eIter = sIter->second.find( _key )) == sIter->second.end() )
    return false;

  if( eIter->second == "true" || eIter->second == "false" ) {
    _val = (eIter->second == "true");
    return true;
  } else {
    return false;
  }
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( std::vector<float> & _val,
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

  QStringList list = eIter->second.split(';');

  bool ok = true;
  for ( int i = 0 ; i < list.size(); ++i) {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;
    _val.push_back(list[i].toFloat(&tmpOk));
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( std::vector<double> & _val,
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

  QStringList list = eIter->second.split(';');

  bool ok = true;
  for ( int i = 0 ; i < list.size(); ++i) {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;
    _val.push_back(list[i].toDouble(&tmpOk));
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( std::vector<bool> & _val,
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

  QStringList list = eIter->second.split(';');

  bool ok = true;
  for ( int i = 0 ; i < list.size(); ++i) {
    if ( list[i].isEmpty() )
      continue;
    if (list[i] == "true")
      _val.push_back(true);
    else
      _val.push_back(false);
  }

  return ok;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( std::vector<int> & _val,
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

  QStringList list = eIter->second.split(';');

  bool ok = true;
  for ( int i = 0 ; i < list.size(); ++i) {
    if ( list[i].isEmpty() )
      continue;
    bool tmpOk = false;
    _val.push_back(list[i].toInt(&tmpOk));
    ok &= tmpOk;
  }

  return ok;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( std::vector<QString> & _val,
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

  QStringList list = eIter->second.split(';');

  bool ok = true;
  for ( int i = 0 ; i < list.size(); ++i) {
    if ( list[i].isEmpty() )
      continue;
    _val.push_back(list[i]);
  }

  return ok;
}


// -----------------------------------------------------------------------------


bool INIFile::get_entry( QStringList & _val,
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

  _val = eIter->second.split(';');

  bool ok = true;
  if ( _val.isEmpty() )
    ok = false;

  return ok;
}


// -----------------------------------------------------------------------------

