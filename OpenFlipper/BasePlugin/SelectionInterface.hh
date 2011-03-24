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


#ifndef SELECTIONINTERFACE_HH
#define SELECTIONINTERFACE_HH

#include <OpenFlipper/common/Types.hh>
#include <QtGui>

/** \file SelectionInterface.hh
*
*  Interface for providing selection functionality for specific data types.\ref selectionInterfacePage
*/


/** \brief Interface for all plugins which want to use selection functions
  *
  * \ref selectionInterfacePage "Detailed description"
  * \n
  *
  * Using this interface you can instruct the core to select objects
  * and different primitive types.
  *
  * \nosubgrouping
  */
class SelectionInterface {

  public:

    typedef std::vector<DataType> TypeList;
    typedef unsigned int          PrimitiveType;

    /// Destructor
    virtual ~SelectionInterface() {};

    //===========================================================================
    /** @name Registering Selection Environments and primitives
     *
     * These functions are used to register your selection environment to
     * the selection system. The environment is basically a collection of selection
     * metaphors that will be available for a set of DataTypes.
     *
     * You Register the Environment via addSelectionEnvironment(). Afterwards you can
     * register DataTypes to the previously created environment with registerType().
     * The selection interactions defined in the environment will only be available,
     * if the registered DataTypes exist.
     *
     * Each DataType can consist of different primitives (e.g. a mesh usually consists
     * of vertices edges and faces). Use addPrimitiveType() to register these
     * primitives and remember the handles you got.
     * @{ */
    //===========================================================================

    signals:

    /** \brief Add a selection environment in order to provide selection functions for specific data type(s)
     *
     *  This adds a toolbar button and initializes a new selection environment that supports
     *  all objects/entities of a specified data type. The type for which the selection
     *  applies is later registered via the signal registerType(QString,DataType).
     *  Use the returned _handleName for referencing the selection environment.
     *
     *  @param _modeName    The name of the selection mode, e.g. "Mesh Selection"
     *  @param _description The description for the selection mode (also tooltip)
     *  @param _icon        The icon for this mode (basically a type icon)
     *  @param _handleName  The handle of the recently added selection environment. Needed for referencing
     */
    virtual void addSelectionEnvironment(QString _modeName, QString _description, QIcon _icon, QString& _handleName) {};

    /** \brief Register data type for a selection environment
     *
     *  After adding a new selection environment, one can register types which the
     *  selection environment accounts for. For example, if we added a selection environment
     *  for polylines, we would have to call registerType(environmentHandle,DATA_POLY_LINE)
     *  in order to enable polyline selections.
     *
     *  @param _handleName  The handle of the selection environment for this type
     *  @param _type        The data type that should be added
     */
    virtual void registerType(QString _handleName, DataType _type) {};

    /** \brief Provide selection for primitives other than the standard ones
     *
     *  Use this signal to add a new primitive type that can be selected.
     *  This returns a handle to the newly added primitive type which will be
     *  of use later on.
     *
     *  @param _handleName  The handle of the selection environment to which this type should be added
     *  @param _name        The name of the primitive type, e.g. "B-Spline Surface Control Point"
     *  @param _icon        The icon for the primitive type
     *  @param _typeHandle  The returned handle to the added primitive type
     */
    virtual void addPrimitiveType(QString _handleName, QString _name, QIcon _icon, PrimitiveType& _typeHandle) {};

    /** @} */


    //===========================================================================
    /** @name  Enabling Selection Metaphors
     * \anchor SelectionInterface_enable_metaphors_group
     * The selection system provides a set of basic selection operations.
     * These include toggle selection, volume lasso, surface lasso, painting,
     * closest boundary and flood fill selection.
     *
     * The interactions for these metaphors are controlled by the Base Selection
     * Plugin. The results of these interactions are than passed on to the
     * specific selection plugins.
     *
     * These functions are used to tell the system, that you can handle the
     * given selection metaphors in your plugin.
     *
     * You can also add your custom seletion mode via addCustomSelectionMode().
     * @{ */
    //===========================================================================

    signals:

    /** \brief Show toggle selection mode in a specified selection environment
     *
     *  Show or hide the toggle selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showToggleSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show lasso selection mode in a specified selection environment
     *
     *  Show or hide the lasso selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show volume lasso selection mode in a specified selection environment
     *
     *  Show or hide the volume lasso selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showVolumeLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show surface lasso selection mode in a specified selection environment
     *
     *  Show or hide the surface lasso selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showSurfaceLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show sphere selection mode in a specified selection environment
     *
     *  Show or hide the sphere selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showSphereSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show closest boundary selection mode in a specified selection environment
     *
     *  Show or hide the closest boundary selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showClosestBoundarySelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Show flood fill selection mode in a specified selection environment
     *
     *  Show or hide the flood fill selection mode for the specified selection environment.
     *  Note that per default no interactive selection mode will be available.
     *  One will always have to explicitly add the required selection modes for
     *  each selection environment.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _show            Indicates whether the mode should be available or not
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     */
    virtual void showFloodFillSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

