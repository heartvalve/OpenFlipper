<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="page">
    <name>main</name>
    <title>Mainpage</title>
    <filename>main</filename>
    <docanchor file="main">DefaultOptions</docanchor>
    <docanchor file="main">Plugincontrols</docanchor>
    <docanchor file="main">intro_sec</docanchor>
    <docanchor file="main">DataStructure</docanchor>
    <docanchor file="main">General</docanchor>
    <docanchor file="main">DataAccess</docanchor>
    <docanchor file="main">Texturecontrol</docanchor>
    <docanchor file="main">Datacontrol</docanchor>
    <docanchor file="main">BasicPlugins</docanchor>
  </compound>
  <compound kind="file">
    <name>PluginFunctions.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/BasePlugin/</path>
    <filename>PluginFunctions_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="no" imported="no">OpenFlipper/common/Types.hh</includes>
    <namespace>PluginFunctions</namespace>
    <class kind="class">PluginFunctions::ObjectIterator</class>
    <member kind="function">
      <type>ObjectIterator</type>
      <name>objects_end</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>438f2fcd4d8a661dfc2d1dc880d86daf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>objectRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bf0ed8a1c2b14f55daf757184ba4688c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5195874eac70f82dc7e8aeb316b1617f</anchor>
      <arglist>(std::vector&lt; TriMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>243b3c32189a1471dda04f7f25c485c3</anchor>
      <arglist>(std::vector&lt; PolyMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b4358af396ca69b2e3029e7f2c46d649</anchor>
      <arglist>(std::vector&lt; TriMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a057a222ce91dd201b545d65cc91f737</anchor>
      <arglist>(std::vector&lt; PolyMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_picked_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ef47f6d2b83b68e6587397646220d91a</anchor>
      <arglist>(uint _node_idx, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b1e3be5d4c7fa2d14535cc0187f1b41</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b045e2632c7378cc2e0150066233b47</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>585008c20534d0ac70ffd428bdbb14ef</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7845369871c9fb0596dbc1c52aaf45c1</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_all_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ca145e68854ad4b3be4fec112737e08c</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_all_object_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c42895597dc095942768517d7aa70cf4</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1091d2449e9d33118bfab6c1e25db63d</anchor>
      <arglist>(int _identifier, BaseObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>3013a288f3318f8718676b06f1921264</anchor>
      <arglist>(int _identifier, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5d62a4329854f98844abb898c042ba3c</anchor>
      <arglist>(int _identifier, TriMeshObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_mesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1172644f56bfb3f5e7ea2221f7501ff9</anchor>
      <arglist>(int _identifier, TriMesh *&amp;_mesh)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_mesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>edb3fc3ac2615a96f6ad631f4d70d892</anchor>
      <arglist>(int _identifier, PolyMesh *&amp;_mesh)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>object_exists</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>dacf0ec50d3ec996ec2edfaf3d95f961</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>object_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7c8f86352f3912f2d11f94e00d9fb2c6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>target_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>45a2037e6dfdbed46d38bb95b22a947c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>source_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>48e21304e2102dbb519d71e644fbb453</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>visible_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>211e3474ca5477b24af33f3cf0afe654</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>86ad67d19dfba2f1e2eabb77b1845435</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::VPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>4e419d2ed41704bc74e90912ad409e9f</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::FPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fcb31fc379e793806451080b35dc5fdf</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::HPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_examiner</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>4aa9ac417f6ae18b0b11e4e70576ed4a</anchor>
      <arglist>(ACG::QtWidgets::QtExaminerViewer *examiner_widget_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_rootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>331db522180614b2e44387ed3f4994e4</anchor>
      <arglist>(SeparatorNode *_root_node)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d14a50aa769da67743500faa66af8687</anchor>
      <arglist>(ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>95bf07301b5f4e367e9d6909acbaf7fd</anchor>
      <arglist>(ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f6afeffa7e67c557c2d7a3c77e894018</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>e859622da656e25418714d1f05dd34c5</anchor>
      <arglist>(std::string _mode)</arglist>
    </member>
    <member kind="function">
      <type>ACG::GLState &amp;</type>
      <name>glState</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>42afb52dcbd4fef6986d372f2f8118f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getCurrentViewImage</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>074bac06a5e27d760728aed1b2b3f0ee</anchor>
      <arglist>(QImage &amp;_image)</arglist>
    </member>
    <member kind="function">
      <type>ACG::SceneGraph::BaseNode *</type>
      <name>getRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>46596260d8ff9904342ff483f9c19591</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>24821d26bcab4ddbc6c13f7c7f11a997</anchor>
      <arglist>(ACG::QtWidgets::QtBaseViewer::ActionMode _mode)</arglist>
    </member>
    <member kind="function">
      <type>ACG::QtWidgets::QtBaseViewer::ActionMode</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>45995def10b964ecb9a43b2f79bd52f4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>allowRotation</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bbc156ec199665bf2ac229741e8ca586</anchor>
      <arglist>(bool _mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewingDirection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>30c72bddf5bcdd6b39abe24e6b8267a7</anchor>
      <arglist>(const ACG::Vec3d &amp;_dir, const ACG::Vec3d &amp;_up)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setScenePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d129d117d838752a75498affc57cc1ac</anchor>
      <arglist>(const ACG::Vec3d &amp;_center, double _radius)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setScenePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a6fa94d1d14e272693d9d12138fb88c2</anchor>
      <arglist>(const ACG::Vec3d &amp;_center)</arglist>
    </member>
    <member kind="function">
      <type>const ACG::Vec3d &amp;</type>
      <name>sceneCenter</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f8548472c36e305d995dbd7531a8d1e0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sceneRadius</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>259318bab04fb94a356fa923384f36bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>translate</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d53f8c6306496ee49021ae68d8873fc7</anchor>
      <arglist>(const ACG::Vec3d &amp;_vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rotate</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9591fe3955d130d450afe9e8924fc521</anchor>
      <arglist>(const ACG::Vec3d &amp;_axis, double _angle, const ACG::Vec3d &amp;_center)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewHome</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9ad0d550a7d8849130166a8b0c8470d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c4811a358a7fcd79f1b8ea2b6fdfb8e3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>viewingDirection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>74db7476ef45e6a94c2a3556ca362551</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>eyePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c7b81abe92e8f2e4f5d07c6b6ba48f46</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>upVector</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>67be45b4919eb692a281fd7ee1ca4a1e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>orthographicProjection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>642fd4181e6702ade84257495b56f9c2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>perspectiveProjection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7ec716d6a176219b3937994a91ac169a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDrawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5676bf850581cb61763b7829ae28a508</anchor>
      <arglist>(unsigned int _mode)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>drawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>43d94fd55d1a0e22c5c9cba49c52c66b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBackColor</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>37a6e72ec175b6ae536f99aa0ca38f61</anchor>
      <arglist>(OpenMesh::Vec4f _color)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>flyTo</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>25dd779116f4a9a352c5abc1ff9d86ed</anchor>
      <arglist>(const TriMesh::Point &amp;_position, const TriMesh::Point &amp;_center, double _time)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDataRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>500a4c2660d974cc5d9ab8ea0604a5ff</anchor>
      <arglist>(BaseObject *_root)</arglist>
    </member>
    <member kind="function">
      <type>TriMesh *</type>
      <name>triMesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>11d22dc1c64537f87c014241cacc4340</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>PolyMesh *</type>
      <name>polyMesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fe48c8dedf9bf541678277213548d61a</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>TriMeshObject *</type>
      <name>triMeshObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1f03569cd9ea2c79069f362b84c42661</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>PolyMeshObject *</type>
      <name>polyMeshObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>dbb1912ef5640b57a886731049f21413</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>BaseObjectData *</type>
      <name>baseObjectData</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ffb238a514f9754ca2a3680ccb4b59a4</anchor>
      <arglist>(BaseObject *_object)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PluginFunctionsBSplineCurve.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/BasePlugin/</path>
    <filename>PluginFunctionsBSplineCurve_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="no" imported="no">OpenFlipper/common/Types.hh</includes>
    <namespace>PluginFunctions</namespace>
    <member kind="function">
      <type>DLLEXPORT bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>61a339afd69b4a5dd217498a088a78f9</anchor>
      <arglist>(int _identifier, BSplineCurveObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT BSplineCurve *</type>
      <name>splineCurve</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>aad1f17d9abdd7cf4ddb2ee1bd9f68b1</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT BSplineCurveObject *</type>
      <name>bsplineCurveObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>db8b914a5ff8d51712fce5b738b9f975</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PluginFunctionsPolyLine.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/BasePlugin/</path>
    <filename>PluginFunctionsPolyLine_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="no" imported="no">OpenFlipper/common/Types.hh</includes>
    <namespace>PluginFunctions</namespace>
    <member kind="function">
      <type>DLLEXPORT bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>84bcca06e54f5ee597175cbf8e1cb52d</anchor>
      <arglist>(int _identifier, PolyLineObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT PolyLine *</type>
      <name>polyLine</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fcde5199276ba3ea227f6e43ad67e111</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT PolyLineObject *</type>
      <name>polyLineObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>07563d23b56254bfdf9c301774f86f9a</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>BaseObject.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>BaseObject_8hh</filename>
    <class kind="class">BaseObject</class>
  </compound>
  <compound kind="file">
    <name>BaseObjectData.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>BaseObjectData_8hh</filename>
    <includes id="BaseObject_8hh" name="BaseObject.hh" local="yes" imported="no">BaseObject.hh</includes>
    <class kind="class">BaseObjectData</class>
    <member kind="typedef">
      <type>ACG::SceneGraph::MaterialNode</type>
      <name>MaterialNode</name>
      <anchorfile>BaseObjectData_8hh.html</anchorfile>
      <anchor>3a689dfbad9465332a5ad72b9406ffa0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>ACG::SceneGraph::ManipulatorNode</type>
      <name>ManipulatorNode</name>
      <anchorfile>BaseObjectData_8hh.html</anchorfile>
      <anchor>205b89dc931a634a8fca9056bb96a15a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>ACG::SceneGraph::SeparatorNode</type>
      <name>SeparatorNode</name>
      <anchorfile>BaseObjectData_8hh.html</anchorfile>
      <anchor>a6997c8a324868d41fb462fff83a98dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>ACG::SceneGraph::BaseNode</type>
      <name>BaseNode</name>
      <anchorfile>BaseObjectData_8hh.html</anchorfile>
      <anchor>5bfac87fd7e6d7cd056e34dbb133de0d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>BSplineCurveObject.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/BSplineCurve/</path>
    <filename>BSplineCurveObject_8hh</filename>
    <includes id="BSplineCurveTypes_8hh" name="BSplineCurveTypes.hh" local="yes" imported="no">BSplineCurveTypes.hh</includes>
    <includes id="BaseObjectData_8hh" name="BaseObjectData.hh" local="no" imported="no">OpenFlipper/common/BaseObjectData.hh</includes>
    <class kind="class">BSplineCurveObject</class>
  </compound>
  <compound kind="file">
    <name>BSplineCurveTypes.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/BSplineCurve/</path>
    <filename>BSplineCurveTypes_8hh</filename>
    <member kind="typedef">
      <type>ACG::KnotvectorT&lt; double &gt;</type>
      <name>Knotvector</name>
      <anchorfile>BSplineCurveTypes_8hh.html</anchorfile>
      <anchor>e634cac86793b3facfe914026e566d52</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>ACG::BSplineCurveT&lt; ACG::Vec3d &gt;</type>
      <name>BSplineCurve</name>
      <anchorfile>BSplineCurveTypes_8hh.html</anchorfile>
      <anchor>9349a240cf875194952236401379be6b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>GlobalDefines.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>GlobalDefines_8hh</filename>
    <member kind="define">
      <type>#define</type>
      <name>OM_FORCE_STATIC_CAST</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>9ea21f5ddf8cd6c7e8abd4c48673fcab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ENABLE_POLYLINE_SUPPORT</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>e1edb96f4aba589bd4dd584cce20fe3f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ENABLE_BSPLINECURVE_SUPPORT</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>bd19c930c1cf4c5ce2f2e927fc446af3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DLLEXPORT</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>808e08638be3cba36e36759e5b150de0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>GlobalOptions.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>GlobalOptions_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="no" imported="no">OpenFlipper/common/Types.hh</includes>
    <class kind="struct">OpenFlipper::Options::RecentFile</class>
    <member kind="function">
      <type>Experience</type>
      <name>getExperience</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a03e0f8642aa04a2f1f4c61323751609</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setExperience</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>81d79e5ad817abb4978eceac1ca7e345</anchor>
      <arglist>(Experience _exp)</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>applicationDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c22ad98e06d4230c44f46c37c2fc52dc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>pluginDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2027c28cf8e92949c41a3d23a848831b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>textureDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>acf823dfd1f2128ea750250b300c3be3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>iconDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>9b04e2011eee7224c5cc4bb0c6cb3a27</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>applicationDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>5ed8ee7d5f41beb8582c703c0df6f74b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>pluginDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>f335e266a93a627041fcfbe032c07fb6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>textureDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>84c039319744cd15c7a824b34606260b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>iconDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2edc0f88cd2fd6afc2b50bd44fdf58eb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applicationDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>597f6fe9d67377f66ecbe883ef48920a</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pluginDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ca89a2db183ebf7ae0ff91f125cd47c4</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>textureDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a90e98967729d61770b3562278ab37c9</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>iconDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>6b62cee43f2521b4997f981107022d4b</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>applicationDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>7fb17360dfa666879c4fd5b055d33f5a</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>pluginDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>9fdd68dd3f0d6e600d671e724b4a702d</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>textureDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>4d762d73f157170db0b8c6a13e3e2ade</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>iconDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>310129d1f3969729eb8cf59dd9e5d5d2</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>optionFiles</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>06b14db4faaecf9da8a682bb3af9d69b</anchor>
      <arglist>(QStringList _list)</arglist>
    </member>
    <member kind="function">
      <type>QStringList</type>
      <name>optionFiles</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>72e9bf6adf32cff26de1f9a18fc6700f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>configDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>0d99ad3eca78e30f2d35de9a4a1ada0f</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>configDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a4030505a148d25af19eb4ae0a265d9c</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>configDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>992f6732914e89dd7e62a1d304d4afee</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>configDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c44854f007625a4fa53ab407dfbd77b4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>currentDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b4f73b81925180a84a561add1be2d9c1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>currentDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c15257539a323ee3ba7161c1661ffad1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>currentDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>7883115ea12d37c1704b5cdd28860506</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>currentDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b47911b455241d7501daa2fde2917560</anchor>
      <arglist>(QString _dir)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>lastDataType</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b3b1a42a4d991098d5024aa9e8acb23b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lastDataType</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>450d93311c6af8a86827e29244126d6a</anchor>
      <arglist>(QString _type)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRecentFile</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>7a7e42306b03e7238e1bee3f98ac7c64</anchor>
      <arglist>(QString _file, DataType _type)</arglist>
    </member>
    <member kind="function">
      <type>QVector&lt; RecentFile &gt;</type>
      <name>recentFiles</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>86c4cb1858ccbb0110a54b0ec5fd7be3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>recentFiles</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2b3b8eaa470c9165aa74198d596a5af7</anchor>
      <arglist>(QVector&lt; RecentFile &gt; _list)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxRecent</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>cb8c19d6281014d7216c7b91935a268d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>maxRecent</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a4fd80eec0c14676d920502948b5a999</anchor>
      <arglist>(int _max)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is64bit</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ef3f2de3d7792799f457497356ba04f3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is32bit</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>0f8feb9e335e36171f49802908ac2260</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isLinux</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>8368714ed757da5298b67c62cec8dc4f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isWindows</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>cd2f1faa7f12cc12634bbb92db06f8c5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>dirSeparator</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b9e91f275be64cab541156b56bb4d093</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>nogui</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>1f9209e9274a748bc4ec5f54f283c140</anchor>
      <arglist>(bool _nogui)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>nogui</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>f9deee761bdd24d1dd33b785101f90f5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>gui</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>14c2143c7c68a1d1a42e6eaa9addef8c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stereo</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b9ee54e3ebd7848c5f72b624463bd4ea</anchor>
      <arglist>(bool _stereo)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>stereo</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c65a7bab86e9d24953043e80affe9f60</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>animation</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fed0d617d9db544bd778fc02d9c837ff</anchor>
      <arglist>(bool _animation)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>animation</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>bb52e0e9b838fef19c63f6c8b1e189f0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>backfaceCulling</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ed6ed751e36ab95a29bf423cb2f85021</anchor>
      <arglist>(bool _backfaceCulling)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>backfaceCulling</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>73c17200256f7609fb79c2b654bf077a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hideLogger</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>bca502e6aa441f9ff80e01aa4e43c584</anchor>
      <arglist>(bool _hide)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hideLogger</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>23d6d24a3aeb9387f8d1314dbb9467e8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hideToolbox</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>3f8c2fba45cd9b0a7f4bdbb8e3e68d7f</anchor>
      <arglist>(bool _hide)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hideToolbox</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fa59cf1dc129ab9c22d7812e48cb435f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>fullScreen</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a930cc6283b7a192d66d702e64260eb3</anchor>
      <arglist>(bool _fs)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>fullScreen</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>688f5d45dc86b811823d2fdb817b9e86</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>defaultToolboxMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>62fee6635a16039431a2b1e9573bbadd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>defaultToolboxMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ba355bc7abb5f1edced58c22867af30c</anchor>
      <arglist>(QString _mode)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>windowTitle</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>09a7e34a4bcab594bcd70f97be1f748e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>windowTitle</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>d4e8df33c666bc1fa632b9715d1ea546</anchor>
      <arglist>(QString _titel)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>splash</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>48bd8f8f5c2ded891c4edcaa9ec32d9c</anchor>
      <arglist>(bool _splash)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>splash</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c325a50071e55968eac0aaedc3c2f488</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scripting</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a828ede211c1dba9532fea61c26a06df</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scripting</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>72cc3dcf493a1242ca51bf0eaa1290c7</anchor>
      <arglist>(bool _status)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>logToConsole</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>f884dd7848d4e36fd0b020dde905ff58</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>logToConsole</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>e2dd31a56becc01836629122be98e8dc</anchor>
      <arglist>(bool _logToConsole)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>debug</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>0b3ebf6a7f955e829dcc53c3999c44d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>debug</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>d08fc63c13b29eb0abb91ab89c9b4843</anchor>
      <arglist>(bool _debug)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>backupEnabled</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>3a4f4dcaa37ad0d5c10bf01da1c49855</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enableBackup</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>63f5e85c90e8ddd39118b62a9c8d5d95</anchor>
      <arglist>(bool _enableBackup)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>openingIni</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>1b64f0567bd82e800413bea9ade72851</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>openingIni</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>8492ab01ac7e8f676dbae7d29ef1ac2e</anchor>
      <arglist>(bool _openingIni)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PolyLineObject.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/PolyLine/</path>
    <filename>PolyLineObject_8hh</filename>
    <includes id="PolyLineTypes_8hh" name="PolyLineTypes.hh" local="yes" imported="no">PolyLineTypes.hh</includes>
    <includes id="BaseObjectData_8hh" name="BaseObjectData.hh" local="no" imported="no">OpenFlipper/common/BaseObjectData.hh</includes>
    <class kind="class">PolyLineObject</class>
  </compound>
  <compound kind="file">
    <name>PolyLineTypes.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/PolyLine/</path>
    <filename>PolyLineTypes_8hh</filename>
    <member kind="typedef">
      <type>ACG::PolyLineT&lt; ACG::Vec3d &gt;</type>
      <name>PolyLine</name>
      <anchorfile>PolyLineTypes_8hh.html</anchorfile>
      <anchor>b54c407eed4ac40b810973a3f9f397bf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PolyMeshTypes.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>PolyMeshTypes_8hh</filename>
    <class kind="struct">PolyTraits</class>
    <member kind="typedef">
      <type>OpenMesh::PolyMesh_ArrayKernelT&lt; PolyTraits &gt;</type>
      <name>PolyMesh</name>
      <anchorfile>PolyMeshTypes_8hh.html</anchorfile>
      <anchor>6f3eb4ad4fc6f95326b848d821958f35</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>TriangleMeshTypes.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>TriangleMeshTypes_8hh</filename>
    <class kind="struct">TriTraits</class>
    <member kind="typedef">
      <type>OpenMesh::TriMesh_ArrayKernelT&lt; TriTraits &gt;</type>
      <name>TriMesh</name>
      <anchorfile>TriangleMeshTypes_8hh.html</anchorfile>
      <anchor>41cbba34c91d41c5325cc019b097ad91</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Types.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>Types_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="yes" imported="no">GlobalDefines.hh</includes>
    <includes id="BaseObject_8hh" name="BaseObject.hh" local="yes" imported="no">BaseObject.hh</includes>
    <includes id="BaseObjectData_8hh" name="BaseObjectData.hh" local="yes" imported="no">BaseObjectData.hh</includes>
    <includes id="PolyLineObject_8hh" name="PolyLineObject.hh" local="yes" imported="no">PolyLine/PolyLineObject.hh</includes>
    <includes id="BSplineCurveObject_8hh" name="BSplineCurveObject.hh" local="yes" imported="no">BSplineCurve/BSplineCurveObject.hh</includes>
    <member kind="typedef">
      <type>ACG::Vec3d</type>
      <name>Vector</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>7932a926ab17fabe9f4a0ee1b498526c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::vector&lt; int &gt;</type>
      <name>idList</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>1a4f42091f45af22a5d56250cd262c5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>ACG::Matrix4x4d</type>
      <name>Matrix4x4</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>2c834138d15bf416d68149d639c97ed8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::vector&lt; BaseObjectData * &gt;</type>
      <name>DataContainer</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>439e837834c4200e2bc2e050ae2cce72</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>MeshObject&lt; TriMesh, DATA_TRIANGLE_MESH &gt;</type>
      <name>TriMeshObject</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>90c8b126d070ab854ae3d7c6bc27cb29</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>MeshObject&lt; PolyMesh, DATA_POLY_MESH &gt;</type>
      <name>PolyMeshObject</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>6d28240c740be2a804d6d33ff2cc3580</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>DataType</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_NONE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb67a0fbad905fa5e37fc0f68349511bdbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_GROUP</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb6e5b1d273fdfedbcca48072627cf89d99</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_POINTS</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb65d6368e367cbc3a591561b72d4aa06fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_POLY_LINE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb6aa12550370fd882d1bb6d0d3e3085a03</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_TRIANGLE_MESH</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb637489b6e73aed7dd2c0928e8a6ea316b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_POLY_MESH</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb6c94d286d0f8a2ddb5524b376d9e72924</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_VOLUME</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb68406f4a86dfe7cec00ca8dba46681542</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_BSPLINE_CURVE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb62b459aab77e19250ba6182de624d40db</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DATA_BSPLINE_SURFACE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d8ed01ff3ff33333d8e19db4d2818bb6733daec6131b1998a282beee8f633cf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Q_DECLARE_METATYPE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>c419bb898d03aee05f11a096602ec958</anchor>
      <arglist>(idList)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Q_DECLARE_METATYPE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>528b579899d8d6e554a0cca11dbedb7f</anchor>
      <arglist>(Vector)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Q_DECLARE_METATYPE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>cfda4e9e8cc954e22bf07d085b5fb0fb</anchor>
      <arglist>(Matrix4x4)</arglist>
    </member>
    <member kind="variable">
      <type>const DataType</type>
      <name>DATA_ALL</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>fbd656a920709cf466fe36603ef2508d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Core.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Core/</path>
    <filename>Core_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="yes" imported="no">OpenFlipper/common/Types.hh</includes>
    <includes id="Logging_8hh" name="Logging.hh" local="yes" imported="no">OpenFlipper/Logging/Logging.hh</includes>
    <includes id="Scripting_8hh" name="Scripting.hh" local="yes" imported="no">OpenFlipper/Scripting/Scripting.hh</includes>
    <includes id="CoreWidget_8hh" name="CoreWidget.hh" local="no" imported="no">OpenFlipper/widgets/coreWidget/CoreWidget.hh</includes>
    <includes id="GlobalOptions_8hh" name="GlobalOptions.hh" local="no" imported="no">OpenFlipper/common/GlobalOptions.hh</includes>
    <class kind="struct">fileTypes</class>
    <class kind="class">Core</class>
    <member kind="function">
      <type>QScriptValue</type>
      <name>myPrintFunction</name>
      <anchorfile>Core_8hh.html</anchorfile>
      <anchor>7a77a0e3a28542e4ccdf8d0ca40e085c</anchor>
      <arglist>(QScriptContext *context, QScriptEngine *engine)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Logging.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Logging/</path>
    <filename>Logging_8hh</filename>
    <class kind="class">PluginLogger</class>
  </compound>
  <compound kind="file">
    <name>Scripting.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Scripting/</path>
    <filename>Scripting_8hh</filename>
    <class kind="class">ScriptingWrapper</class>
  </compound>
  <compound kind="file">
    <name>CoreWidget.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/coreWidget/</path>
    <filename>CoreWidget_8hh</filename>
    <includes id="Types_8hh" name="Types.hh" local="yes" imported="no">OpenFlipper/common/Types.hh</includes>
    <class kind="struct">PluginInfoT</class>
    <class kind="struct">ViewMode</class>
    <class kind="class">StackWidgetInfo</class>
    <class kind="struct">MenuInfo</class>
    <class kind="class">CoreWidget</class>
  </compound>
  <compound kind="page">
    <name>dataStructure</name>
    <title>Datastructures</title>
    <filename>dataStructure</filename>
    <docanchor file="dataStructure">creatingCustomObjectTypes</docanchor>
    <docanchor file="dataStructure">MeshObjectDescription</docanchor>
    <docanchor file="dataStructure">Overview</docanchor>
    <docanchor file="dataStructure">baseObjectDescription</docanchor>
    <docanchor file="dataStructure">baseObjectDataDescription</docanchor>
    <docanchor file="dataStructure">basicObjectTypes</docanchor>
  </compound>
  <compound kind="page">
    <name>interfaces</name>
    <title>Plugin Interfaces</title>
    <filename>interfaces</filename>
    <docanchor file="interfaces">KeyPlugin</docanchor>
    <docanchor file="interfaces">BasePlugin</docanchor>
    <docanchor file="interfaces">ToolboxPlugin</docanchor>
    <docanchor file="interfaces">MousePlugin</docanchor>
    <docanchor file="interfaces">PickingPlugin</docanchor>
    <docanchor file="interfaces">TexturePlugin</docanchor>
    <docanchor file="interfaces">INIPlugin</docanchor>
    <docanchor file="interfaces">MenubarPlugin</docanchor>
    <docanchor file="interfaces">LoggingPlugin</docanchor>
  </compound>
  <compound kind="page">
    <name>pluginProgramming</name>
    <title>Plugin Programming</title>
    <filename>pluginProgramming</filename>
    <docanchor file="pluginProgramming">plugin_sec</docanchor>
    <docanchor file="pluginProgramming">plugin_prog_sec</docanchor>
  </compound>
  <compound kind="struct">
    <name>AreaNodeMod</name>
    <filename>structAreaNodeMod.html</filename>
    <templarg>MeshT</templarg>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_area</name>
      <anchorfile>structAreaNodeMod.html</anchorfile>
      <anchor>bf58574e4881161d0607a292db308dc5</anchor>
      <arglist>(const MeshT &amp;_m, typename MeshT::VertexHandle _vh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_area</name>
      <anchorfile>structAreaNodeMod.html</anchorfile>
      <anchor>fd2d9eed7e92bfd3da98160754659b0e</anchor>
      <arglist>(const MeshT &amp;_m, typename MeshT::FaceHandle _fh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_vertex_selected</name>
      <anchorfile>structAreaNodeMod.html</anchorfile>
      <anchor>c64f44c9832c9ee91ff301d0e3eb8fbd</anchor>
      <arglist>(const MeshT &amp;_mesh, typename MeshT::VertexHandle _vh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_edge_selected</name>
      <anchorfile>structAreaNodeMod.html</anchorfile>
      <anchor>a3a6b26a116722b3d8a3d971d150f5eb</anchor>
      <arglist>(const MeshT &amp;, typename MeshT::EdgeHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_face_selected</name>
      <anchorfile>structAreaNodeMod.html</anchorfile>
      <anchor>67620829bd2f578e8b95b731e60d048e</anchor>
      <arglist>(const MeshT &amp;_mesh, typename MeshT::FaceHandle _fh)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BackupInterface</name>
    <filename>classBackupInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>createBackup</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>866910a0021ab5279cbb9556881e1cd4</anchor>
      <arglist>(int _id, QString _name)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BackupInterface</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>3545a3052a793ccda0d66042e1f1d202</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotBackupRequested</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>3adae57ded51cdcfa2ccd80497204943</anchor>
      <arglist>(int _id, QString _name, int _backup_id)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotRestoreRequested</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>08a636db130ea2ec5bd17c079b46eea8</anchor>
      <arglist>(int _id, QString _name, int _backup_id)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BaseInterface</name>
    <filename>classBaseInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>update_view</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>c78de79f3797cb65f590c9aa84b38f71</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updated_objects</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>9bd2a94322fcd438a70b63753d5a1105</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>activeObjectChanged</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>8352ea374c4893bad326f3fc93cb6667</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BaseInterface</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>ab4caea3727739ce97fa28bdc5ddf575</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QString</type>
      <name>name</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>2b351aa6ef0eb368bce735e243235958</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QString</type>
      <name>description</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>43f0d3612695b53c848c14f48fea9a7a</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotObjectUpdated</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>e58a2d95eae6eddf5df443beb20f3c44</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotActiveObjectChanged</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>67453077e07cfd4b9f92c27046c861bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>initializePlugin</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>705fdf2e221af47198aed689b0da7a32</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>pluginsInitialized</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>5f240478907447bf172618bd4205df65</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>exit</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>2087c7acf96aa7c4ef44f0b4508745a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>noguiSupported</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>79ba17f1f9aca08b95ec56f85d2d1204</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BaseObject</name>
    <filename>classBaseObject.html</filename>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>id_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>595df29a993f17654f4d19cc5cca1e6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>persistentId_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>53d0ccddbcb9dd50f80b2d293ec855f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>id</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>f95d3fe98ac3291056c65bb8a176ca9f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>persistentId</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>aabb2ca9cf68252969bbb2c3b597d3f1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>persistentId</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>3f84ae8785eaecb34c933e5707af45ca</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>DataType</type>
      <name>objectType_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>7dd348f7adfe20a6c21580c15d7f4e4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dataType</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>ced3412859c918f66b09f64fd6fe7979</anchor>
      <arglist>(DataType _type)</arglist>
    </member>
    <member kind="function">
      <type>DataType</type>
      <name>dataType</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>455c872b435f2058315f8488e3569888</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDataType</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>52a09ec4fddbe98c8f6ba9ea6de21a24</anchor>
      <arglist>(DataType _type)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>target_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>5b26b3e2c7ee66520255f621582c48bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>source_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>644af1e38f7e8a4d79c6f45c79a0cbf4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>target</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>f4a514366ad189280af7afeab921bad9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>target</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>edceaa1ec665b4c181f6962bb17c9715</anchor>
      <arglist>(bool _target)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>source</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>d8330b5203d97559a4850e7af8e69a57</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>source</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>2b8c34a2eddf97738ea980a1f881b1be</anchor>
      <arglist>(bool _source)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>visible_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>b379f185320ebd74ad19009d9d758545</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>visible</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>ffd9139ebbcc5bfcf2789ca89a39836c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BaseObject *</type>
      <name>parentItem_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>62559d469dc29a30bb75709645c62f12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QList&lt; BaseObject * &gt;</type>
      <name>childItems_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>a9f825937872fda86e579b04f8120e8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>next</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>497c4fd91698490adf07723b2639015b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>level</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>3b28945e9e8cb941941dd40de8bbd053</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>name_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>a959e7abf0b0bf67b7481e6bf209590a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>name</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>31062de44e42146933dcef3fbe3a0400</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setName</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>b8203eaf661f6ddd0eff27243c1cecd7</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BaseObject</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>2eeb757b362888655e06fdbf4b9524a7</anchor>
      <arglist>(BaseObject *_parent=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanup</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>c246e42b17b1ed17b5db9f3de93bd83d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QString</type>
      <name>getObjectinfo</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>16966c1c32af694839bc4d31eed8759b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>printObjectInfo</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>2327fade4b2b7e1b82351f2076953953</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>update</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>95b4d384dc501c88d89a92143b71ce62</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dumpTree</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>971699e251c18482516af5d1466d8a00</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>row</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>c153538c574dc5960956ee033a0f2113</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>parent</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>3e5b69de1acfaea3eeadcfb2b7a4d29f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParent</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>5b411a7d63dc0a4f31285e53f402df01</anchor>
      <arglist>(BaseObject *_parent)</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>childExists</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>623635b510ec9de5fc83e9137cee84ad</anchor>
      <arglist>(int _objectId)</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>childExists</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>6628373a338349a55fcac93ca2168a7e</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>appendChild</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>8a5bec1ff2bb7514df9f8d553f8928df</anchor>
      <arglist>(BaseObject *child)</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>child</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>bd729e3bd6849be9197c42c92cfe8199</anchor>
      <arglist>(int row)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>childCount</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>4a2d812b3194bfd9491220a901f0d63b</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeChild</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>1b26ef1cc64c344cefc6c1d0835b4e3f</anchor>
      <arglist>(BaseObject *_item)</arglist>
    </member>
    <member kind="function">
      <type>QList&lt; BaseObject * &gt;</type>
      <name>getLeafs</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>2829a3f80a5efa0d5087382a19537a00</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteSubtree</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>75009c3dfb05cd386d69ab8c649a6eb9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>group</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>89a917f54462914c62d712b45cb0523f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isGroup</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>46659b816996b2f7bcc561f666763a03</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInGroup</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>3223cfc78a18fc4aa944d7e15b56f085</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInGroup</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>ff62ad2df502d0689e8d7feb5d1958df</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; int &gt;</type>
      <name>getGroupIds</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>3204170c6520f3b44a2ab137cf662986</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QStringList</type>
      <name>getGroupNames</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>07e96b0a2d857e8c6b093e97640075db</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>BaseObjectData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>43d1abab334341a6473a92b16e508eed</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BaseObjectData</name>
    <filename>classBaseObjectData.html</filename>
    <base>BaseObject</base>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>path_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>0465c8d8d8f2020869a5d3ab22b8282b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFromFileName</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>bd569cf0a5616e978ce4f945e8783fe0</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setName</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>625a358192b886259eec57f134f0b363</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>path</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>359d9008674eaaef8ea5dde70c202638</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>path</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>ee81052e342412e78b0f94d99c784c5a</anchor>
      <arglist>(QString _path)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>manipPlaced_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>2a18977aaabede76890ae6ea4a3ccd4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode *</type>
      <name>rootNode_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>56324c0a9a874aac9aa4bfc68708017f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode *</type>
      <name>separatorNode_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>ca64467531a2916b233191ead3c32e33</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ManipulatorNode *</type>
      <name>manipulatorNode_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>b1caddea1367c56cc036f2802906edc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>MaterialNode *</type>
      <name>materialNode_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>9379caeb475370b65cf091ad8e30b883</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>show</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>991588f6f5afc6f48fa08607646da9f7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hide</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>9d48ae4e906a01d1aed3e061799adfe4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>SeparatorNode *</type>
      <name>baseNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>8317cce9d65624082fc3d3b3f0c0b3fd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ManipulatorNode *</type>
      <name>manipulatorNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>e485628371fc7d87e90dc8309b558b70</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>MaterialNode *</type>
      <name>materialNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>abc3f13376927eadadb4614629b95a4d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBaseColor</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>46029536153d93f046abbe52db5a5d5f</anchor>
      <arglist>(ACG::Vec4f _color)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>manipPlaced</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>eae2e01cea709b1f15c12dce3e5128e6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>manipPlaced</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>d20704f0c5e57b1e42700c8c070b8f54</anchor>
      <arglist>(bool _placed)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; std::pair&lt; BaseNode *, QString &gt; &gt;</type>
      <name>additionalNodes_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>f59ccde862bd86f82f436c7896a24573</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>addAdditionalNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>acb0e61a1e0c29607d5c2131b4e579cb</anchor>
      <arglist>(NodeT *_node, QString _pluginName, QString _nodeName, int _id=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasAdditionalNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>7a866edb3ec8c6268f47ab3657996d03</anchor>
      <arglist>(QString _pluginName, QString _nodeName, int _id=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getAdditionalNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>8c2b55683274e7a0f71858bdb4bc99fb</anchor>
      <arglist>(NodeT *&amp;_node, QString _pluginName, QString _nodeName, int _id=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BaseObjectData</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>cd442dfd346e1d78ec898b013226e9d8</anchor>
      <arglist>(SeparatorNode *_rootNode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BaseObjectData</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>b7ba36271607ad06cecd525ed0080ac3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanup</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>e8e8f4f5995a356a9876d354c2822afd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>picked</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>fdb0f57814b92dd4ad19c81b1601857c</anchor>
      <arglist>(uint _node_idx)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>update</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>d994484e36b811867db1cb2b829525c7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>init</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>6dad55c2bc5d38e0ca09af501323e9dc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BSPImplT::NearestNeighbor</name>
    <filename>structBSPImplT_1_1NearestNeighbor.html</filename>
    <member kind="function">
      <type></type>
      <name>NearestNeighbor</name>
      <anchorfile>structBSPImplT_1_1NearestNeighbor.html</anchorfile>
      <anchor>d2cf3591d52f2edaecb6f5ccf76e4d66</anchor>
      <arglist>(Handle _h, Scalar _d)</arglist>
    </member>
    <member kind="variable">
      <type>Handle</type>
      <name>handle</name>
      <anchorfile>structBSPImplT_1_1NearestNeighbor.html</anchorfile>
      <anchor>b81ccbed172f46480566e62c01db7d35</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Scalar</type>
      <name>dist</name>
      <anchorfile>structBSPImplT_1_1NearestNeighbor.html</anchorfile>
      <anchor>79a7b2b20af53a5fb0705a4c9a12ece8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BSPImplT::NearestNeighborData</name>
    <filename>structBSPImplT_1_1NearestNeighborData.html</filename>
    <member kind="variable">
      <type>Point</type>
      <name>ref</name>
      <anchorfile>structBSPImplT_1_1NearestNeighborData.html</anchorfile>
      <anchor>8c032ac1510a12d2ad1fc2db19c6f5c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Scalar</type>
      <name>dist</name>
      <anchorfile>structBSPImplT_1_1NearestNeighborData.html</anchorfile>
      <anchor>fed85561f115cd4c01a1428c96d6b7f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Handle</type>
      <name>nearest</name>
      <anchorfile>structBSPImplT_1_1NearestNeighborData.html</anchorfile>
      <anchor>d18ce86a74e06a9c1895ad9fd4746ee0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BSplineCurveObject</name>
    <filename>classBSplineCurveObject.html</filename>
    <base>BaseObjectData</base>
    <member kind="variable" protection="private">
      <type>BSplineCurve *</type>
      <name>splineCurve_</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>47f769ce688506f237bb2f016388dd49</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>BSplineCurve *</type>
      <name>splineCurve</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>04981c2f9c83c76b2720e24d860fd3b9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::BSplineCurveNodeT&lt; BSplineCurve &gt; *</type>
      <name>splineCurveNode_</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>9669fb84cd35605c5f84efa12fdfa1fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>ACG::SceneGraph::BSplineCurveNodeT&lt; BSplineCurve &gt; *</type>
      <name>splineCurveNode</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>3dc679cec059d6841e57ed22797639f9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BSplineCurveObject</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>4c60e0f5c68cf4e1d328214262485f02</anchor>
      <arglist>(SeparatorNode *_rootNode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BSplineCurveObject</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>cceec799fb7aebebb73e850dd58f6274</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanup</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>78b312d47d5ce31b99638ced7de4f637</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>9fdcd89101c1d2604ba5a57abe765af4</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>getObjectinfo</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>41e882d0efd13025bbb04282c98623c0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>picked</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>2e02b3615d1461f495c16baa709b899a</anchor>
      <arglist>(uint _node_idx)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>init</name>
      <anchorfile>classBSplineCurveObject.html</anchorfile>
      <anchor>66eb6e2bd106aba4e1f053057aaeb387</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ContextMenuInterface</name>
    <filename>classContextMenuInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addContextMenu</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>2bc4fd5c6257d91a3a879ba5f54107aa</anchor>
      <arglist>(QMenu *_menu, ContextMenuType _type)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addContextMenu</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>286f33392df92c259fd2312bf8fcdbfd</anchor>
      <arglist>(QMenu *_menu, DataType _objectType, ContextMenuType _type)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ContextMenuInterface</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>60e636363c9fd434860c981ef926b0d2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotUpdateContextMenu</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>74cbe2257620672cbc565cc6536c0332</anchor>
      <arglist>(int _objectId)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Core</name>
    <filename>classCore.html</filename>
    <member kind="variable" protection="private">
      <type>std::vector&lt; PluginInfoT &gt;</type>
      <name>plugins</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>12bdd8b258466fbe623c566be0c4f8e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QStringList</type>
      <name>dontLoadPlugins_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8dc6b5c7ccd884a0fab6849f7a03ce41</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>toolboxindex_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>d78748a6a0af01c7f6b3fef7cd944d27</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotUnloadPlugin</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>e0f29beb45111f7a18ae7ff964c450c8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>unloadPlugin</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>71a4e4d721fea0199ee5b1b8a04caf11</anchor>
      <arglist>(QString name)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>dontLoadPlugins</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>d95c96971f9497cee3478109c1899087</anchor>
      <arglist>(QStringList _plugins)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLoadPlugin</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>5b221f4b9f21ad191bd9aedf098ae684</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>loadPlugin</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2e54ef01d2bacc02d33e02749cc2c7a6</anchor>
      <arglist>(QString filename, bool silent)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>loadPlugins</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>524c421d35b6a388455de321bbd44953</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QScriptEngine</type>
      <name>scriptEngine_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>a6948437b70817a32a8a540fed21bf31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; ScriptingWrapper * &gt;</type>
      <name>scriptingWrappers_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>5c3c57bfe0532ee496bb170a5f152ea8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QStringList</type>
      <name>scriptingFunctions_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>5947e8bba9168c6919c1564cbc8b1496</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>prototypeVec3d</type>
      <name>vec3dPrototype_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>e48798304abda3207e7c0b88fc04f4f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>prototypeMatrix4x4</type>
      <name>matrix4x4Prototype_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>de3704eb2c05857fdeaa594fc0ab03be</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotScriptInfo</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>1012c4002095f6b4388988963d69db3f</anchor>
      <arglist>(QString _pluginName, QString _functionName)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotExecuteScript</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3496fdcb9d0cb2229eb44a9f1402494e</anchor>
      <arglist>(QString _script)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetScriptingEngine</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ecbcfb207be972e54677e8c589420661</anchor>
      <arglist>(QScriptEngine *&amp;_engine)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetAllAvailableFunctions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>07f24f908f78575d3461c6ea20fc15c7</anchor>
      <arglist>(QStringList &amp;_functions)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>scriptInfo</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>54b47351aaa8abd85fea2e5e03b66e9a</anchor>
      <arglist>(QString _pluginName, QString _functionName)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>executeScript</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>64dd6ec020fde218c91dbaa033095f90</anchor>
      <arglist>(QString _script)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>executeFileScript</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>08ff5518ad3e1de36e35128bf8fa1f42</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>commandLineOpen</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3a17724b1a6b634650fd2d49ade3b37a</anchor>
      <arglist>(const char *_filename, bool asPolyMesh)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>commandLineScript</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6a0ac46c3400efe71ee265e465f4e4f7</anchor>
      <arglist>(const char *_filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>addEmptyObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>fb81bd1994d1b9a9a62982b618e9a806</anchor>
      <arglist>(DataType _type)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>loadObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>83b5472d8d30510c6eb8dc04341956f3</anchor>
      <arglist>(DataType _type, QString _filename)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>saveObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f5cb94dacfd9bb358a34cfd93acbc1fd</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>saveObjectTo</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0e32e8b8667e22348f236aba050e20e8</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>int</type>
      <name>loadObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8fc55f139463d796c4f9e1d235f11c6f</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>resetScenegraph</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>bd168f66ef1cc8003b4002152c16888f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scriptLogFunction</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8d1172986583f15197a55bf3fc1348ae</anchor>
      <arglist>(QString _output)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLog</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>5d9187a7515d482e9434bc52c83221c3</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>log</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ca47c7ecad26bcce34f6ab099e7f216e</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>log</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3be8c200c4dac0b05ce7f1935a5bf0b3</anchor>
      <arglist>(QString _message)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>scriptLog</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4f8c108f0215e02e7caa852e2c5b3db7</anchor>
      <arglist>(QString _message)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>externalLog</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f74194950cc01393a6141d14aaf74010</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>openIniFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>9318f30d3faec5401af0f6c61b79861b</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>setupOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4ba499f73a95a5595519e20fbbaf1677</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>applyOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>1552c32c581b43c9bbbbd2872e65ce00</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>readApplicationOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>77241fa8487f84cdf3aea5f29df7ecf1</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeApplicationOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>5a6c450fb5a8875ee0817d66cc2bad3a</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeIniFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>701a82bd39354b33871a555e780ae555</anchor>
      <arglist>(QString _filename, bool _relativePaths)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeImportant</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>208ee1afdea615decde60ef3fbcfc9b7</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeOnExit</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6aca5df19b85d926d3d977ac5d970ec0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>updateView</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>c747b324a49a921a26a8d6d3492d95cc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>clearAll</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0b9f852b3653dc3a570806f6b46821c3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>exitApplication</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ce1115b7dac51436b408bc2fd0bd4230</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>ObjectListUpdated</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>cd8b71728fb4d9d04a999d016ed4610c</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>PluginWheelEvent</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>cbe6159879b30970436741db5286749a</anchor>
      <arglist>(QWheelEvent *, const std::string &amp;)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>PluginMouseEvent</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>362a33d6f3dd30fc5eaf9a5348978973</anchor>
      <arglist>(QMouseEvent *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>PluginMouseEventIdentify</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>d6b73d6490552ec1bec4e775c3006453</anchor>
      <arglist>(QMouseEvent *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>activeObjectChanged</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>785f93022a66cf9c89f43bd106f3a09d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>addTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ff320fa257323d287645eb1d250b2365</anchor>
      <arglist>(QString, QString, uint)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>updateTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0a404f59f1605f3a20decc135b9436f2</anchor>
      <arglist>(QString, int)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>updateAllTextures</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>47cde6edf6349d0bbc2d57eaca2e32f0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>updatedTextures</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0cfe01f8b312f7d8a1d8c35407111d42</anchor>
      <arglist>(QString, int)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>setTextureMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6401349cae08a60a416ac35c93000c25</anchor>
      <arglist>(QString _textureName, QString _mode)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>switchTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>c87a4fe7b6d0dd7597928d79562c893c</anchor>
      <arglist>(QString)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>iniLoad</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>886333188719c1a54ee915fd5c3fafd3</anchor>
      <arglist>(INIFile &amp;, int)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>iniSave</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>86631e905550201c59e0c5c19f0de5b3</anchor>
      <arglist>(INIFile &amp;_ini, int _id)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>iniSaveOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>464b9c689f031a385c10fda164c813be</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>iniLoadOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>7ba3d4b3589e7f43108d776db9b9eac4</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>createBackup</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>c240186fd9d5c711eb1d32c066ee333f</anchor>
      <arglist>(int _id, QString _name, int nextBackupId_)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>openedFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>539645222d2382d13f599a3e00a5e63e</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>pluginsInitialized</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>602ff944213d23bd10424d3dfea1ca59</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Core</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>14e63188e0aa7c4a6f72d5501384d1f9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>95b5c90a86bd4f1dec37edb1f32a6406</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Core</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>776f8c46504b14183883c6273f93eaed</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>add_sync_host</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2d90ef7054a62dd168fe7d7f1d28ec3a</anchor>
      <arglist>(const QString &amp;_name)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotMouseEventIdentify</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>927e3433b4b6544d0fdbad996f70050e</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotObjectsListUpdated</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4a8c2a094d5e8d6a27661eb447e9ba6b</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotActiveObjectChanged</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2b43dec3add3e44237ba6c66118bb112</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddPickMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>24344a21b2e6bcca23a3f2e187a291e5</anchor>
      <arglist>(const std::string _mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddHiddenPickMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f98adee68d079c9259a2e4f67d7d0dbe</anchor>
      <arglist>(const std::string _mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddPickMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8cc13fd3e3bd3ed65afd790a674673c6</anchor>
      <arglist>(const std::string _mode, QCursor _cursor)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddHiddenPickMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>40b9e175c106e447eeeeb5c0970b89d9</anchor>
      <arglist>(const std::string _mode, QCursor _cursor)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>fd694ebabd9d74be05f3088a21836e92</anchor>
      <arglist>(QString _textureName, QString _filename, uint _dimension)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotUpdateTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>91a3737dba315b0fd22c82a964a2074b</anchor>
      <arglist>(QString _name, int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotUpdateAllTextures</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>69502ab8347635d2356d965c0328d254</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotTextureUpdated</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>88c282e7614ce221ecfdc456ab946970</anchor>
      <arglist>(QString _textureName, int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSetTextureMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>550c71a523f3507926115d8b66ff070b</anchor>
      <arglist>(QString _textureName, QString _mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSwitchTexture</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>18661ce18b00e0911979f28cbe0a30bf</anchor>
      <arglist>(QString _textureName)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>backupRequest</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>23a08ed7784027e25d17e505297043a1</anchor>
      <arglist>(int _id, QString _name)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLoad</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>9df13efa1eb583229fb259ad3e4c936c</anchor>
      <arglist>(QString _filename, DataType _type, int &amp;_id)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSave</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>51c3165bf7fb89b9468801a36d58c9f2</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotObjectOpened</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0c9daef85da765a82e28f5dfaf970ce6</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotEmptyObjectAdded</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>66958f0632efb7402cc85c4bf91efb93</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetAllFilters</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>069d0d8edd3f9f3ed9f45449e529ee4b</anchor>
      <arglist>(QStringList &amp;_list)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotMouseEvent</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>16dd77f09fae4b95027d3d6dec0dc6db</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotWheelEvent</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2919109a2223354d076c5cd5739d1048</anchor>
      <arglist>(QWheelEvent *_event, const std::string &amp;_mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddEmptyObjectMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>520e60b1fcb8dca72a479cd99cc1cdc2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddEmptyObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>baf12c0c5eb4428fb8e4654f53214416</anchor>
      <arglist>(DataType _type, int &amp;_id)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSaveMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2743f4b120198eac85e54239fe0674b0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSaveToMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>105857c5fc93289fc55c61da93a2ad8f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLoadIniMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8264d33dcaee527ca17c845e4f81745c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSaveIniMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>db6853276fd5b334ee791723ae751a5f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotExit</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>df3cef1b50fdbdefde8a5fd979a5d744</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLoadMenu</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>665a111eb61e7625daab3dfa442bf749</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotRecentOpen</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>dd36584ea0ae76483c6a5802b30ffb0f</anchor>
      <arglist>(QAction *_action)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotPluginExists</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>7a00b0ec35feb19bbe45a525dd6cdd60</anchor>
      <arglist>(QString _pluginName, bool &amp;_exists)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotFunctionExists</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>b251425765b397ae026fcc9b60d60503</anchor>
      <arglist>(QString _pluginName, QString _functionName, bool &amp;_exists)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotCall</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4366a69ca73c14f63cd8ba2c7b83200a</anchor>
      <arglist>(QString _pluginName, QString _functionName, bool &amp;_success)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotCall</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>b1eeae464678294e53a3cdeb2d585c78</anchor>
      <arglist>(QString _expression, bool &amp;_success)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>checkSlot</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>7561ff0b3585f16e1c10554fe90c0807</anchor>
      <arglist>(QObject *_plugin, const char *_slotSignature)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>checkSignal</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f7e086567ff6d8a0f30d23c5854709db</anchor>
      <arglist>(QObject *_plugin, const char *_signalSignature)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>nextBackupId_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>e8d38537099ad4b53ece9058f47ff79f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>unsigned int</type>
      <name>standard_draw_mode_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6eb3b1f7ae4a17dc91ecad9508f6c620</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>set_random_base_color_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2cdcf198df3244d18d991d71bab9f23a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; PluginLogger * &gt;</type>
      <name>loggers_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ba18ffb88705ca880f5b860ed0ee3188</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; fileTypes &gt;</type>
      <name>supportedTypes_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4c1f6e004ff1caa881ac61223dab298f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BaseObject *</type>
      <name>objectRoot_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>1db8f2235b948eb13ca1bee83e620c83</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>CoreWidget *</type>
      <name>coreWidget_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>fca364429c7e22172d8c7420f808b42b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QSplashScreen *</type>
      <name>splash_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>88b6df81b38625ffd7f8309bdb21c245</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>splashMessage_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>973c63b234153a78b40dc67032e68fed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QVector&lt; ViewMode * &gt;</type>
      <name>viewModes_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6b612c8f6b9e55c8683caaf12d44d893</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode</type>
      <name>root_node_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>7f2eb7b1aaf9456cef3e743741513abc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CoreWidget</name>
    <filename>classCoreWidget.html</filename>
    <member kind="variable" protection="private">
      <type>QVector&lt; QWidget * &gt;</type>
      <name>toolWidgets_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>949fb8d562fd9349f8e9da6de1f743ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QPushButton *</type>
      <name>viewModeButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e468832db236aa3f5423cef447474c86</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>viewModeMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>ba968a9e6721f1a64df64f12803a87da</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QActionGroup *</type>
      <name>viewGroup_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>072b46c4e561ca3fb0edb090af505ad8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QVector&lt; ViewMode * &gt; &amp;</type>
      <name>viewModes_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>21a568606035083c6df70c773fa18433</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QDockWidget *</type>
      <name>dockViewMode_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8d6e839761de0c384204753fa249e45d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>tabDockWidgets</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>36b2a3fefe23d4f344c685ece0ccf233</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotRemoveViewMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8d23490824aefe6e80aae4453031de6d</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSetViewMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>190fe0417173fa185747d9eece482527</anchor>
      <arglist>(QAction *action)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotViewModeDialog</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>55e8d857cc772abd295e4464f32a1e10</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotVisibilityChanged</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>80187a1ac1a2267593293e396132392c</anchor>
      <arglist>(bool visible)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>initViewModes</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5323e0ac079d1adc2e83fe7d5e7692e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>slotAddViewMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>aa4e596bc2bb727328f0bec65d10ed82</anchor>
      <arglist>(QString _mode, QStringList _usedWidgets)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>slotAddViewMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a34a137632c4931182ec1336b77f5efd</anchor>
      <arglist>(QString _mode, bool _custom, QStringList _usedWidgets)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>slotChangeView</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0dc3a22e112a1a25144128da9ae6ceca</anchor>
      <arglist>(QString _mode, QStringList _toolWidgets)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setViewMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3a2855184f44aa4459b34af76d4524a7</anchor>
      <arglist>(QString _mode)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>fileMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7cb761b9e102628175ba109afc6adf64</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>viewMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5b9c0274c15d277d0e96c4cd411850c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QAction *</type>
      <name>fileMenuEnd_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>01b72518b154bd0b561969e4d491691f</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>555b4799a6d28f89ba1fe8fa1550e841</anchor>
      <arglist>(QMenu *_menu, MenuType _type)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>contextMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0233644347b1db500bfc3fefa5079c6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>contextSelectionMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2be74ee3a61b1ef89939a0c99fc50be2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; MenuInfo &gt;</type>
      <name>persistentContextMenus_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>91a4f9c608d20fafbebbc4d043e61287</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; MenuInfo &gt;</type>
      <name>contextMenus_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>15e7548852b67675459fb079248e4545</anchor>
      <arglist></arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>updateContextMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>54a16d25d1d85723d231ccb0af0a07d9</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotCustomContextMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a0ac0cca2bf9766ace6bc793c55c67d3</anchor>
      <arglist>(const QPoint &amp;_point)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddContextMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c0fae3ce64c83810425bec10cc6e9c34</anchor>
      <arglist>(QMenu *_menu)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddContextMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>b1643539c56985b9d8373cdcecaf6ad1</anchor>
      <arglist>(QMenu *_menu, DataType _dataType, ContextMenuType type_)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>updatePopupMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a25f86c929c5ce5fb998f73e3e59cea2</anchor>
      <arglist>(const QPoint &amp;_point)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>stackMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8a6b029cb374e2dfdd9b2ef92970e8f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; StackWidgetInfo &gt;</type>
      <name>stackWidgetList_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a9e27b67dc20e04ea96f5b7e732cee1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QStackedWidget *</type>
      <name>stackedWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c3a7d78838f42c299d23f526d1bd1e3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetStackWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2a9e136e120f41274cf5d472e3e15412</anchor>
      <arglist>(QString _name, QWidget *&amp;_widget)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddStackWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>40aa4316a8241e60e1e725dae620827e</anchor>
      <arglist>(QString _name, QWidget *_widget)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotUpdateStackWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>34c9090c60566c2c2c1b4383b19c45b6</anchor>
      <arglist>(QString _name, QWidget *_widget)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotViewMenuAction</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>01d69334199a6ddf3d7e9ccdc9f9d2f1</anchor>
      <arglist>(QAction *_action)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotUpdateStackMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d5c9984d6a3fa528c36e5e08f3e429a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>HelpWidget *</type>
      <name>helpBrowserDeveloper_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>1fe2d9862d8ceca857ac17793bd413e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>HelpWidget *</type>
      <name>helpBrowserUser_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>692ef7880cfefbcefe34b0785eac1654</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showHelpBrowserUser</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d444bacad98dd3ae7f1d9764c50191da</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showHelpBrowserDeveloper</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2c41373eb2794800d69e70feb65f96bc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolBar *</type>
      <name>mainToolbar_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>ecb53a1b74438c95b056a6a501977c70</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAddToolbar</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>67899ab619f915a781c6d751fc94f97e</anchor>
      <arglist>(QToolBar *_toolbar)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotRemoveToolbar</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0641ddf9a613275830848be3401ed564</anchor>
      <arglist>(QToolBar *_toolbar)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>PluginKeyEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>55e1c8c9061c5826ebeed3337d7a2a4a</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>PluginKeyReleaseEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>96fbfaffc5f55b138dbdb77a354f10f2</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>keyPressEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3ff4c255c761294742c2d94ef578cd78</anchor>
      <arglist>(QKeyEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>keyReleaseEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>6430573bd51a82cd32610ae715634158</anchor>
      <arglist>(QKeyEvent *_e)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>clearAll</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>f63331c0a8b58b0545f9a0b95f937aba</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>loadMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0866406ae55fbf42c2fad124bd5b4a0d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>addEmptyObjectMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5438bc34b7a0f408485363b00142cdad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>saveMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c5645d9c792b2f4c2b3e45894d0d6e4f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>saveToMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0d9ed8979a14d46a94d71e47acfba75d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>loadIniMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7c2d2506bed4659a430f4edf6a09230d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>saveIniMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e1b602945b3821a168f1aaba14afec2d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>recentOpen</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>72bb04b7007ec4babfa95fd89fc79b16</anchor>
      <arglist>(QAction *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>exit</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7ac4e29c46b9b55d5e92ba6bc559362b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>loadPlugin</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d95af47c8dc8c819077d002272b7e09b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>unloadPlugin</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d289f63b41c01f1b591dcfd06a5886dd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CoreWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7cd95ae6e498bd95ce83315aec5f0064</anchor>
      <arglist>(SeparatorNode *_rootNode, QVector&lt; ViewMode * &gt; &amp;_viewModes, std::vector&lt; PluginInfoT &gt; &amp;_plugins)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~CoreWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>290e466de203fe62471b9c633b1c4a56</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setupMenuBar</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c3db1cac7982fcf689c22f7823697bfd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRecent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>6dff9daea7bf60124911ce4a97a2802d</anchor>
      <arglist>(QString _filename, DataType _type)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateRecent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3f36a3308b6be434a8d13f52e61a73ce</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>ACG::QtWidgets::QtExaminerViewer *</type>
      <name>examiner_widget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>6788f311ef8cc94302882c8f55e97cc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QSplitter *</type>
      <name>splitter_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8dffa1c280af6ea85395f0e656d0c14e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QTextEdit *</type>
      <name>textedit_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>76e7e75db04d81afb52e01a1e3b23074</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>originalLoggerSize_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>b0686090a68170373b893dc13d4d54cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QMenu *</type>
      <name>recentFilesMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>527557ac59773ee467d53629937ffe18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QMenu *</type>
      <name>pluginsMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>84cc25b9c783a0947c3c72c16a2f6c46</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QMenu *</type>
      <name>helpMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d96dd4e322d38a6524611aab9fbbc1d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>closeEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7bc83ecda701b3c667f730706a9256fb</anchor>
      <arglist>(QCloseEvent *event)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLog</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a51e3d88efc42526907734abc9f0a178</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode *</type>
      <name>root_node_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7db67f2b8656c43a53eb81ded51f7fe3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; PluginInfoT &gt; &amp;</type>
      <name>plugins_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e5469938e0b68798c73c414934d64df7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolBar *</type>
      <name>viewerToolbar_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>25a5cfb2c398b27ace6d22420881b405</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FileInterface</name>
    <filename>classFileInterface.html</filename>
    <member kind="slot" virtualness="pure">
      <type>virtual int</type>
      <name>loadObject</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>0efbab4d230207cfa77fac0523add4eb</anchor>
      <arglist>(QString _filename)=0</arglist>
    </member>
    <member kind="slot" virtualness="pure">
      <type>virtual bool</type>
      <name>saveObject</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>cc96b2af7bbc2e1dc9b9a2f344523939</anchor>
      <arglist>(int _id, QString _filename)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>setObjectRoot</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>68ee5db86b63128c757892e699af71d9</anchor>
      <arglist>(BaseObject *_root)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>supportAddEmpty</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>d52e2c7a9d7e15a97c3472fa2918b07c</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual int</type>
      <name>addEmpty</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>59e29d9d4b046c74afc332379ebb85bf</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QString</type>
      <name>typeName</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>0e3683bd508b4bba507179b7c76406d7</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual DataType</type>
      <name>supportedType</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>dd81abf9277dae44167763da4371fb28</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QString</type>
      <name>getLoadFilters</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>804009a394ded7436b43c624af9cc44d</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QString</type>
      <name>getSaveFilters</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>86b79f14fef9913720c6456df43de7df</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QWidget *</type>
      <name>saveOptionsWidget</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>4e808d02f1c2e9d96e6af2cc506d24fb</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QWidget *</type>
      <name>loadOptionsWidget</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>7d8fabaef4e12549ce0e7867f1c68a19</anchor>
      <arglist>()=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GlobalAccessInterface</name>
    <filename>classGlobalAccessInterface.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~GlobalAccessInterface</name>
      <anchorfile>classGlobalAccessInterface.html</anchorfile>
      <anchor>7f146ce730efd2290ae9fd5f2fade0e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>set_examiner</name>
      <anchorfile>classGlobalAccessInterface.html</anchorfile>
      <anchor>0a5f6fde20190800f9f419b16c57d01f</anchor>
      <arglist>(ACG::QtWidgets::QtExaminerViewer *_examiner_widget)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GroupObject</name>
    <filename>classGroupObject.html</filename>
    <base>BaseObject</base>
    <member kind="function">
      <type></type>
      <name>GroupObject</name>
      <anchorfile>classGroupObject.html</anchorfile>
      <anchor>3d7118287515f4552fd3d4f19de9f58b</anchor>
      <arglist>(QString _groupName=&quot;Group&quot;, GroupObject *parent=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~GroupObject</name>
      <anchorfile>classGroupObject.html</anchorfile>
      <anchor>3c818f9d0e04b760e581461f8bcb08d2</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HandleNodeMod</name>
    <filename>structHandleNodeMod.html</filename>
    <templarg>MeshT</templarg>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_handle</name>
      <anchorfile>structHandleNodeMod.html</anchorfile>
      <anchor>56dfa797fe5b06a0c5685806b7a34fe2</anchor>
      <arglist>(const MeshT &amp;_m, typename MeshT::VertexHandle _vh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_handle</name>
      <anchorfile>structHandleNodeMod.html</anchorfile>
      <anchor>1a4feb8612b192fc091be04c4992e271</anchor>
      <arglist>(const MeshT &amp;_m, typename MeshT::FaceHandle _fh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_vertex_selected</name>
      <anchorfile>structHandleNodeMod.html</anchorfile>
      <anchor>bf2fe1c37b9809cf3fcb5a5cee828795</anchor>
      <arglist>(const MeshT &amp;_mesh, typename MeshT::VertexHandle _vh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_edge_selected</name>
      <anchorfile>structHandleNodeMod.html</anchorfile>
      <anchor>2ddf6d44df06b0921a262f629074806c</anchor>
      <arglist>(const MeshT &amp;_mesh, typename MeshT::EdgeHandle _eh)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>is_face_selected</name>
      <anchorfile>structHandleNodeMod.html</anchorfile>
      <anchor>3946df6b18a5659cab04991e5faebd21</anchor>
      <arglist>(const MeshT &amp;_mesh, typename MeshT::FaceHandle _fh)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>INIFile</name>
    <filename>classINIFile.html</filename>
    <member kind="typedef" protection="private">
      <type>std::map&lt; QString, QString &gt;</type>
      <name>EntryMap</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>46208a933fd6b08fb1d39274813c71eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef" protection="private">
      <type>std::map&lt; QString, EntryMap &gt;</type>
      <name>SectionMap</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>2ba609082fab70d0232fd17905e8389e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>m_filename</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>e85c7ee649f6932f77436e0a070d2a8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>mf_isConnected</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>8dbb5d020f9cbc1c58bab519f2fb1363</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SectionMap</type>
      <name>m_iniData</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>c6519b45be7fcd571d2e4e9e72584bcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>4b4ec384c81ffa2a8935eee28806ab81</anchor>
      <arglist>(QString &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>6c8ee038cb1048834fce004892e735ee</anchor>
      <arglist>(double &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>0507b5d79c7818fbecd773e81f818fd6</anchor>
      <arglist>(float &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>487bd8b3bb89eacfe85ffa52573cabaf</anchor>
      <arglist>(int &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>9fe9c8ca644520579082f394248d9d3e</anchor>
      <arglist>(unsigned int &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>cbdea05d8a48af62d69ef3c4e226531d</anchor>
      <arglist>(bool &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>5b9df639b76a8662dd22106cfd1a9cef</anchor>
      <arglist>(std::vector&lt; float &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>43086f65066983689eec88e2bb0f224c</anchor>
      <arglist>(std::vector&lt; double &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>0bad9b76260ebc13a1417afe0e06330b</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>6515b478e40ac29a37c7949a7334e9cf</anchor>
      <arglist>(std::vector&lt; QString &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>226b6a854a1f63bf21ec8e080601f16d</anchor>
      <arglist>(QStringList &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVecd</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>1312056ae5817694a53b6ca2f27cd3d9</anchor>
      <arglist>(VectorT &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVecf</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>de62b8f7c63d4a07c1a30d6a534444e8</anchor>
      <arglist>(VectorT &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVeci</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>52a1c05d00786aa38b0709bc9f0bc105</anchor>
      <arglist>(VectorT &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVecd</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>63eb09772b145391e29be8be84779608</anchor>
      <arglist>(std::vector&lt; VectorT &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVecf</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>437fd275829e5454464e88f4736b187c</anchor>
      <arglist>(std::vector&lt; VectorT &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_entryVeci</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>a1b32930ed7a1ca378a5b12147d770dd</anchor>
      <arglist>(std::vector&lt; VectorT &gt; &amp;_val, const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>parseFile</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>e68ffbd65e630a75534aa204d56d8130</anchor>
      <arglist>(QFile &amp;_inputStream)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>writeFile</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>2d83bd8abb62033fdac574845efcdf89</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>INIFile</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>278639bfc921feb2a20c0ced75d66dbc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~INIFile</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>a78854b355249d8b011e9de8026d19b2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>48eb806e36ccd4c847265f69dad313d7</anchor>
      <arglist>(const QString &amp;name, const bool create)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disconnect</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>7cea8921480878404645ab890eba0973</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_connected</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>ed4c2f2e6da5a31855037af8d0048062</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>const QString &amp;</type>
      <name>name</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>d9fc5b406708bd7a9a61e3ca14fbae5e</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>section_exists</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>246896bd897ea1e6a77ee5b42fb376bb</anchor>
      <arglist>(const QString &amp;_section) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>entry_exists</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>ac3a4bdb558c749d2109f9bc1cad72ce</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>entry_in_section</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>c9f82b98c7b155b1d2c8f3e924481645</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_section</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>b4c3d0fec0fc322ccd3366cbc7fe35db</anchor>
      <arglist>(const QString &amp;_sectionname)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>b3826958cf55d970330f57e65b65f9f2</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const QString &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>65f9de13c0d843fdec2ac949caccc776</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const char *_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>971a3f75bc38c7748b52f73cb9aed8f4</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const double &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>dcbb891299c43ad75f94c394af528808</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const float &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>6db961a5c6d40d16ed7d203e1f3ed95e</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const int &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>a6ddddea3638234879359d6f5d35e4b8</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const unsigned int &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>70647829b3c1fa36b2b8abf533329e7e</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const bool &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>20c42d31135ea3bcbdcc4b7ddc59ecc1</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const std::vector&lt; float &gt; &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>51fadb6d8bd7bc12475efa36de8bb46c</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const std::vector&lt; double &gt; &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entryVec</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>af07253288e7e4993d5d33a570078d14</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const VectorT &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entryVec</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>0d7b32846d185e04c199d3784ba94092</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const std::vector&lt; VectorT &gt; &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>9b868b7f1a7db8f4774b327f71b4b67b</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const std::vector&lt; int &gt; &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>7a421f5ecdf3d338bb1fc13b38c5c6f9</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const std::vector&lt; QString &gt; &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>787e3ed324bbb9951d4a8d9875dcc36e</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key, const QStringList &amp;_value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>delete_entry</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>a0c373517e3108631860806770c65f10</anchor>
      <arglist>(const QString &amp;_section, const QString &amp;_key)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>delete_section</name>
      <anchorfile>classINIFile.html</anchorfile>
      <anchor>f2b979dfbc0266dc59ea8939e1a7242c</anchor>
      <arglist>(const QString &amp;_sectionname)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>INIInterface</name>
    <filename>classINIInterface.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~INIInterface</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>192349b9a189aac0f9a2f7cf392789bc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>loadIniFile</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>66ebf39ccc11383a860dc8c10709fb1e</anchor>
      <arglist>(INIFile &amp;_ini, int _id)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>saveIniFile</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>4e249320fbfd6155523078c7b10de1ac</anchor>
      <arglist>(INIFile &amp;_ini, int _id)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>loadIniFileOptions</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>3b8ae0e16b554c4b96a6e2c6071dbebb</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>saveIniFileOptions</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>85c019af7f106311394d28d844679a59</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>KeyInterface</name>
    <filename>classKeyInterface.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~KeyInterface</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>6530d2ecdfa3538a0efc350974c24d9d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotKeyEvent</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>66e0b9cdc50addcbf20ac78b58e9bff7</anchor>
      <arglist>(QKeyEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotKeyReleaseEvent</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>94d9819293643c5abe9550f9dae8ba6a</anchor>
      <arglist>(QKeyEvent *_event)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoadSaveInterface</name>
    <filename>classLoadSaveInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>load</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>152541b680116d103397e2c2a92d049e</anchor>
      <arglist>(QString _filename, DataType _type, int &amp;_id)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addEmptyObject</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>088846b57b3ed0ec478afcda31b64596</anchor>
      <arglist>(DataType _type, int &amp;_id)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>save</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>b0ff1379c8613bfbac7ecd48e9456bef</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>openedFile</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>07293e70294182d866b9a24873824090</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>emptyObjectAdded</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>cc1d5d24bfe7c2af461becfcdd7927f9</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getAllFilters</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>64100c541bb1419dd4d999e8c6debedf</anchor>
      <arglist>(QStringList &amp;_list)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>fileOpened</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>715c23643892f4938aae8ea771ac6e3b</anchor>
      <arglist>(int _id)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoggingInterface</name>
    <filename>classLoggingInterface.html</filename>
    <member kind="signal" virtualness="pure">
      <type>virtual void</type>
      <name>log</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>7c755e4f595b6c64aa1c33910ff0cc6f</anchor>
      <arglist>(Logtype _type, QString _message)=0</arglist>
    </member>
    <member kind="signal" virtualness="pure">
      <type>virtual void</type>
      <name>log</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>06c95870c2614d5f54c6f15a26fb58d3</anchor>
      <arglist>(QString _message)=0</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>logOutput</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>eaed8f7c2a62569c7cecbc68ae12e63e</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MenuInfo</name>
    <filename>structMenuInfo.html</filename>
    <member kind="variable">
      <type>QMenu *</type>
      <name>menu</name>
      <anchorfile>structMenuInfo.html</anchorfile>
      <anchor>0b66f65e600de893ce60bd5c8d918e4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DataType</type>
      <name>contextType</name>
      <anchorfile>structMenuInfo.html</anchorfile>
      <anchor>aab6ff4f6bc63719d19a79d40e8146d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ContextMenuType</type>
      <name>position</name>
      <anchorfile>structMenuInfo.html</anchorfile>
      <anchor>864f8ed5eedfec823123194538b6ace1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MenuInterface</name>
    <filename>classMenuInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>initializeMenu</name>
      <anchorfile>classMenuInterface.html</anchorfile>
      <anchor>33ce7e316efe71d99194d0c8903f750c</anchor>
      <arglist>(QMenu *_menu, MenuType _type)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~MenuInterface</name>
      <anchorfile>classMenuInterface.html</anchorfile>
      <anchor>9691f35ec992aed2d68ea655f5dfeebf</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MeshObject</name>
    <filename>classMeshObject.html</filename>
    <templarg>MeshT</templarg>
    <templarg>objectDataType</templarg>
    <base>BaseObjectData</base>
    <member kind="variable" protection="private">
      <type>MeshT *</type>
      <name>mesh_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>4dd9be4f47fbb5d0ccac1187cc5f60ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>MeshT *</type>
      <name>mesh</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>15935652184030a463adfd10100f5e96</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>update</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>e3498d492e2afc629c30a85767eae400</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateSelection</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>04acbec78c5c8954092b056d57655f8d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateModelingRegions</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>f9b904b8a3cf404dbf5abfee43a970c1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateGeometry</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>402d0a14795ea3fe334093072e5f0c36</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateTopology</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>61000f44bc2ae8b49136dd2fce8ec9ea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::SelectionNodeT&lt; MeshT &gt; *</type>
      <name>statusNode_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>cda4b9f9eed1380fb60ff03b07957351</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::StatusNodeT&lt; MeshT, AreaNodeMod&lt; MeshT &gt; &gt; *</type>
      <name>areaNode_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>2ada01b0c43f8a36e9019c1e19fa59d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::StatusNodeT&lt; MeshT, HandleNodeMod&lt; MeshT &gt; &gt; *</type>
      <name>handleNode_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>c0516e0392c97ace7fafcbf1ac9f8679</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::MeshNodeT&lt; MeshT &gt; *</type>
      <name>meshNode_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>932a42e49511e9d468796d2e4ea3f843</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>TextureNode *</type>
      <name>textureNode_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>839df4b20340c62915fb7f61ea2d37b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>TextureNode *</type>
      <name>textureNode</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>3aabc5fbdd75b99f2ea0797f89feeef4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::SceneGraph::MeshNodeT&lt; MeshT &gt; *</type>
      <name>meshNode</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>e916689aee89b32af64a784ec5f50318</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>boundingBox</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>613a498d99bd52a3351d807d04530ff0</anchor>
      <arglist>(ACG::Vec3f &amp;_bbMin, typename ACG::Vec3f &amp;_bbMax)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>OMTriangleBSP *</type>
      <name>triangle_bsp_</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>64f05dc4a93210193be78aaa47968c5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>OpenMeshTriangleBSPT&lt; MeshT &gt;</type>
      <name>OMTriangleBSP</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>040327579fcbda65ffff44f89d6675d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>OMTriangleBSP *</type>
      <name>requestTriangleBsp</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>069477aa53958c84eba4c5f35319d17c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OMTriangleBSP *</type>
      <name>resetTriangleBsp</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>623969419d1d15c24956de8d96a71946</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::pair&lt; QString, QString &gt; &gt;</type>
      <name>textures</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>79554c9d41edae8fd5866106a467e559</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addTexture</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>b9e5ce02ba1e5454145a0f53299e31e5</anchor>
      <arglist>(QString _property, QString _textureFile)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MeshObject</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>7c1a8ef51a7cd328ff7995d509276ba0</anchor>
      <arglist>(SeparatorNode *_rootNode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~MeshObject</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>4f512ad4dc699b6d829527545167b6d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanup</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>c07a0d3cb0ed3cbebbc8f32a4811046e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>1bffa16684d9d8612dc2cfe12cad144f</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>loadMesh</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>353794a7196065e4c31948275934499e</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>getObjectinfo</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>1b37e004a88ffb148a7b845778ef3046</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>picked</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>8968ca376aef5ca76bb59bced701bb19</anchor>
      <arglist>(uint _node_idx)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>init</name>
      <anchorfile>classMeshObject.html</anchorfile>
      <anchor>78ed91b444cb303155201f9418fc0ec4</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MouseInterface</name>
    <filename>classMouseInterface.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~MouseInterface</name>
      <anchorfile>classMouseInterface.html</anchorfile>
      <anchor>9adc16b9a1de1cd62d1a8be615576134</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotMouseWheelEvent</name>
      <anchorfile>classMouseInterface.html</anchorfile>
      <anchor>a421d325c2298c063609ab657fe3a528</anchor>
      <arglist>(QWheelEvent *_event, const std::string &amp;_mode)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotMouseEvent</name>
      <anchorfile>classMouseInterface.html</anchorfile>
      <anchor>d998de7699f7f6ea0e60de3ba5a5503d</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotMouseEventIdentify</name>
      <anchorfile>classMouseInterface.html</anchorfile>
      <anchor>aa410150a78e9d6f16501d45ff4f7bdf</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ObjectIDPointerManagerT</name>
    <filename>classObjectIDPointerManagerT.html</filename>
    <templarg>Pointer</templarg>
    <member kind="typedef">
      <type>std::map&lt; int, Pointer &gt;</type>
      <name>PointerObjectIDMap</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>9e35bb192095abb5982b64270506e2ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::pair&lt; int, Pointer &gt;</type>
      <name>PointerObjectIDPair</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>77d0163b2f9a76a84699b0180cf7625e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ObjectIDPointerManagerT</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>9dae0b228894750b3f2af4f3ed2cabbf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ObjectIDPointerManagerT</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>dc879ddcb403a5be2e1baf755cfc4360</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_pointer</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>6c7a983c0d3b1f0164983625c104061d</anchor>
      <arglist>(int _identifier, Pointer &amp;_pointer)</arglist>
    </member>
    <member kind="function">
      <type>Pointer</type>
      <name>get_pointer</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>78cf36225f169b7dd942708a8ec333d4</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>add_pointer</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>a944b766aa7a99a188ff7e1239117eb0</anchor>
      <arglist>(int _identifier, const Pointer &amp;_pointer)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>delete_object</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>1fe64ca08678ed09196659e72a3090dd</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>ObjectIDPointerManagerT</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>499f58e8a29842af9f138390593a9caa</anchor>
      <arglist>(const ObjectIDPointerManagerT &amp;_rhs)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>ObjectIDPointerManagerT &amp;</type>
      <name>operator=</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>82cb3e91ffb1bdd007e8eeb516cfa34d</anchor>
      <arglist>(const ObjectIDPointerManagerT &amp;_rhs)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>PointerObjectIDMap</type>
      <name>pointer_objectID_map_</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>f5368a354ca128534835eb56080907b2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PickingInterface</name>
    <filename>classPickingInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>a20f3591f19b492040c5a637c0840853</anchor>
      <arglist>(const std::string _mode)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addHiddenPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>4f8958a5dce7769e461f290021fc2492</anchor>
      <arglist>(const std::string _mode)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>90c9a345d878a5f5b6c690624c2738c1</anchor>
      <arglist>(const std::string _mode, QCursor _cursor)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addHiddenPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>7827061c000690ab805d0fb28d6ac83a</anchor>
      <arglist>(const std::string _mode, QCursor _cursor)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>slotPickModeChanged</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>5bf0bf6559d2b5411d0f3270415242c1</anchor>
      <arglist>(const std::string &amp;_mode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~PickingInterface</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>e6db7c7b173ae542e477d0de9594ed68</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PluginInfoT</name>
    <filename>structPluginInfoT.html</filename>
    <member kind="variable">
      <type>QObject *</type>
      <name>plugin</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>049ed3271ccd78b9e54ae71ce3a26cef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>name</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>1979cd1bb26aeedfafe6def9ccfc81a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>description</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>42d60b2a6cd727f3bd6a166f40a896c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>path</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>ee8b616bd31ad545455671b177a90999</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>rpcName</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>c8848b81b6a5e25bdab3cfca9d06fcae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QStringList</type>
      <name>rpcFunctions</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>ef6fc0f76d4f82dba44e53b1d460d339</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QDockWidget *</type>
      <name>widget</name>
      <anchorfile>structPluginInfoT.html</anchorfile>
      <anchor>f326c76edcff0cfa65a938384ff5891d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginLogger</name>
    <filename>classPluginLogger.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>log</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>21aa22c478ca3296f6ceff770d965ccb</anchor>
      <arglist>(Logtype, QString)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PluginLogger</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>91b1f262e2dee20a38efa468c11de821</anchor>
      <arglist>(QString _pluginName, Logtype _type=LOGOUT)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~PluginLogger</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>c30ea56abf41b2ea983cbf05534ed6fd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>2fc78484a7a990935e32cccc67652fb2</anchor>
      <arglist>(const std::string &amp;_s)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLog</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>7829c7ee1069c99dee47ac262e88c9e7</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLog</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>f3d00762f1ee810e0310ffa40dbed3c8</anchor>
      <arglist>(QString _message)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>pluginName_</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>524c7787e9a5413ed0104569ef7c784c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>Logtype</type>
      <name>defaultLogType_</name>
      <anchorfile>classPluginLogger.html</anchorfile>
      <anchor>eba9b69cf6ef4cbdaf3ef3c536cf9b9c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PolyLineObject</name>
    <filename>classPolyLineObject.html</filename>
    <base>BaseObjectData</base>
    <member kind="variable" protection="private">
      <type>PolyLine *</type>
      <name>line_</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>95914f4d59050f00ad0a9a5532026de7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>PolyLine *</type>
      <name>line</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>d29dbf7491edf76a7203ee3bc511d427</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::PolyLineNodeT&lt; PolyLine &gt; *</type>
      <name>lineNode_</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>a8da673952e666a9650676c1fcf7156a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>ACG::SceneGraph::PolyLineNodeT&lt; PolyLine &gt; *</type>
      <name>lineNode</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>c4ba3b255efd8b6f1f78cca2604362ce</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PolyLineObject</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>2d054a4c33458fa07005b0bc5945fecf</anchor>
      <arglist>(SeparatorNode *_rootNode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~PolyLineObject</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>639a993e9faf7bf7fd5bd5680c54be4b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanup</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>ef0957efb375104aec9f2429b40d1205</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setName</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>941b6a6f21d58a82817f5920c3e19926</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>getObjectinfo</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>6db45f20226b95e2704a6ec27ee2c93a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>picked</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>e7d2642deeb351e73e164462f46a47c6</anchor>
      <arglist>(uint _node_idx)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>init</name>
      <anchorfile>classPolyLineObject.html</anchorfile>
      <anchor>f78accacad0697b2b622300e7d36bab5</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PolyTraits</name>
    <filename>structPolyTraits.html</filename>
    <member kind="typedef">
      <type>OpenMesh::Vec3d</type>
      <name>Point</name>
      <anchorfile>structPolyTraits.html</anchorfile>
      <anchor>a4416f575cc8eb72fa9dccebd2cea40b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>OpenMesh::Vec3d</type>
      <name>Normal</name>
      <anchorfile>structPolyTraits.html</anchorfile>
      <anchor>78aa2f9ec459e136f5a0e408f00cb4da</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RPCInterface</name>
    <filename>classRPCInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>pluginExists</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>5db569e86875af8d34f4270476f091f3</anchor>
      <arglist>(QString _pluginName, bool &amp;_exists)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>functionExists</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>622cd36167ca99bf7f96b7fe407410a0</anchor>
      <arglist>(QString _pluginName, QString _functionName, bool &amp;_exists)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>call</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>89509c17f133786937d55fc17ce728a3</anchor>
      <arglist>(QString _pluginName, QString _functionName, bool &amp;_success)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>call</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>cb12c852cdc634d1558ca5326cd46bfe</anchor>
      <arglist>(QString _expression, bool &amp;_success)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ScriptingWrapper</name>
    <filename>classScriptingWrapper.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>scriptInfo</name>
      <anchorfile>classScriptingWrapper.html</anchorfile>
      <anchor>6971d0114158f72f27986ab7c4d08208</anchor>
      <arglist>(QString _pluginName, QString _functionName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ScriptingWrapper</name>
      <anchorfile>classScriptingWrapper.html</anchorfile>
      <anchor>b32fa9af2eafd505858941a4d99febcb</anchor>
      <arglist>(QString _pluginName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ScriptingWrapper</name>
      <anchorfile>classScriptingWrapper.html</anchorfile>
      <anchor>5e2c467275b66110fa3e42dcd5ce8bde</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotScriptInfo</name>
      <anchorfile>classScriptingWrapper.html</anchorfile>
      <anchor>1b9b9c471d09e266afa1c6d4e480d2ce</anchor>
      <arglist>(QString _functionName)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QString</type>
      <name>pluginName_</name>
      <anchorfile>classScriptingWrapper.html</anchorfile>
      <anchor>e91868a5cd863ac5d7eb6ced9799b0e4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ScriptInterface</name>
    <filename>classScriptInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>scriptInfo</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>ac87b748add6c4b7961d73b8bdb032f8</anchor>
      <arglist>(QString _functionName)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>executeScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>70522aa5df14916fa00e3747021d27bd</anchor>
      <arglist>(QString _script)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getScriptingEngine</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>44edc5563856d3d605d78f58c4c2e94a</anchor>
      <arglist>(QScriptEngine *&amp;_engine)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getAvailableFunctions</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>356df2d1b6bcd96b588c475d7d2d27bf</anchor>
      <arglist>(QStringList &amp;_functions)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotScriptInfo</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>3b389cf3a3da8f04e01b3a2c2d887177</anchor>
      <arglist>(QString _pluginName, QString _functionName)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotExecuteScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>c99bbe03eaea82d99ae6f32d13ecec96</anchor>
      <arglist>(QString _script)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotExecuteFileScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>9eae0f44005a7e4eb49d72b83336f4eb</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TextureInterface</name>
    <filename>classTextureInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>b0806792054ae243388c1be00c1348c6</anchor>
      <arglist>(QString _name, QString _filename, uint _dimension)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>823da66c85a5d986addb783ef279cef0</anchor>
      <arglist>(QString _textureName, int _identifier)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateAllTextures</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>fe1b4dbd7b5c9bcb615734168796d0de</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updatedTextures</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>862d7c31117a668195f14a09ba8efaf8</anchor>
      <arglist>(QString, int)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>switchTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>7f19f66b66f842fc4c3266579376756d</anchor>
      <arglist>(QString)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>setTextureMode</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>bd95929bad975b9f792fc1a1ec1c1fb6</anchor>
      <arglist>(QString _textureName, QString _mode)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~TextureInterface</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>ef1718df41e63084af42c37f270c5dcc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotUpdateTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>4cdfc4a94990fd274fcfcdd8df416095</anchor>
      <arglist>(QString _textureName, int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotUpdateAllTextures</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>48b8d7f5899a91d7961595c7f5b2e212</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotTextureAdded</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>4ee502da91b8593fd70ddd38c3fd4458</anchor>
      <arglist>(QString _textureName, QString _filename, uint dimension)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotTextureUpdated</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>dcda2f55fd606faf2177dbbaeef05ebd</anchor>
      <arglist>(QString _textureName, int _identifier)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotSetTextureMode</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>620c99584eb81755830e6fd0aaa9bad1</anchor>
      <arglist>(QString _textureName, QString _mode)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotSwitchTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>49fefe07bc881d96bb6a5e85607c8f82</anchor>
      <arglist>(QString _textureName)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ToolbarInterface</name>
    <filename>classToolbarInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addToolbar</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>f3a0622e6323797c2eb0b29501e379d3</anchor>
      <arglist>(QToolBar *_toolbar)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>removeToolbar</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>1cd064609f92063e8bdd6e823710bce5</anchor>
      <arglist>(QToolBar *_toolbar)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ToolbarInterface</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>f72b2768e473608cbda7f62299953d70</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ToolboxInterface</name>
    <filename>classToolboxInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>defineViewMode</name>
      <anchorfile>classToolboxInterface.html</anchorfile>
      <anchor>7ebf36020de161c87b204b309108b602</anchor>
      <arglist>(QString &amp;_mode, QStringList &amp;_usedWidgets)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ToolboxInterface</name>
      <anchorfile>classToolboxInterface.html</anchorfile>
      <anchor>0f2acdcde4c317977aa2a9930e7c34d6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>initializeToolbox</name>
      <anchorfile>classToolboxInterface.html</anchorfile>
      <anchor>9c874f09fb596bcd94f45bcb91732bc9</anchor>
      <arglist>(QWidget *&amp;_widget)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>toolboxActivated</name>
      <anchorfile>classToolboxInterface.html</anchorfile>
      <anchor>64aea64085c2bfe4b7098baf088cf0d1</anchor>
      <arglist>(bool _active)=0</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TriTraits</name>
    <filename>structTriTraits.html</filename>
    <member kind="typedef">
      <type>OpenMesh::Vec3d</type>
      <name>Point</name>
      <anchorfile>structTriTraits.html</anchorfile>
      <anchor>c40091204d816d1c3a1218477dd485c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>OpenMesh::Vec3d</type>
      <name>Normal</name>
      <anchorfile>structTriTraits.html</anchorfile>
      <anchor>9bf69372c8c333e445c62d695058e195</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ViewInterface</name>
    <filename>classViewInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>abb3bc612aa1f471208639cbd285e472</anchor>
      <arglist>(QString _name, QWidget *&amp;_widget)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>f732ea6b30ea27a65d7fe8c3b2738009</anchor>
      <arglist>(QString _name, QWidget *_widget)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>1855a401f450afb4c41a80e04d32b5c9</anchor>
      <arglist>(QString _name, QWidget *_widget)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ViewInterface</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>a68b666618c52c289cdebe88c8f3ac87</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OpenFlipper::Options::RecentFile</name>
    <filename>structOpenFlipper_1_1Options_1_1RecentFile.html</filename>
    <member kind="variable">
      <type>QString</type>
      <name>filename</name>
      <anchorfile>structOpenFlipper_1_1Options_1_1RecentFile.html</anchorfile>
      <anchor>d383068527dc065682a29bc10dba7e7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>DataType</type>
      <name>type</name>
      <anchorfile>structOpenFlipper_1_1Options_1_1RecentFile.html</anchorfile>
      <anchor>0bb3d4cd5f1f4ff94de286f2fd920936</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>PluginFunctions</name>
    <filename>namespacePluginFunctions.html</filename>
    <class kind="class">PluginFunctions::ObjectIterator</class>
    <member kind="function">
      <type>ObjectIterator</type>
      <name>objects_end</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>438f2fcd4d8a661dfc2d1dc880d86daf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>get_all_objects</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>8c64011c52e292c7b1aeb1e36b772561</anchor>
      <arglist>(std::vector&lt; BaseObjectData * &gt; &amp;_objects)</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *</type>
      <name>objectRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bf0ed8a1c2b14f55daf757184ba4688c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>61a339afd69b4a5dd217498a088a78f9</anchor>
      <arglist>(int _identifier, BSplineCurveObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT BSplineCurve *</type>
      <name>splineCurve</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>aad1f17d9abdd7cf4ddb2ee1bd9f68b1</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT BSplineCurveObject *</type>
      <name>bsplineCurveObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>db8b914a5ff8d51712fce5b738b9f975</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>84bcca06e54f5ee597175cbf8e1cb52d</anchor>
      <arglist>(int _identifier, PolyLineObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT PolyLine *</type>
      <name>polyLine</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fcde5199276ba3ea227f6e43ad67e111</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT PolyLineObject *</type>
      <name>polyLineObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>07563d23b56254bfdf9c301774f86f9a</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDataRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>500a4c2660d974cc5d9ab8ea0604a5ff</anchor>
      <arglist>(BaseObject *_root)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_examiner</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>4aa9ac417f6ae18b0b11e4e70576ed4a</anchor>
      <arglist>(ACG::QtWidgets::QtExaminerViewer *examiner_widget_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_rootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>331db522180614b2e44387ed3f4994e4</anchor>
      <arglist>(SeparatorNode *_root_node)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d14a50aa769da67743500faa66af8687</anchor>
      <arglist>(ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>95bf07301b5f4e367e9d6909acbaf7fd</anchor>
      <arglist>(ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f6afeffa7e67c557c2d7a3c77e894018</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>e859622da656e25418714d1f05dd34c5</anchor>
      <arglist>(std::string _mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>24821d26bcab4ddbc6c13f7c7f11a997</anchor>
      <arglist>(ACG::QtWidgets::QtBaseViewer::ActionMode _mode)</arglist>
    </member>
    <member kind="function">
      <type>ACG::GLState &amp;</type>
      <name>glState</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>42afb52dcbd4fef6986d372f2f8118f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getCurrentViewImage</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>074bac06a5e27d760728aed1b2b3f0ee</anchor>
      <arglist>(QImage &amp;_image)</arglist>
    </member>
    <member kind="function">
      <type>ACG::QtWidgets::QtBaseViewer::ActionMode</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>45995def10b964ecb9a43b2f79bd52f4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>allowRotation</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bbc156ec199665bf2ac229741e8ca586</anchor>
      <arglist>(bool _mode)</arglist>
    </member>
    <member kind="function">
      <type>ACG::SceneGraph::BaseNode *</type>
      <name>getRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>46596260d8ff9904342ff483f9c19591</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5195874eac70f82dc7e8aeb316b1617f</anchor>
      <arglist>(std::vector&lt; TriMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>243b3c32189a1471dda04f7f25c485c3</anchor>
      <arglist>(std::vector&lt; PolyMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b4358af396ca69b2e3029e7f2c46d649</anchor>
      <arglist>(std::vector&lt; TriMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a057a222ce91dd201b545d65cc91f737</anchor>
      <arglist>(std::vector&lt; PolyMesh * &gt; &amp;_meshes)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_picked_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ef47f6d2b83b68e6587397646220d91a</anchor>
      <arglist>(uint _node_idx, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b1e3be5d4c7fa2d14535cc0187f1b41</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b045e2632c7378cc2e0150066233b47</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_source_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>585008c20534d0ac70ffd428bdbb14ef</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_target_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7845369871c9fb0596dbc1c52aaf45c1</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1091d2449e9d33118bfab6c1e25db63d</anchor>
      <arglist>(int _identifier, BaseObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>3013a288f3318f8718676b06f1921264</anchor>
      <arglist>(int _identifier, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5d62a4329854f98844abb898c042ba3c</anchor>
      <arglist>(int _identifier, TriMeshObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_mesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1172644f56bfb3f5e7ea2221f7501ff9</anchor>
      <arglist>(int _identifier, TriMesh *&amp;_mesh)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_mesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>edb3fc3ac2615a96f6ad631f4d70d892</anchor>
      <arglist>(int _identifier, PolyMesh *&amp;_mesh)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>object_exists</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>dacf0ec50d3ec996ec2edfaf3d95f961</anchor>
      <arglist>(int _identifier)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_all_meshes</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ca145e68854ad4b3be4fec112737e08c</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_all_object_identifiers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c42895597dc095942768517d7aa70cf4</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_identifiers)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>object_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7c8f86352f3912f2d11f94e00d9fb2c6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>target_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>45a2037e6dfdbed46d38bb95b22a947c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>source_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>48e21304e2102dbb519d71e644fbb453</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>visible_count</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>211e3474ca5477b24af33f3cf0afe654</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBackColor</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>37a6e72ec175b6ae536f99aa0ca38f61</anchor>
      <arglist>(OpenMesh::Vec4f _color)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDrawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5676bf850581cb61763b7829ae28a508</anchor>
      <arglist>(unsigned int _mode)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>drawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>43d94fd55d1a0e22c5c9cba49c52c66b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>perspectiveProjection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7ec716d6a176219b3937994a91ac169a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>orthographicProjection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>642fd4181e6702ade84257495b56f9c2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewingDirection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>30c72bddf5bcdd6b39abe24e6b8267a7</anchor>
      <arglist>(const ACG::Vec3d &amp;_dir, const ACG::Vec3d &amp;_up)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setScenePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d129d117d838752a75498affc57cc1ac</anchor>
      <arglist>(const ACG::Vec3d &amp;_center, double _radius)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setScenePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a6fa94d1d14e272693d9d12138fb88c2</anchor>
      <arglist>(const ACG::Vec3d &amp;_center)</arglist>
    </member>
    <member kind="function">
      <type>const ACG::Vec3d &amp;</type>
      <name>sceneCenter</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f8548472c36e305d995dbd7531a8d1e0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sceneRadius</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>259318bab04fb94a356fa923384f36bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>translate</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d53f8c6306496ee49021ae68d8873fc7</anchor>
      <arglist>(const ACG::Vec3d &amp;_vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rotate</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9591fe3955d130d450afe9e8924fc521</anchor>
      <arglist>(const ACG::Vec3d &amp;_axis, double _angle, const ACG::Vec3d &amp;_center)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewHome</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9ad0d550a7d8849130166a8b0c8470d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c4811a358a7fcd79f1b8ea2b6fdfb8e3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>viewingDirection</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>74db7476ef45e6a94c2a3556ca362551</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>eyePos</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c7b81abe92e8f2e4f5d07c6b6ba48f46</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ACG::Vec3d</type>
      <name>upVector</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>67be45b4919eb692a281fd7ee1ca4a1e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>flyTo</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>25dd779116f4a9a352c5abc1ff9d86ed</anchor>
      <arglist>(const TriMesh::Point &amp;_position, const TriMesh::Point &amp;_center, double _time)</arglist>
    </member>
    <member kind="function">
      <type>BaseObjectData *</type>
      <name>baseObjectData</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ffb238a514f9754ca2a3680ccb4b59a4</anchor>
      <arglist>(BaseObject *_object)</arglist>
    </member>
    <member kind="function">
      <type>TriMesh *</type>
      <name>triMesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>11d22dc1c64537f87c014241cacc4340</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>PolyMesh *</type>
      <name>polyMesh</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fe48c8dedf9bf541678277213548d61a</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>TriMeshObject *</type>
      <name>triMeshObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1f03569cd9ea2c79069f362b84c42661</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>PolyMeshObject *</type>
      <name>polyMeshObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>dbb1912ef5640b57a886731049f21413</anchor>
      <arglist>(BaseObjectData *_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>86ad67d19dfba2f1e2eabb77b1845435</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::VPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>4e419d2ed41704bc74e90912ad409e9f</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::FPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_property_handle</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>fcb31fc379e793806451080b35dc5fdf</anchor>
      <arglist>(MeshT *_mesh, QString _name, OpenMesh::HPropHandleT&lt; propT &gt; &amp;_property)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static BaseObject *</type>
      <name>objectRoot_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1becdbeb1c22e6b1a271ddb2226d0fe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static ACG::QtWidgets::QtExaminerViewer *</type>
      <name>examiner_widget_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>3fe438cdfeb912e1a2987c051c5c6bb3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static SeparatorNode *</type>
      <name>root_node_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f38b951861d0228b1a05909dff038ede</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginFunctions::ObjectIterator</name>
    <filename>classPluginFunctions_1_1ObjectIterator.html</filename>
    <member kind="typedef">
      <type>BaseObjectData</type>
      <name>value_type</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>53cb0ee7b60b82141c865eeccde1fd35</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>BaseObjectData *</type>
      <name>value_handle</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>35380acf0e46b697358c85c4657bf1ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>value_type &amp;</type>
      <name>reference</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>20b3789e0a2258c739cb2bdd7a4fc119</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>value_type *</type>
      <name>pointer</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>7069b9ce3ae31f454f7e40c5c592c608</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ObjectIterator</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>54d100ee050b2f18d53a64bcbdcd8a41</anchor>
      <arglist>(IteratorRestriction _restriction=ALL_OBJECTS, DataType _dataType=DATA_ALL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ObjectIterator</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>9f59ed2b053a6772993a1728740e2363</anchor>
      <arglist>(BaseObjectData *pos, IteratorRestriction _restriction=ALL_OBJECTS, DataType _dataType=DATA_ALL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>operator value_handle</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>674ccdb66799d8c2daa73b64f541baad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>cc5903ae80dd04752a56b2e6f660429a</anchor>
      <arglist>(const ObjectIterator &amp;_rhs)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>be2a988bcab6affd570d0a6bacaa2883</anchor>
      <arglist>(const ObjectIterator &amp;_rhs)</arglist>
    </member>
    <member kind="function">
      <type>ObjectIterator &amp;</type>
      <name>operator=</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>9994d19c795e070791c1b2f151e43e20</anchor>
      <arglist>(const ObjectIterator &amp;_rhs)</arglist>
    </member>
    <member kind="function">
      <type>pointer</type>
      <name>operator-&gt;</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>751dc91bfaec0bbbaa25a8d7a40a9beb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ObjectIterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>5bb5bee9217a64dea909fa217feb1c11</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ObjectIterator &amp;</type>
      <name>operator--</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>7bdd5294938f3ee7b953477dbc7e09b4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>BaseObjectData *</type>
      <name>operator*</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>cc46efe403f3746b09058510aebefdd8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>BaseObjectData *</type>
      <name>index</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>c08bdae296f4b09334dab82649987a86</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BaseObjectData *</type>
      <name>pos_</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>5916b78491ac48c7a312d881f6fce735</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>DataType</type>
      <name>dataType_</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>b42108268d3aa2198242950eeec446fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>IteratorRestriction</type>
      <name>restriction_</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>c9865213be6b7fa872c4a5d118e19286</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="dir">
    <name>ACGHelper/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/ACGHelper/</path>
    <filename>dir_6afef1ac87ce4b3c7cce84489a851a3f.html</filename>
    <file>DrawModeConverter.cc</file>
    <file>DrawModeConverter.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/addEmptyWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/addEmptyWidget/</path>
    <filename>dir_9cc39214bb3808d91bd8644300e632f2.html</filename>
    <file>addEmptyWidget.cc</file>
    <file>addEmptyWidget.hh</file>
    <file>ui_addEmpty.hh</file>
  </compound>
  <compound kind="dir">
    <name>BasePlugin/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/BasePlugin/</path>
    <filename>dir_6b745c6e48e3706a2492895c09ed01dc.html</filename>
    <file>BackupInterface.hh</file>
    <file>BaseInterface.hh</file>
    <file>ContextMenuInterface.hh</file>
    <file>FileInterface.hh</file>
    <file>GlobalAccessInterface.hh</file>
    <file>INIInterface.hh</file>
    <file>KeyInterface.hh</file>
    <file>LoadSaveInterface.hh</file>
    <file>LoggingInterface.hh</file>
    <file>MenuInterface.hh</file>
    <file>MouseInterface.hh</file>
    <file>PickingInterface.hh</file>
    <file>PluginFunctions.cc</file>
    <file>PluginFunctions.hh</file>
    <file>PluginFunctionsBSplineCurve.cc</file>
    <file>PluginFunctionsBSplineCurve.hh</file>
    <file>PluginFunctionsPolyLine.cc</file>
    <file>PluginFunctionsPolyLine.hh</file>
    <file>PluginFunctionsT.cc</file>
    <file>RPCInterface.hh</file>
    <file>ScriptInterface.hh</file>
    <file>TextureInterface.hh</file>
    <file>ToolbarInterface.hh</file>
    <file>ToolboxInterface.hh</file>
    <file>ViewInterface.hh</file>
  </compound>
  <compound kind="dir">
    <name>common/bsp/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/bsp/</path>
    <filename>dir_54e407fa1cbccffdd1634e73a25a0d1a.html</filename>
    <file>BSPImplT.cc</file>
    <file>BSPImplT.hh</file>
    <file>TriangleBSPCoreT.cc</file>
    <file>TriangleBSPCoreT.hh</file>
    <file>TriangleBSPT.hh</file>
  </compound>
  <compound kind="dir">
    <name>common/BSplineCurve/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/BSplineCurve/</path>
    <filename>dir_d595ab3ccede140d797f2418d1ee28e4.html</filename>
    <file>BSplineCurveObject.cc</file>
    <file>BSplineCurveObject.hh</file>
    <file>BSplineCurveTypes.hh</file>
  </compound>
  <compound kind="dir">
    <name>common/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>dir_1be831ad6813636322aff3cfbb793ba7.html</filename>
    <dir>common/bsp/</dir>
    <dir>common/BSplineCurve/</dir>
    <dir>common/PolyLine/</dir>
    <file>BaseObject.cc</file>
    <file>BaseObject.hh</file>
    <file>BaseObjectData.cc</file>
    <file>BaseObjectData.hh</file>
    <file>BaseObjectDataT.cc</file>
    <file>GlobalDefines.hh</file>
    <file>GlobalOptions.cc</file>
    <file>GlobalOptions.hh</file>
    <file>GroupObject.cc</file>
    <file>GroupObject.hh</file>
    <file>MeshObjectT.cc</file>
    <file>MeshObjectT.hh</file>
    <file>ObjectIDPointerManagerT.cc</file>
    <file>ObjectIDPointerManagerT.hh</file>
    <file>PolyMeshTypes.hh</file>
    <file>StatusNodeMods.hh</file>
    <file>TriangleMeshTypes.hh</file>
    <file>Types.hh</file>
  </compound>
  <compound kind="dir">
    <name>Core/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Core/</path>
    <filename>dir_e320c60241f3f15353bb7cad138eff61.html</filename>
    <file>Core.cc</file>
    <file>Core.hh</file>
    <file>Logging.cc</file>
    <file>openFunctions.cc</file>
    <file>optionHandling.cc</file>
    <file>ParseIni.cc</file>
    <file>PluginCommunication.cc</file>
    <file>PluginLoader.cc</file>
    <file>RPC.cc</file>
    <file>saveFunctions.cc</file>
    <file>scripting.cc</file>
  </compound>
  <compound kind="dir">
    <name>widgets/coreWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/coreWidget/</path>
    <filename>dir_56a69f0fbb49fda25c91167fa202740d.html</filename>
    <file>ContextMenu.cc</file>
    <file>CoreWidget.cc</file>
    <file>CoreWidget.hh</file>
    <file>CoreWidgetLogging.cc</file>
    <file>CoreWidgetToolbar.cc</file>
    <file>Help.cc</file>
    <file>MenuBar.cc</file>
    <file>StackWidget.cc</file>
    <file>viewMode.cc</file>
  </compound>
  <compound kind="dir">
    <name>Doxygen/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Doxygen/</path>
    <filename>dir_15ee26eb48dc46c47c455f7f6deafd0c.html</filename>
    <file>dataStructure.docu</file>
    <file>Interfaces.docu</file>
    <file>mainpage.docu</file>
    <file>PluginProgramming.docu</file>
  </compound>
  <compound kind="dir">
    <name>widgets/helpBrowser/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/helpBrowser/</path>
    <filename>dir_8b62dc7f51b1e3f1ac6e79adf1dca1fc.html</filename>
    <file>helpWidget.cc</file>
    <file>helpWidget.hh</file>
    <file>ui_helpWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>INIFile/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/INIFile/</path>
    <filename>dir_0c04a1a7503982aacfc85ab933f0bdc1.html</filename>
    <file>INIFile.cc</file>
    <file>INIFile.hh</file>
    <file>INIFileT.cc</file>
  </compound>
  <compound kind="dir">
    <name>widgets/loadWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/loadWidget/</path>
    <filename>dir_9fc3c5f491b01d4b75ebd1f8e39005ba.html</filename>
    <file>loadWidget.cc</file>
    <file>loadWidget.hh</file>
    <file>ui_load.hh</file>
  </compound>
  <compound kind="dir">
    <name>Logging/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Logging/</path>
    <filename>dir_b907f522836bf04bb84548b61670133c.html</filename>
    <file>Logging.hh</file>
    <file>PluginLogging.cc</file>
  </compound>
  <compound kind="dir">
    <name>common/PolyLine/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/PolyLine/</path>
    <filename>dir_8c42b1ab889f9bcfbe75b979af966eef.html</filename>
    <file>PolyLineObject.cc</file>
    <file>PolyLineObject.hh</file>
    <file>PolyLineTypes.hh</file>
  </compound>
  <compound kind="dir">
    <name>Scripting/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Scripting/</path>
    <filename>dir_c97075f2720219032fccff1b8bdcfcd5.html</filename>
    <dir>Scripting/scriptPrototypes/</dir>
    <dir>Scripting/scriptWrappers/</dir>
    <file>Scripting.hh</file>
    <file>ScriptingWrapper.cc</file>
  </compound>
  <compound kind="dir">
    <name>Scripting/scriptPrototypes/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Scripting/scriptPrototypes/</path>
    <filename>dir_7f818d5a9e258b6b503d930d08efa4e9.html</filename>
    <file>prototypeMatrix4x4.cc</file>
    <file>prototypeMatrix4x4.hh</file>
    <file>prototypeVec3d.cc</file>
    <file>prototypeVec3d.hh</file>
  </compound>
  <compound kind="dir">
    <name>Scripting/scriptWrappers/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Scripting/scriptWrappers/</path>
    <filename>dir_08300fed192bcb66f6137dbe84f57711.html</filename>
    <file>matrix4x4Wrapper.cc</file>
    <file>matrix4x4Wrapper.hh</file>
    <file>vec3dWrapper.cc</file>
    <file>vec3dWrapper.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/unloadPluginsWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/unloadPluginsWidget/</path>
    <filename>dir_3537f2718c44bf5387a0c1cd04e8438d.html</filename>
    <file>ui_unloadPlugins.hh</file>
    <file>unloadPluginsWidget.cc</file>
    <file>unloadPluginsWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/viewModeWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/viewModeWidget/</path>
    <filename>dir_d90a30b42ac5271b0fe6b95a4c309e07.html</filename>
    <file>ui_viewMode.hh</file>
    <file>viewModeWidget.cc</file>
    <file>viewModeWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/</path>
    <filename>dir_7ca3df6b42838197b5d9bc81f4b4838a.html</filename>
    <dir>widgets/addEmptyWidget/</dir>
    <dir>widgets/coreWidget/</dir>
    <dir>widgets/helpBrowser/</dir>
    <dir>widgets/loadWidget/</dir>
    <dir>widgets/unloadPluginsWidget/</dir>
    <dir>widgets/viewModeWidget/</dir>
    <file>.kdbgrc.PluginCommunication.cc</file>
  </compound>
</tagfile>