    /** \brief Add a custom interactive selection mode
     *
     *  If a plugin should provide an interactive selection mode other than the
     *  standard ones (which include toggle, lasso, volume lasso, sphere, closest boundary
     *  and flood fill selection), one can add a custom interactive selection mode
     *  via this signal. Once the custom mode is added, it will appear in the sub-menu
     *  for the associated selection environment. If the user chooses this mode in order
     *  to do selection, slotCustomSelection(QMouseEvent*,QString) is called.
     *  This signal returns the added identifier for this selection mode in parameter
     *  _identifier.
     *
     *  @param _handleName      The handle of the selection environment in which this mode should be available
     *  @param _modeName        The name of this mode (also button caption)
     *  @param _description     A brief description of what the selection mode does
     *  @param _icon            An icon which is used for this selection mode
     *  @param _associatedTypes Make this mode available only for the specified types (OR'ed)
     *  @param _customIdentifier Holds the identifier of the custom selection modes
     */
    virtual void addCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QIcon _icon,
                                        PrimitiveType _associatedTypes, QString& _customIdentifier) {};

    /** @} */



    //===========================================================================
     /** @name States of the selection system.
      *
      * You can get the active data types,
      * primitive types and if only target objects should be affected.
      *
      * @{ */
    //===========================================================================


    /** \brief Get the data types that the currently active selection environment supports
     *
     *  This fetches a list of data types that are supported by the currently activated
     *  selection environment.
     *
     *  @param _types The list of currently active data types
     */
    virtual void getActiveDataTypes(TypeList& _types) {};

    /** \brief Get the primitive type that is selected
     *
     *  This returns the id of the primitive type that is selected.
     *
     *  @param _type Id of selected primitive type
     */
    virtual void getActivePrimitiveType(PrimitiveType& _type) {};

    /** \brief Indicates whether selection should be performed on target objects only
     *
     *  Self-explanatory
     *
     *  @param _targetsOnly True if selection should restrict to target objects
     */
    virtual void targetObjectsOnly(bool& _targetsOnly) {};

    /** @} */

    //===========================================================================
     /** @name File Interaction
      *
      * @{ */
    //===========================================================================

    private slots:

      virtual void slotLoadSelection(const INIFile& _file) {};
      virtual void slotSaveSelection(INIFile& _file) {};

    /** @} */

    //===========================================================================
    /** @name Keyboard Interactions
     *
     * @{ */
    //===========================================================================

    signals:
    /** \brief Register key shortcut
     *
     *  This signal is emitted whenever a type selection plugin wants
     *  to provide key shortcuts for its functions. Note that multiple
     *  registration of one key will be possible. Key events will be passed
     *  to ALL type selection plugins that want to listen to key events of this
     *  key, thus, one has to ignore the event if the currently active primitive
     *  type (request via signal getActivePrimitiveType(PrimitiveType&) ) is
     *  not handled by the type selection plugin.
     *
     *  @param _key       Key to register
     *  @param _modifiers Key modifiers (i.e. shift, control, meta keys, defaults to none)
     */
    virtual void registerKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier) {};


    private slots:

      virtual void slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {};

    /** @} */

    //===========================================================================
    /** @name User Interface controls
     *
     * @{ */
    //===========================================================================

    signals:
    /** \brief Add non-interactive selection operations for a specific primitive type
     *
     *  One can add non-interactive selection operations for each primitive type
     *  that will appear as buttons in the selection base toolbar.
     *  An example for this would be "Clear selection", "Invert", "Delete", etc.
     *  If one of this operations is requested, slotSelectionOperation(QString) is
     *  called. These functions are listed in categories which are mainly used
     *  to improve usability.
     *
     *  @param _handleName      The handle of the selection environment in which this operation should be available
     *  @param _operationsList  The list of operations that can be performed for a given primitive type
     *  @param _category        The category under which the specified operations will be listed
     *  @param _type            The primitive type for which the specified operations will be available (0u if operation should always be available)
     */
    virtual void addSelectionOperations(QString _handleName, QStringList _operationsList, QString _category, PrimitiveType _type = 0u) {};

    private slots:

    virtual void slotSelectionOperation(QString _operation) {};


    /** @} */

   //===========================================================================
    /** @name Slots which have to be implemented to use the global interaction metaphors
     * If you enabled selection metaphors for your environment via the
     * \ref SelectionInterface_enable_metaphors_group "control functions" you will
     * have to implement the corresponding slots here. These will be
     * called when the events occur.
     *
     * @{ */
   //===========================================================================

   private slots:

   /** \brief Called whenever the user performs a toggle selection
    *
    *  This has to be implemented by each type selection plugin if this interactive
    *  selection mode should be provided.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotToggleSelection(QPoint _position, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a lasso selection
    *
    *  This has to be implemented by each type selection plugin if this
    *  interactive selection mode should be provided. The metaphor behind it is
    *  that the user draws a polygonal line and all primitives which are <b>visible</b>
    *  get selected.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a volume lasso selection
    *
    *  This has to be implemented by each type selection plugin if this interactive
    *  selection mode should be provided. The metaphor behind it is
    *  that the user draws a polygonal line and all primitives which are insinde the
    *  volume spanned by the eye position and the polygon get selected.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotVolumeLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a surface lasso selection
    *
    *  This has to be implemented by each type selection plugin if this
    *  interactive selection mode should be provided. The metaphor behind it is
    *  that the user draws a polygonal line on the surface of an object and
    *  all primitives which are insinde the surface patch defined
    *  by the line get selected.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotSurfaceLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a sphere selection
    *
    *  This has to be implemented by each type selection plugin if
    *  this interactive selection mode should be provided. The metaphor behind
    *  it is that the user draws the selection with a sphere. All primitives inside
    *  the current sphere get selected.
    *
    *  @param _position            The 2-D position of the mouse cursor
    *  @param _radius              The current radius of the selection sphere
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotSphereSelection(QPoint _position, double _radius, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a closest boundary selection
    *
    *  This has to be implemented by each type selection plugin if
    *  this interactive selection mode should be provided.  The metaphor behind
    *  it is that the user clicks on an object and the primitives at the closest boundary
    *  get selected.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotClosestBoundarySelection(QPoint _position, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a flood fill selection
    *
    *  This has to be implemented by each type selection plugin if this interactive selection mode should be provided.
    *  The metaphor behind it is that the user clicks on an object and all primitives close to this point
    *  get selected, if the angle between the clicked point and the next point does not differ more than
    *  the specified angle.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _maxAngle            The maximum angle used for flood filling
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotFloodFillSelection(QPoint _position, double _maxAngle, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Called whenever the user performs a custom selection
    *
    *  This has to be implemented by each type selection plugin if this
    *  interactive selection mode should be provided.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void slotCustomSelection(QMouseEvent* _event, PrimitiveType _currentType, QString _customIdentifier, bool _deselect) {};


   /** @} */



   //===========================================================================
    /** @name Signals emitted by the SelectionBasePlugin
     *
     * The following signals are emitted by the selection base plugin.
     * <b>You should not use them!</b>
     * @{ */
   //===========================================================================

   signals:

   /** \brief Emitted by selection base plugin when a non-interactive selection operation is requested
    *
    *  This connects to slot slotSelectionOperation and is called whenever a non-interactive
    *  selection operation is requested.
    *
    *  @param _operation The name of the requested operation
    */
   virtual void selectionOperation(QString _operation) {};


   /** \brief Emitted by selection base plugin whenever the user performs a toggle selection
    *
    *  This connects to slotToggleSelection(QPoint,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void toggleSelection(QPoint _position, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a lasso selection
    *
    *  This connects to slotLassoSelection(QMouseEvent*,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void lassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a volume lasso selection
    *
    *  This connects to slotVolumeLassoSelection(QMouseEvent*,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void volumeLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a surface lasso selection
    *
    *  This connects to slotSurfaceLassoSelection(QMouseEvent*,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void surfaceLassoSelection(QMouseEvent* _event, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a sphere selection
    *
    *  This connects to slotSphereSelection(QPoint,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _position            The 2-D position of the mouse cursor
    *  @param _radius              The current radius of the selection sphere
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void sphereSelection(QPoint _position, double _radius, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a closest boundary selection
    *
    *  This connects to slotClosestBoundarySelection(QPoint,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void closestBoundarySelection(QPoint _position, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a flood fill selection
    *
    *  This connects to slotFloodFillSelection(QPoint,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _position            The 2-D position of the mouse cursor in local screen coordinates
    *  @param _maxAngle            The maximum angle used for flood filling
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void floodFillSelection(QPoint _position, double _maxAngle, PrimitiveType _currentType, bool _deselect) {};

   /** \brief Emitted by selection base plugin whenever the user performs a custom selection
    *
    *  This connects to slotCustomSelection(QMouseEvent*,QString) which has to be implemented
    *  by each type selection plugin if this interactive selection mode should be provided.
    *
    *  @param _event               The mouse event that currently is performed
    *  @param _currentType         The currently active primitive type
    *  @param _deselect            True if entities should be deselected
    */
   virtual void customSelection(QMouseEvent* _event, PrimitiveType _currentType, QString _customIdentifier, bool _deselect) {};

   /** \brief Load selections from ini-file
    *
    *  This signal is emitted by SelectionBase-Plugin each time a selection
    *  should be read from a file. This ini-file handle is then passed
    *  to each object type selection plugin where object specific
    *  selections can be extracted.
    *
    *  @param _file The ini-file handle
    */
   virtual void loadSelection(const INIFile& _file) {};

   /** \brief Save selections into ini-file
    *
    *  This signal is emitted by SelectionBase-Plugin each time a selection
    *  should be written into a file. This ini-file handle is then passed
    *  to each object type selection plugin where object specific
    *  selections can be saved.
    *
    *  @param _file The ini-file handle
    */
   virtual void saveSelection(INIFile& _file) {};

   /** \brief Key shortcut event happened
    *
    *  Emitted by selection base plugin whenever a registered key shortcut has been pressed.
    *
    *  @param _key       Key to register
    *  @param _modifiers Key modifiers (i.e. shift, control, meta keys, defaults to none)
    */
   virtual void keyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers = Qt::NoModifier) {};


   /** @} */

   //===========================================================================
   /** @name Slots handled by the SelectionBasePlugin
    *
    * The following slots are implemented by the selection base plugin.
    * <b>You should not use them!</b>
    * @{ */
  //===========================================================================

   private slots:

   virtual void slotAddSelectionEnvironment(QString _modeName, QString _description, QIcon _icon, QString& _handleName) {};
   virtual void slotRegisterType(QString _handleName, DataType _type) {};
   virtual void slotAddPrimitiveType(QString _handleName, QString _name, QIcon _icon, PrimitiveType& _typeHandle) {};
   virtual void slotAddCustomSelectionMode(QString _handleName, QString _modeName, QString _description, QIcon _icon,
                                           PrimitiveType _associatedTypes, QString& _customIdentifier) {};


   virtual void slotShowToggleSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowVolumeLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowSurfaceLassoSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowSphereSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowClosestBoundarySelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};
   virtual void slotShowFloodFillSelectionMode(QString _handleName, bool _show, PrimitiveType _associatedTypes) {};

   virtual void slotRegisterKeyShortcut(int _key, Qt::KeyboardModifiers _modifiers) {};

   virtual void slotGetActiveDataTypes(TypeList& _types) {};
   virtual void slotGetActivePrimitiveType(PrimitiveType& _type) {};

   virtual void slotTargetObjectsOnly(bool& _targetsOnly) {};

   virtual void slotAddSelectionOperations(QString _handleName, QStringList _operationsList, QString _category, PrimitiveType _type) {};


   /** @} */


};

/** \page selectionInterfacePage Selection Interface
\image html SelectionInterface.png

The SelectionInterface can be used to provide selection functions. Selection works
as follows. OpenFlipper basically contains the SelectionBasePlugin which offers
some graphical tools (such as selection sphere, lasso tools, etc.) that can be used for selecting entities.

Now each object type (poly/triangle meshes, skeletons, splines, whatever) has to implement
its own selection functionality. Say we wanted to implement selection for poly/tri mesh objects.
For this purpose we derive a plugin from SelectionInterface and create a new selection environment
via the addSelectionEnvironment() signal, naming it e.g. "Mesh Object Selection".

After having created the necessary selection environment, we have to register our specific data type,
that is tell the SelectionBase plugin which data types will be handled (selected) by our plugin.
In the example from above we would register DATA_POLY_MESH as well as DATA_TRIANGLE_MESH.
We do this by calling the signal registerType(). Now we have to determine which primitive type
should be selectable. For poly/tri meshes we would determine vertices, edges, halfedges and faces
to be selectable. This is performed via emission of the signal addPrimitiveType().

We can then choose which of the default selection tools should be available for this environment.
We could, for example, request the toggle, flood fill and the sphere selection tool by calling
the signals showToggleSelectionMode(), and so on. Additionally, it's possible to create
custom selection tools that can be completely different from the default ones.

After having requested the desired selection tools, each time a selection tool has been used,
the appropriate slot is called for further processing, that is the actual selection of the
primitives.

SelectionInterface also provides means to add operations for a specific primitive type
(such as, for example, "Clear Vertex Selection" or "Select All Vertices" for vertex selections).
We can tell SelectionBase to add there operations by emitting addSelectionOperations().
Each time an operation button has been clicked, slot slotSelectionOperation() is called.

The overall idea is to separate the UI and control elements from actual implementations
of selection functionality for a specific data type. So, in general SelectionBase manages
all UI control elements and and the rendering of the selection tools whereas the specific
type selection plugins manage the real selection of their entities as well as scripting functions.
*/

Q_DECLARE_INTERFACE(SelectionInterface,"OpenFlipper.SelectionInterface/1.0")

#endif // SELECTIONINTERFACE_HH
