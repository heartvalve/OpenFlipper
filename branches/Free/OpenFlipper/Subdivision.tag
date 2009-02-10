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
    <includes id="ViewerProperties_8hh" name="ViewerProperties.hh" local="no" imported="no">OpenFlipper/common/ViewerProperties.hh</includes>
    <namespace>PluginFunctions</namespace>
    <class kind="class">PluginFunctions::ObjectIterator</class>
    <member kind="function">
      <type>DLLEXPORT ObjectIterator</type>
      <name>objects_end</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d1ebd6bab5cdb61704584c9f7a8fa1e3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>BaseObject *&amp;</type>
      <name>objectRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7db0b763935fa0d5933fac6f605b5cba</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_picked_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5a5a6830d7f7289647fa6fecec7ab36c</anchor>
      <arglist>(const unsigned int _node_idx, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f781f21a8d2ac7ebb2c027284407ed0f</anchor>
      <arglist>(const int _id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b045e2632c7378cc2e0150066233b47</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>copyObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9e5cdfe8b47d65f62e51f87b88ef2bfa</anchor>
      <arglist>(const int _id)</arglist>
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
      <anchor>634e1aa080dbbd82e96144b0483fd085</anchor>
      <arglist>(const int _identifier, BaseObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>17125357788f11bf91380cd146aa6a3d</anchor>
      <arglist>(const int _identifier, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>object_exists</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>924e6965de6c3a36291811c121ded9df</anchor>
      <arglist>(const int _identifier)</arglist>
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
      <name>setActiveExaminer</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b67d722dc9182c4973603519638e98f8</anchor>
      <arglist>(const unsigned int _id)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>activeExaminer</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c092e03e4f2fd8744c6ea4b721bf8c13</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>e20d1424a44165838009bc1828f982ff</anchor>
      <arglist>(ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>da942b5e50f2ca047ed4fc54b6834e51</anchor>
      <arglist>(const unsigned int _examiner, ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>95bf07301b5f4e367e9d6909acbaf7fd</anchor>
      <arglist>(ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d95c682f42a2936ed2a7ac62f3068f3d</anchor>
      <arglist>(const unsigned int _examiner, ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
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
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>354c6201d4629d6607ebd0a43d2125e2</anchor>
      <arglist>(const unsigned int _examiner, std::string _mode)</arglist>
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
      <name>getSceneGraphRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>db9160f9b0e24053d577b42efe1d37d4</anchor>
      <arglist>()</arglist>
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
      <name>addNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f0b8838cfc4bbe0cdb7b91815ecc8477</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addGlobalNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>2458aef543b14b2ac0b06c50d168414e</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>10625157f033ef4f8e3c73554450e1b0</anchor>
      <arglist>(Viewer::ActionMode _mode)</arglist>
    </member>
    <member kind="function">
      <type>Viewer::ActionMode</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a7ad2d5b69ca3cc449a3e702fb4e5d4f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Viewer::ViewerProperties &amp;</type>
      <name>viewerProperties</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>897bdc5ebf036348c0e543380c2a6cfa</anchor>
      <arglist>(int _id)</arglist>
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
      <name>setMainGLContext</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bf5a1e13a33047b0af8399456e5363a0</anchor>
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
      <anchor>0b648e6b44a59282e5edfc8eda51c675</anchor>
      <arglist>(const ACG::Vec3d &amp;_center, const double _radius)</arglist>
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
      <anchor>5edf7564992ebddd3e5b5ed4207c72bc</anchor>
      <arglist>(const ACG::Vec3d &amp;_axis, const double _angle, const ACG::Vec3d &amp;_center)</arglist>
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
      <anchor>a0d5f7587ac6b1cdd1ae5a84feb428ef</anchor>
      <arglist>(const unsigned int _mode)</arglist>
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
      <name>setGlobalDrawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>47c8bcb0fcca09628f1f9470906d27df</anchor>
      <arglist>(const unsigned int _mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBackColor</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>37a6e72ec175b6ae536f99aa0ca38f61</anchor>
      <arglist>(OpenMesh::Vec4f _color)</arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>mapToGlobal</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>499ada11f6b60e736f314d03395dd808</anchor>
      <arglist>(const QPoint _point)</arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>mapToLocal</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>6da01f4b28ec6cca2a6699cd7c93f244</anchor>
      <arglist>(const QPoint _point)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>flyTo</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b06ba401a82fc57b90afa01f9e29af07</anchor>
      <arglist>(const ACG::Vec3d &amp;_position, const ACG::Vec3d &amp;_center, double _time)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDataRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>500a4c2660d974cc5d9ab8ea0604a5ff</anchor>
      <arglist>(BaseObject *_root)</arglist>
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
    <name>BaseObject.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>BaseObject_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="no" imported="no">OpenFlipper/common/GlobalDefines.hh</includes>
    <includes id="perObjectData_8hh" name="perObjectData.hh" local="yes" imported="no">perObjectData.hh</includes>
    <class kind="class">BaseObject</class>
  </compound>
  <compound kind="file">
    <name>BaseObjectData.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>BaseObjectData_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="no" imported="no">OpenFlipper/common/GlobalDefines.hh</includes>
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
      <type>ACG::SceneGraph::QtTranslationManipulatorNode</type>
      <name>QtTranslationManipulatorNode</name>
      <anchorfile>BaseObjectData_8hh.html</anchorfile>
      <anchor>f3661d1854c63aa68ed6f3f61bbbd9f5</anchor>
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
      <name>USE_OPENMP</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>01df339826abe970836832ae79e5f894</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DLLEXPORT</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>808e08638be3cba36e36759e5b150de0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DLLEXPORTONLY</name>
      <anchorfile>GlobalDefines_8hh.html</anchorfile>
      <anchor>1c491d48ac3a6834027355b3c3ba843e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>GlobalOptions.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>GlobalOptions_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="no" imported="no">OpenFlipper/common/GlobalDefines.hh</includes>
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
      <name>shaderDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>f3ec6c8f3bf06cfbf8b90b88575ae32c</anchor>
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
      <type>QDir</type>
      <name>fontsDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b923819bcdb0136b2ab13621c04bb36c</anchor>
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
      <name>shaderDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>e7c275a8828869baa67ce64bb02258aa</anchor>
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
      <type>QString</type>
      <name>fontsDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>4e56424d7e28ffd236073a63e1efaee6</anchor>
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
      <name>shaderDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ab6114d89740edb133e2a8c26989b2c2</anchor>
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
      <type>void</type>
      <name>fontsDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>9436d9f1321cc9b6cc587ecbf3ef01c1</anchor>
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
      <name>shaderDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>0736929daa5476b736141efb80239e91</anchor>
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
      <name>currentScriptDirStr</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b92edb9df11d8359f46d81242ee08f6f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QDir</type>
      <name>currentScriptDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>af133c1d9fd64ff512681f80b5915cb2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>currentScriptDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>234bbc69e49f1675473dc523e707974b</anchor>
      <arglist>(QDir _dir)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>currentScriptDir</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>4e1164f8c452abb5a63e57d7e0f14c84</anchor>
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
      <type>QString</type>
      <name>lang</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>53603fe965755eaed3853c888f4b3eab</anchor>
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
      <name>synchronization</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c7b7d0dba2867fc70bc7cfedbca720d2</anchor>
      <arglist>(bool _synchronization)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>synchronization</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2f2d3cc7de72adbf2f4ac9356aa6c530</anchor>
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
      <name>twoSidedLighting</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fce9476037e3dc278bf3012858361a38</anchor>
      <arglist>(bool _twoSidedLighting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>twoSidedLighting</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>763cc9f51b329e7a24b320a66928aa05</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wheelZoomFactor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>50ac0e6973ef90295736eed562b51491</anchor>
      <arglist>(double _factor)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wheelZoomFactor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>0e04b69ee92ffec4b27e46ff2be7351c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wheelZoomFactorShift</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>43d391b4e7b4dcd3d7747874cd5b2963</anchor>
      <arglist>(double _factor)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wheelZoomFactorShift</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>258e8c8d6ca2d9fc5b9d680915324e81</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>restrictFrameRate</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>52ec7b67ff493d78580335e948755ec5</anchor>
      <arglist>(bool _enable)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>restrictFrameRate</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>e8c71b0ee52bcb9b01013a8134fe113e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>maxFrameRate</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a9fac9954c19f9dba5a319a4506dc224</anchor>
      <arglist>(int _fps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxFrameRate</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a62d58fef92b7a2eac9403ed95908d1d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>standardDrawMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>7a5898552d02de25382abbae82c8137b</anchor>
      <arglist>(uint _mode)</arglist>
    </member>
    <member kind="function">
      <type>uint</type>
      <name>standardDrawMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>8136e7279e0eea1610f66693b06b20ba</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>redrawDisabled</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>94dd40a140ca8e65600fcdaa018c1569</anchor>
      <arglist>(bool disable)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>redrawDisabled</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2d537e9e76914beea19b712d539f50b0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>drawModesInContextMenu</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c2f698c35d36f96550665a50598e5b2f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawModesInContextMenu</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>6f1088e6eaa3cff3f32e67b568e090f1</anchor>
      <arglist>(bool _show)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>multiView</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>be6b7acec18bfa0fdeaba74cff7ad83b</anchor>
      <arglist>(bool _multiView)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>multiView</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>349cc0c29e41a63ae2a2f412cf5827f2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>examinerWidgets</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fc38453e9f1ea7ea21aacf96e66f870c</anchor>
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
      <type>void</type>
      <name>defaultBackgroundColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>77b67a70342e62fa69ffcb4bf46b44fe</anchor>
      <arglist>(QColor _color)</arglist>
    </member>
    <member kind="function">
      <type>QColor</type>
      <name>defaultBackgroundColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>6ef02619dfb5b103df9ade27e2ae2c2b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>defaultBaseColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>2056f6748b8c263e0ee93053562c2053</anchor>
      <arglist>(QColor _color)</arglist>
    </member>
    <member kind="function">
      <type>QColor</type>
      <name>defaultBaseColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>8966b40d75575050e83761f45804eb70</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>randomBaseColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>4a78fc76a7919a6b8156f2331b2d84bd</anchor>
      <arglist>(bool _random)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>randomBaseColor</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>337b3aa612c466cccde55fa364728b74</anchor>
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
      <type>void</type>
      <name>logFileEnabled</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>efc145dc6b49eb779fc145a1f8557d83</anchor>
      <arglist>(bool _enable)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>logFileEnabled</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b1bfcdc1806c316b00b5d2ec968d42ea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>logFile</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>d2ea906f487d3759c2418cbee1794237</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>logFile</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fe1593d1500fa4300eee7456ca09240e</anchor>
      <arglist>()</arglist>
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
      <name>loadingSettings</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>3acecb7ff6cf6260b5be8ac6b3bf6184</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadingSettings</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c5476769d5f816d972ed51fea46415ee</anchor>
      <arglist>(bool _loadingSettings)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>savingSettings</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c997de827c5805eed0bec36d6d8a79a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>savingSettings</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>b9f9c7c7968c200ddbf398cdaf41e915</anchor>
      <arglist>(bool _savingSettings)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>loadingRecentFile</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>cab4489697bf743b43e5c8b037c46cdf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadingRecentFile</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>376c1d71c3d505d86d331f448185d937</anchor>
      <arglist>(bool _loadingRecentFile)</arglist>
    </member>
    <member kind="function">
      <type>int *</type>
      <name>argc</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>9f0b15c98b77438f01a36ee061f2c64a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>char ***</type>
      <name>argv</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>87dc8e3ef0936a37c29972ac51ac3222</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>argc</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>c35717e8aa088fe905793aa950064e9c</anchor>
      <arglist>(int *_argc)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>argv</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>bb32a019b432e8e50f97d910c1b9b67b</anchor>
      <arglist>(char ***_argv)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>doSlotDebugging</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a493468ebaf1cddbd508571174e5ab8f</anchor>
      <arglist>(bool _debugging)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>doSlotDebugging</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>4b4c7d01ea7b78c26c6e314ad278a3d8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateUrl</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>bea9f69d9e9073b76afbd468a9f46a9d</anchor>
      <arglist>(QString _url)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>updateUrl</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>fef07b56dc2adf604a74245e0f675377</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateUsername</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>ac9f48cfb27c79b943b57f780e93a39d</anchor>
      <arglist>(QString _username)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>updateUsername</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>6e79e76fb42466b076e27868023c83a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updatePassword</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>5847ec70cb102d47a2135668d6939583</anchor>
      <arglist>(QString _password)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>updatePassword</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>bf190fbbafea5b4f7e79fc74b4b60ebb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>coreVersion</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>a5482271a1ce0f77edc30af35d4d51b8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pickingRenderMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>280cf2ce41e2b9ace6028c380445ab6a</anchor>
      <arglist>(QString _target)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>pickingRenderMode</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>8bd50dac1ce139e22de8d6a90f8bf5fa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>renderPicking</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>239e43a47e9a3aeef06441c58f854bbb</anchor>
      <arglist>(bool _enable)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>renderPicking</name>
      <anchorfile>namespaceOpenFlipper_1_1Options.html</anchorfile>
      <anchor>af7d40993ecc8b528f580cf8cd971585</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>perObjectData.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>perObjectData_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="no" imported="no">OpenFlipper/common/GlobalDefines.hh</includes>
    <class kind="class">PerObjectData</class>
  </compound>
  <compound kind="file">
    <name>Types.cc</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>Types_8cc</filename>
    <includes id="Types_8hh" name="Types.hh" local="yes" imported="no">Types.hh</includes>
    <class kind="class">TypeInfo</class>
    <member kind="function">
      <type>void</type>
      <name>initialize</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>25a40b6614565f755233080a384c35f1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DataType</type>
      <name>addDataType</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>e474369a16f48f4d8b5ca0a8954a9fb1</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>DataType</type>
      <name>typeId</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>1109336c14463d8aac9527d02a86d4ef</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>typeName</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>3e7e15b9e3323948f062f192d5bc8f1f</anchor>
      <arglist>(DataType _id)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>typeIcon</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>fa0cd1c394dcffcdd78a0284b529d3df</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>typeIcon</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>722ea7e638a22548d8cf0c4c39de8fab</anchor>
      <arglist>(DataType _id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTypeIcon</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>2eaed7f07a023bde3085f471d19f5c28</anchor>
      <arglist>(DataType _id, QString _icon)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTypeIcon</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>67c3e0c037a37de8f131fe0c85170656</anchor>
      <arglist>(QString _name, QString _icon)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static int</type>
      <name>nextTypeId_</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>0497caf0b7c344d91bd921edd45b19d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static bool</type>
      <name>initialized_</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>ef7436a220692b5b863db4e9d8b870a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::map&lt; unsigned int, QString &gt;</type>
      <name>typeToString</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>c684468fe7ef0a90c84720ac121226d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::map&lt; QString, unsigned int &gt;</type>
      <name>stringToTypeInfo</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>e446555fc36d80d883500da26dd2f174</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::map&lt; unsigned int, unsigned int &gt;</type>
      <name>typeToTypeInfo</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>981bc17b5cc31e74d67cd964254beaa6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::vector&lt; TypeInfo &gt;</type>
      <name>types</name>
      <anchorfile>Types_8cc.html</anchorfile>
      <anchor>c313c299f3f4500f14ad23e5789d162f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Types.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>Types_8hh</filename>
    <includes id="GlobalDefines_8hh" name="GlobalDefines.hh" local="no" imported="no">OpenFlipper/common/GlobalDefines.hh</includes>
    <includes id="BaseObject_8hh" name="BaseObject.hh" local="yes" imported="no">BaseObject.hh</includes>
    <includes id="BaseObjectData_8hh" name="BaseObjectData.hh" local="yes" imported="no">BaseObjectData.hh</includes>
    <member kind="typedef">
      <type>unsigned int</type>
      <name>DataType</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>43b5b28c54892b611c165fd5ebea327e</anchor>
      <arglist></arglist>
    </member>
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
      <anchor>3022e04c6ae5353c5912a516f549ee65</anchor>
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
      <anchor>075f87f167b3abc05e60323bdc261ce8</anchor>
      <arglist>(QVector&lt; int &gt;)</arglist>
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
    <member kind="function">
      <type>DLLEXPORT DataType</type>
      <name>addDataType</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>711d5b1d18c54e1fb6cd0283f1a7a215</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT DataType</type>
      <name>typeId</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>2e03981ec754e81543c3126ab6e64bae</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT QString</type>
      <name>typeName</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>419f47c22fff00dcf334dd9df25bd1ed</anchor>
      <arglist>(DataType _id)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT QString</type>
      <name>typeIcon</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>0abc26d2a5371f42592ec4a35e20569b</anchor>
      <arglist>(QString _name)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT QString</type>
      <name>typeIcon</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>0f7003e4a281d209223037feee53eaf5</anchor>
      <arglist>(DataType _id)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT void</type>
      <name>setTypeIcon</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d665808a88085c8d50a2f0c3863885f6</anchor>
      <arglist>(DataType _id, QString _icon)</arglist>
    </member>
    <member kind="function">
      <type>DLLEXPORT void</type>
      <name>setTypeIcon</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>c0596bb3d5ba2c0f99bee2b414eba084</anchor>
      <arglist>(QString _name, QString _icon)</arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_NONE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d1bf9c234e58f0dfef88a6d6096d7285</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_GROUP</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>ea1affb414ebd6b90b9bf6894e19d049</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_POINTS</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>7124788a3c13985de538109bdf973980</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_POLY_LINE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>d7bb42f3597812bd7b4673b1d33e783f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_TRIANGLE_MESH</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>e7ca37510fdfd73ea2b99b4a25c9d7fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_POLY_MESH</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>36628d595a9ad80b703f065b770be077</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_BSPLINE_CURVE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>51969beff7e433b379e1476ab6aa67fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_VOLUME</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>48b8a6905326ed901e156134f08edb57</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_BSPLINE_SURFACE</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>2cc8a32cff7ea5de261aee442a16353a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_SKELETON</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>31457ee1f1f4f745a44d36bbef1bd174</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_GIS</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>560bcad8cd494f1614b31335c4109289</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const unsigned int</type>
      <name>DATA_ALL</name>
      <anchorfile>Types_8hh.html</anchorfile>
      <anchor>5689ea47af0e95e34c631e174efe8837</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ViewerProperties.hh</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>ViewerProperties_8hh</filename>
    <class kind="class">Viewer::ViewerProperties</class>
    <member kind="enumvalue">
      <name>PickingMode</name>
      <anchorfile>namespaceViewer.html</anchorfile>
      <anchor>23df76c69888d15a460c61ef853567d89ca30e13af1da823ee1e8e7a5b8df15d</anchor>
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
    <class kind="struct">ViewMode</class>
    <class kind="class">StackWidgetInfo</class>
    <class kind="struct">MenuInfo</class>
    <class kind="class">CoreWidget</class>
  </compound>
  <compound kind="page">
    <name>buildingOpenFlipper</name>
    <title>Building OpenFlipper</title>
    <filename>buildingOpenFlipper</filename>
    <docanchor file="buildingOpenFlipper">bs</docanchor>
    <docanchor file="buildingOpenFlipper">blinux</docanchor>
    <docanchor file="buildingOpenFlipper">dependencies</docanchor>
    <docanchor file="buildingOpenFlipper">reqlibs</docanchor>
    <docanchor file="buildingOpenFlipper">bwin</docanchor>
    <docanchor file="buildingOpenFlipper">bmac</docanchor>
  </compound>
  <compound kind="page">
    <name>dataFlow</name>
    <title>Dataflow</title>
    <filename>dataFlow</filename>
    <docanchor file="dataFlow">updateObjects</docanchor>
    <docanchor file="dataFlow">saveSettings</docanchor>
    <docanchor file="dataFlow">loadSettings</docanchor>
    <docanchor file="dataFlow">ref1</docanchor>
    <docanchor file="dataFlow">ref2</docanchor>
    <docanchor file="dataFlow">updateView</docanchor>
    <docanchor file="dataFlow">ref3</docanchor>
    <docanchor file="dataFlow">ref4</docanchor>
    <docanchor file="dataFlow">quickref</docanchor>
    <docanchor file="dataFlow">ref5</docanchor>
    <docanchor file="dataFlow">startup</docanchor>
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
    <docanchor file="pluginProgramming">ex1b</docanchor>
    <docanchor file="pluginProgramming">ex1</docanchor>
    <docanchor file="pluginProgramming">plugin_sec</docanchor>
    <docanchor file="pluginProgramming">plugin_prog_sec</docanchor>
  </compound>
  <compound kind="class">
    <name>BackupInterface</name>
    <filename>classBackupInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>createBackup</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>b73983b858434397e6d41c1dea571d94</anchor>
      <arglist>(int, QString)</arglist>
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
      <anchor>b884cce9d2b9c76c54035c71be933c20</anchor>
      <arglist>(int, QString, int)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotRestoreRequested</name>
      <anchorfile>classBackupInterface.html</anchorfile>
      <anchor>00f15086f92007aaf966fadb3527ba51</anchor>
      <arglist>(int, QString, int)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BaseInterface</name>
    <filename>classBaseInterface.html</filename>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotObjectUpdated</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>98f47d25101c25198b8230c751e4d5d6</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotAllCleared</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>a00a3111da5d92c9d001a0a8272c5a91</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotActiveObjectChanged</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>67453077e07cfd4b9f92c27046c861bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateView</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>ce0d6b943ce94f48c40e8c0e17a8413e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updatedObject</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>c500b033dc68fb00ed571f0424f954a4</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>activeObjectChanged</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>8352ea374c4893bad326f3fc93cb6667</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>setSlotDescription</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>e44dcc39152105cb9b00f7cccc979246</anchor>
      <arglist>(QString, QString, QStringList, QStringList)</arglist>
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
    <member kind="slot" virtualness="virtual">
      <type>virtual QString</type>
      <name>version</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>1223005ae02b961eb515823343811adc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BaseInterface</name>
      <anchorfile>classBaseInterface.html</anchorfile>
      <anchor>ab4caea3727739ce97fa28bdc5ddf575</anchor>
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
      <name>last</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>115e78be7fa0bee0d78a0f9bc9e4ede9</anchor>
      <arglist>()</arglist>
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
    <member kind="variable" protection="private">
      <type>QMap&lt; QString, PerObjectData * &gt;</type>
      <name>dataMap_</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>e699683a8dada8178a6aba7b20936770</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setObjectData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>011e6fd32423693f783231046db4c680</anchor>
      <arglist>(QString _dataName, PerObjectData *_data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearObjectData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>e0ad3d27cc922cb4ada6149aaa4b04e4</anchor>
      <arglist>(QString _dataName)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasObjectData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>a5b773c721abdf3273b55eacbeb8488d</anchor>
      <arglist>(QString _dataName)</arglist>
    </member>
    <member kind="function">
      <type>PerObjectData *</type>
      <name>objectData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>64c9fbbfa9225b718474667da04b7dba</anchor>
      <arglist>(QString _dataName)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteData</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>8fd08ba49fa8a38e4164a556930b6072</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BaseObject</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>1d2856f44cdd84060bedbf58c69a99de</anchor>
      <arglist>(const BaseObject &amp;_object)</arglist>
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
    <member kind="function" virtualness="virtual">
      <type>virtual BaseObject *</type>
      <name>copy</name>
      <anchorfile>classBaseObject.html</anchorfile>
      <anchor>a914502976b64d3048fe068cd0097e67</anchor>
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
      <type>QtTranslationManipulatorNode *</type>
      <name>manipulatorNode_</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>a2f70298a2c1e14b9389b53944c74a35</anchor>
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
      <type>QtTranslationManipulatorNode *</type>
      <name>manipulatorNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>df4fdd191347abb3d104c8f740b7cbfd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual ACG::SceneGraph::ShaderNode *</type>
      <name>shaderNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>c09e5177c2a65d7cb7bc1e9577e41196</anchor>
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
    <member kind="function">
      <type>void</type>
      <name>getBoundingBox</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>b9aa8e04bc57c7a74f8a391a72c5a531</anchor>
      <arglist>(ACG::Vec3d &amp;bbmin, ACG::Vec3d &amp;bbmax)</arglist>
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
      <type>bool</type>
      <name>removeAdditionalNode</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>0c2e9e1ff79884132be3f70e46ca5617</anchor>
      <arglist>(NodeT *&amp;_node, QString _pluginName, QString _nodeName, int _id=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BaseObjectData</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>ac91c0bbe48e5fb842b3aced5e1dcd1d</anchor>
      <arglist>(const BaseObjectData &amp;_object)</arglist>
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
      <name>enablePicking</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>4a09f3b75c87eb646d96c9bd0b840c3e</anchor>
      <arglist>(bool _enable)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>pickingEnabled</name>
      <anchorfile>classBaseObjectData.html</anchorfile>
      <anchor>569ab93d5ca6edfb643f2b560d974ae6</anchor>
      <arglist>()</arglist>
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
    <name>ContextMenuInterface</name>
    <filename>classContextMenuInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addContextMenu</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>29cff297615ddbc3f20c1ed26ed64a49</anchor>
      <arglist>(QMenu *, ContextMenuType)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addContextMenu</name>
      <anchorfile>classContextMenuInterface.html</anchorfile>
      <anchor>8f739de4ba5077148ca329f265cb4924</anchor>
      <arglist>(QMenu *, DataType, ContextMenuType)</arglist>
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
      <anchor>ada243267d3da87b564b42fd68db1f37</anchor>
      <arglist>(int)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Core</name>
    <filename>classCore.html</filename>
    <member kind="variable" protection="private">
      <type>QTextStream *</type>
      <name>logStream_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>834829ad748775e0739553f65010928a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QFile *</type>
      <name>logFile_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>cbef8f10d653a760f65a89ad3edc0e23</anchor>
      <arglist></arglist>
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
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLogToFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6b26279666e591c9e4fe280dff5f29ad</anchor>
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
    <member kind="variable" protection="private">
      <type>std::vector&lt; PluginInfo &gt;</type>
      <name>plugins</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>53fad6e68bbafa423fcff9af0037be96</anchor>
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
    <member kind="variable" protection="private">
      <type>QList&lt; SlotInfo &gt;</type>
      <name>coreSlots_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>86ec620fd881f6cf77074dbc6f9a33c5</anchor>
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
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSetSlotDescription</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>91f2838f9835721ab288734ad783e8ac</anchor>
      <arglist>(QString _slotName, QString _slotDescription, QStringList _parameters, QStringList _descriptions)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetDescription</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>50658045acf36fba98ec2d876d7fc921</anchor>
      <arglist>(QString _function, QString &amp;_fnDescription, QStringList &amp;_parameters, QStringList &amp;_descriptions)</arglist>
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
    <member kind="signal">
      <type>void</type>
      <name>setSlotDescription</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>045b9a10ec2052de397c453c45659366</anchor>
      <arglist>(QString _slotName, QString _slotDescription, QStringList _parameters, QStringList _descriptions)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>createWidget</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>efabc75820f7c5db7be88332f62ab585</anchor>
      <arglist>(QString _objectName, QString _uiFilename)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>setDescriptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>29cca226915ead5d25e5b77c1a925ac7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QTimer *</type>
      <name>scenegraphCheckTimer_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6d25d95c73c609296b6dff6d3dfa70fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QTime *</type>
      <name>redrawTime_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>d336b033a504d0c87e7f58b5161d29d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>checkScenegraphDirty</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3eb492bf04a786dcb4baf98a0dbceff8</anchor>
      <arglist>()</arglist>
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
    <member kind="slot">
      <type>void</type>
      <name>saveOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>a127e2bf9cf276b5a6824a3ddc349a0d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>applyOptions</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>1552c32c581b43c9bbbbd2872e65ce00</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>openIniFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>fe994b01c7e2d7f19aaebadfe2fedb8e</anchor>
      <arglist>(QString _filename, bool _coreSettings, bool _perPluginSettings, bool _loadObjects)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>openObjFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>665adaf3244d97bcd7d38ba31fe4212f</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>preprocessObjFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>8cd760829087908ebf5b6aad90c1d3c2</anchor>
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
      <name>restoreKeyBindings</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ccf920a8a2f57801516816ee1f5c4b43</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeIniFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>7b3b84aac13b3d922fa78b28cbf951ad</anchor>
      <arglist>(QString _filename, bool _relativePaths, bool _targetOnly, bool _saveSystemSettings, bool _savePluginSettings, bool _saveObjectInfo)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>writeObjFile</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3664630d860c23ee28f210b6acc09077</anchor>
      <arglist>(QString _filename, bool _relativePaths, bool _targetOnly)</arglist>
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
    <member kind="slot">
      <type>void</type>
      <name>translate</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ac6e21d790ce73a00b603e8d872672df</anchor>
      <arglist>(Vector _vec)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>rotate</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>9d64d351cf75351b720abe57b65f591b</anchor>
      <arglist>(Vector _axis, double _angle, Vector _center)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setViewingDirection</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>48819454934fd4b7cf8e9367ed20d97a</anchor>
      <arglist>(Vector _direction, Vector _upvector)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>fullscreen</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4d1913bc396afb49fefe6158abc21381</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showLogger</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4eea7b30a6bcb1df0af9f40de07e7b1f</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showToolbox</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>17c3efb533115c207bc21107270e8207</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setDrawMode</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>23b2b4eb6e23a58e8869df5daea4c5fc</anchor>
      <arglist>(QString _mode)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>restrictFrameRate</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>6b3abafad547fdd5fa64c7bcc69df14e</anchor>
      <arglist>(bool _enable)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setMaxFrameRate</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>bff9729e1ac8f02da373e58f7f4b1ac9</anchor>
      <arglist>(int _rate)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>snapshotBaseFileName</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>94252cf51e142df761c2c553f39c0274</anchor>
      <arglist>(const QString &amp;_fname, unsigned int _viewerId=0)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>snapshot</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>bf414b28901e3e3fcb178f96a38fb3e9</anchor>
      <arglist>(unsigned int _viewerId=0)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>resizeViewer</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>a0075748ff6afea543786415cf512a94</anchor>
      <arglist>(int _width, int _height)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>writeVersionNumbers</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>b4efd5ba293df9072a1a0d8b6b395d73</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="slot">
      <type>bool</type>
      <name>saveObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f5cb94dacfd9bb358a34cfd93acbc1fd</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="slot">
      <type>bool</type>
      <name>saveObjectTo</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>0e32e8b8667e22348f236aba050e20e8</anchor>
      <arglist>(int _id, QString _filename)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>saveAllObjects</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>4e586376528b35d75241fcc64dd43638</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>saveAllObjectsTo</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>49ab082bb69e3d4b9638e11bda4a8572</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>saveSettings</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2795006a50820ff1c0887f2bf6a93290</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>loadObject</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>69dcb3c8ea17c3a6d5f4d9b6644703ba</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>loadSettings</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>ddcd0d7e15a9219b12f4e64e768be41a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>loadSettings</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>425e2028441ac691e5cb9a78cf3458e1</anchor>
      <arglist>(QString _filename)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>allCleared</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>2bda4925b1f009b92d40996116ba438c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalObjectUpdated</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>f704c76940c997670721d1e71891aad5</anchor>
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
      <name>iniLoadOptionsLast</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>3a0e9cb3c946d5a714650ec9a1abecf9</anchor>
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
      <name>slotObjectUpdated</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>c86dff1812e25d8afeaf1e20de0cdc86</anchor>
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
      <name>slotExit</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>df3cef1b50fdbdefde8a5fd979a5d744</anchor>
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
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotGetValue</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>1a30ad8750d1d9b5cc9fac596e1d91ef</anchor>
      <arglist>(QString _expression, QVariant &amp;_result)</arglist>
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
      <type>QTimer *</type>
      <name>redrawTimer_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>697e78b76df6777873fb1146cacc5db2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode *</type>
      <name>root_node_scenegraph_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>c1270bc4d4efe405abc22306775c2be9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>SeparatorNode *</type>
      <name>root_node_</name>
      <anchorfile>classCore.html</anchorfile>
      <anchor>91ac2a9f5dcbd6667817495b9323f5c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CoreWidget</name>
    <filename>classCoreWidget.html</filename>
    <member kind="typedef" protection="private">
      <type>std::multimap&lt; std::pair&lt; int, Qt::KeyboardModifiers &gt;, std::pair&lt; QObject *, int &gt; &gt;</type>
      <name>KeyMap</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>89285677f9fc5ab90d660c1d64e0b0dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef" protection="private">
      <type>std::multimap&lt; std::pair&lt; QObject *, int &gt;, std::pair&lt; int, Qt::KeyboardModifiers &gt; &gt;</type>
      <name>InverseKeyMap</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>f2896ced7ff1204c6dcb720d5c32aece</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef" protection="private">
      <type>std::pair&lt; KeyMap::iterator, KeyMap::iterator &gt;</type>
      <name>KeyRange</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e56bdc726a4ca0da71025fd020aa88f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; KeyBinding &gt;</type>
      <name>coreKeys_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c9dda7b40a40fce4af24f16aafdd94ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>KeyMap</type>
      <name>keys_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>63882e3bd4dae219e6b47b200fcd66b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>InverseKeyMap</type>
      <name>invKeys_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c11b560a2c6d45cea81911be08d790fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QList&lt; SlotInfo &gt; &amp;</type>
      <name>coreSlots_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5f8996f83bc9c2e625c04bc007ca92a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadKeyBindings</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>95ca5b4f82f2894de15939af26501156</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveKeyBindings</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>955990bd5c4afc4b960488c0ce2bf6af</anchor>
      <arglist>(INIFile &amp;_ini)</arglist>
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
    <member kind="signal">
      <type>void</type>
      <name>registerKey</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a2d30979f416e884642d7da41f44e7ab</anchor>
      <arglist>(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse=false)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>call</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5764acdfa3e8bbc4a318abe36ead5b41</anchor>
      <arglist>(QString _expression, bool &amp;_success)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotRegisterKey</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>77782ad6103c907c5bb5e2e550c27891</anchor>
      <arglist>(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse=false)</arglist>
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
    <member kind="function" protection="private">
      <type>void</type>
      <name>registerCoreKeys</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>b6c3d7e961e2da2cc7b39202a0ba6dc6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>coreKeyPressEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>32ea3c7b03bf050d71a972ce19e7dabd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>coreKeyPressEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8b93f82a950c5f39c71d07a46f5fdf64</anchor>
      <arglist>(QKeyEvent *_e)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>coreKeyReleaseEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5d33f06f83d0ca36cc28019623e2e0fa</anchor>
      <arglist>(QKeyEvent *_e)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>KeyBinding</type>
      <name>getKeyBinding</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8a4dd451dc65399dc7c9fa5e5762f989</anchor>
      <arglist>(QObject *_plugin, int _keyIndex)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>QString</type>
      <name>getRPCName</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>11d70e7e012f9d2aa4c201760ce09b90</anchor>
      <arglist>(QObject *_plugin)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>slotAddKeyMapping</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3ea6ee8dbf05f66716aeedd3c16682ca</anchor>
      <arglist>(int _key, Qt::KeyboardModifiers _modifiers, QObject *_plugin, int _keyBindingID)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>slotRegisterSlotKeyBindings</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>f845ab3d7ac371b452317f698bf6e37b</anchor>
      <arglist>()</arglist>
    </member>
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
      <anchor>e1aa8028b45bbe82f9745adafafeab33</anchor>
      <arglist>(QVector&lt; QDockWidget * &gt; _widgets)</arglist>
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
      <name>functionMenu_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>13f2cc01d0aa5016534ff172c6f18801</anchor>
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
    <member kind="slot" protection="private">
      <type>void</type>
      <name>updateGlobalOptions</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>413c073ba2419770eb430012e22dc7ab</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>changeBackgroundColor</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a79f7f9d75b8525d95c322b03a792cfe</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotPasteView</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>1df87e8b2a7cb51655eed2682b90038e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotCopyView</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>1b4ba15cdb71845c5d6bad51551219a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSnapshot</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>50b1afb1f379f033cfcb177c889e099c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotSnapshotName</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0c6319c463e1443559abd9ea2cbfb124</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>updatePopupMenu</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>59486883d2004e09a30323a07115b563</anchor>
      <arglist>(const QPoint &amp;_point, unsigned int _examinerId)</arglist>
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
      <type>AboutWidget *</type>
      <name>aboutWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>bc2fbfa11a672006c0ccf16ab81c612f</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showAboutWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>7777ae8855ca888abbda01593e89da94</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>OptionsWidget *</type>
      <name>optionsWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e005d1790de3d9570c593c4d049bb7b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>applyOptions</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>ef14826de9df81baf1b87b72d0363432</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>saveOptions</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>550998375e6d5edf99c555f47a3e9ce9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>showOptionsWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>d536081ecbc4c4c959a6035a175e1856</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolBar *</type>
      <name>mainToolbar_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>ecb53a1b74438c95b056a6a501977c70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; QToolBar * &gt;</type>
      <name>toolbars_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>82fb1b3cc12089599cf6e925314b6c8a</anchor>
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
    <member kind="slot" protection="private">
      <type>void</type>
      <name>getToolBar</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a845879932bac0295330842bdad18801</anchor>
      <arglist>(QString _name, QToolBar *&amp;_toolbar)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QStatusBar *</type>
      <name>statusBar_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>6f89e3ed6337ea63c6f9e5064551cba7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QLabel *</type>
      <name>statusIcon_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5f4110aa004b7358d87766ad3a1d0090</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>setupStatusBar</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e6f7e6214851d0b054574f2974b6b99c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>statusMessage</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0f03b55e06d24f1ee7d5b7d8dddf7267</anchor>
      <arglist>(QString _message, int _timeout=0)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>clearStatusMessage</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a313be9143e350d9386861c0af4bb24c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setStatus</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>509291d47c1da4955ffc4fe786a423de</anchor>
      <arglist>(ApplicationStatus::applicationStatus _status)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>stereoActive_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>5f34285475c2102c7024ca91ebdf754d</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotToggleStereoMode</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>858e06f8e9167de1867d57e19f4b2d60</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotProjectionModeChanged</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>144f67346455d7b89f46ed342c39300f</anchor>
      <arglist>(bool _ortho)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotActionModeChanged</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>aa054f75c98879a835825c1b9c7227f2</anchor>
      <arglist>(Viewer::ActionMode _mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotFunctionMenuUpdate</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>6fffbfe2bd33c0ee1881c1c973e86f27</anchor>
      <arglist>()</arglist>
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
    <member kind="signal">
      <type>void</type>
      <name>log</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>c63ecedc633251242e33f4a868914391</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>log</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>1ead1c75560a1d31b2c0128da48b14c9</anchor>
      <arglist>(QString _message)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CoreWidget</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2ed25eb964e15149a83c8a8363d7fd32</anchor>
      <arglist>(QVector&lt; ViewMode * &gt; &amp;_viewModes, std::vector&lt; PluginInfo &gt; &amp;_plugins, QList&lt; SlotInfo &gt; &amp;_coreSlots)</arglist>
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
    <member kind="function">
      <type>void</type>
      <name>toggleFullscreen</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0c3e9bb8f6f1470b87d044912ea6f6fe</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFullscreen</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>94f65d0721191eaffccf7455b8b7afdc</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>toggleLogger</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>41c9b89e0447999597a7470b17174a0d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>showLogger</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>82d4c52515774c4df39458572d32a283</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>toggleToolbox</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>345aaf88027e405f04c39b2c1636b84f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>showToolbox</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2e693a6a9b572f5193beac0ecfcf2f5c</anchor>
      <arglist>(bool _state)</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; glViewer * &gt;</type>
      <name>examiner_widgets_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2140c923a209abf1f070da33570fc129</anchor>
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
      <type>LoggerWidget *</type>
      <name>logWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>4d90fc665b506932d8b4bb8380ed4fc6</anchor>
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
    <member kind="variable">
      <type>QGLWidget *</type>
      <name>glWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>e0f7b8a6eeea3a7d6452203bce78914b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QtGLGraphicsScene *</type>
      <name>glScene_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>35ec4d501b157c452a3655d53a13f94b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QtGLGraphicsView *</type>
      <name>glView_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>bff2fdaa2c2e510074327d1540cc67f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QGraphicsWidget *</type>
      <name>centerWidget_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>11a925fb82dfe0994c9f3ea718e18ba7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QtMultiViewLayout *</type>
      <name>baseLayout_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>802a6c37b67d8c6b22f8d6a3869aa160</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ACG::QtWidgets::QtSceneGraphDialog *</type>
      <name>sceneGraphDialog_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8e7f2c7573fe85709fe1638181c59965</anchor>
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
      <name>slotShowSceneGraphDialog</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>0dcacda93e1cae52e5567ca187ccb6e3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotLog</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>a51e3d88efc42526907734abc9f0a178</anchor>
      <arglist>(Logtype _type, QString _message)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>sceneRectChanged</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>98f0b09a56303ef0663b9913a43df4ca</anchor>
      <arglist>(const QRectF &amp;rect)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>startDrag</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>8f0b0cd449b7b5ca15973d9222cfa488</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>dragEnterEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3441edd1d244d011de59c4c50c5d84db</anchor>
      <arglist>(QDragEnterEvent *_event)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>dropEvent</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3d5b707f4e28ae077aec9b11089f3065</anchor>
      <arglist>(QDropEvent *_event)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; PluginInfo &gt; &amp;</type>
      <name>plugins_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>36ee50855a24bf54a70471606a84a9f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolBar *</type>
      <name>viewerToolbar_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>25a5cfb2c398b27ace6d22420881b405</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>toolbarCount_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>f4bb53c8d58205cb03c7e42e4a72c76c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>stereoButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>1e2a2af299e972e8de85f7d83f29e248</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>projectionButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>2691fe7852b7a77b904aa1893f53598d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>moveButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>3a81b859e28a3624364e32cdb742e07c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>lightButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>58e2ff25c66c38c9a61783e8deb6e988</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>pickButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>4df262d89e79017b7f4dceb95b405c2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QToolButton *</type>
      <name>questionButton_</name>
      <anchorfile>classCoreWidget.html</anchorfile>
      <anchor>98add8278cd6ed013aa428a12cb72e72</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FileInterface</name>
    <filename>classFileInterface.html</filename>
    <member kind="slot" virtualness="pure">
      <type>virtual int</type>
      <name>addEmpty</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>59e29d9d4b046c74afc332379ebb85bf</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="slot" virtualness="pure">
      <type>virtual int</type>
      <name>loadObject</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>e54da5fb8992d7850ea6b2d87851cd27</anchor>
      <arglist>(QString)=0</arglist>
    </member>
    <member kind="slot" virtualness="pure">
      <type>virtual bool</type>
      <name>saveObject</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>5ed9d6acc10a25f0b2488d0587e3840a</anchor>
      <arglist>(int, QString)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QWidget *</type>
      <name>saveOptionsWidget</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>7c833c5577f76b7fcdeb43ef03fb4b2a</anchor>
      <arglist>(QString)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QWidget *</type>
      <name>loadOptionsWidget</name>
      <anchorfile>classFileInterface.html</anchorfile>
      <anchor>132713e6d18b3ab275b2d174bb220dd7</anchor>
      <arglist>(QString)=0</arglist>
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
      <anchor>4d62afb1662143b21dfd26b78006bc42</anchor>
      <arglist>(glViewer *)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>glViewer</name>
    <filename>classglViewer.html</filename>
    <member kind="variable" protection="protected">
      <type>QPoint</type>
      <name>lastPoint2D_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>00446950f8b6d47d8acd31636e43f51b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::Vec3d</type>
      <name>lastPoint3D_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a2b83d75bf45d04b422df9ed9b66b039</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>lastPoint_hitSphere_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9db33b61abd6b84c46ede4fa75a1a246</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>allowRotation_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>22941ad7c7da19057c029940dd4e763b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::Vec3d</type>
      <name>lastRotationAxis_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5a4e5688ae7163cd2b8e8496b4ad67ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>lastRotationAngle_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>19ac1c6a32492ab833ad7ede3f5e8e89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QTime</type>
      <name>lastMoveTime_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a2ee419cea934d78372d486a2ae084cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QTimer *</type>
      <name>timer_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>2e95cbe21947cfdf06f1f46178c740e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>allowRotation</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3d054f546c2fa9b94f00634edeffcb63</anchor>
      <arglist>(bool _mode)</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotAnimation</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>581ff3dcb5ff643572f1bd740ea91c9d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>mapToSphere</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>cb6a95462f8576f4741b2647a13bded1</anchor>
      <arglist>(const QPoint &amp;_p, ACG::Vec3d &amp;_result) const </arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>stereo_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>0046283fd48cadac04c30cd8921de956</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>eyeDist_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4b6d680f93eee269360d40ee72b77fbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>focalDist_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>97c9059ac0e67b0fd19fc197329d6574</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setEyeDistance</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>aadeb3b979ce1e595d601c2afa76e2f1</anchor>
      <arglist>(double _distance)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eyeDistance</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ea1b5eb7c11dee67424fb1452fe9d0d9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFocalDistance</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f0a8aeedc38f4f06daf66d0b50fd39bf</anchor>
      <arglist>(double _distance)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>focalDistance</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>cd4a87453269ccd501f51539294cf6bc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>setStereoMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>95925e71190d9bd331d5208c8c2fd97c</anchor>
      <arglist>(bool _b)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; PickMode &gt;</type>
      <name>pick_modes_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>019fdcfc41e7e7ccc0062d57b9ea92d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>pick_mode_name_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9a4ec2feed01efb0d00ecf4818515145</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>pick_mode_idx_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>aafe5532f3ea7195c6a55d96b1107ba0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>pick</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>1a66a4c613ec1e81697c29029674c121</anchor>
      <arglist>(ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>fast_pick</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f062e1ff98e412324d8b473442452ffb</anchor>
      <arglist>(const QPoint &amp;_mousePos, ACG::Vec3d &amp;_hitPoint)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPickMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>865176ee03c4993699c2993290086805</anchor>
      <arglist>(const std::string &amp;_name, bool _mouse_tracking=false, int _pos=-1, bool _visible=true, QCursor _cursor=Qt::ArrowCursor)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearPickModes</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>baa98a137239890f004489682fb23a5f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>pickMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>db51a7e4c94c49a945cff456f976d077</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pickMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>be2ebf608a2ae4305f141c206c41cc2d</anchor>
      <arglist>(const std::string &amp;_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pickMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>070ec2061d800a79626eaf7af77b25dd</anchor>
      <arglist>(int _id)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalPickModeChanged</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f820e0c63f528f689fac6ef2a02cc192</anchor>
      <arglist>(const std::string &amp;)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setPickModeCursor</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>1ba55746366be14987fb59373146fe99</anchor>
      <arglist>(const std::string &amp;_name, QCursor _cursor)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setPickModeMouseTracking</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a174fe4a23cf1d7f913db66a605c2939</anchor>
      <arglist>(const std::string &amp;_name, bool _mouseTracking)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>Viewer::ViewerProperties</type>
      <name>properties_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d28fbefa8fbd0e360da20175493ffcb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::GLState *</type>
      <name>glstate_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>973f1239106e8ab29f3d6f17cd29e23a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>Viewer::ViewerProperties *</type>
      <name>properties</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>196247d4b997aeafdd42dfecc0bea987</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>slotPropertiesUpdated</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>447cb4cd04c778ae72dacd1e165f06f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>updateActionMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>60f4c70ea8f80c41349ce7be0d0223ba</anchor>
      <arglist>(Viewer::ActionMode _am)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>applyProperties</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>b625defb1e20d9a9c00e48c252a32943</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>dragEnterEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>cbf88b37ee977196bb465cfea66b690c</anchor>
      <arglist>(QGraphicsSceneDragDropEvent *_e)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>dropEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>fe3f0abf667d3dc4bd835ee992c253bd</anchor>
      <arglist>(QGraphicsSceneDragDropEvent *_e)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>startDragEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>6eb7c7867eea5eea34a424cda5206f2b</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>dragEnterEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>acc76abe69982fcd9df74fdfb4cc951c</anchor>
      <arglist>(QDragEnterEvent *_event)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>dropEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f46ab2ec34f01c28cc3ccc44d76a2ffa</anchor>
      <arglist>(QDropEvent *_event)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalKeyPressEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>46b1def1a4c0b415f6590410e9ef553e</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>keyPressEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e374e0892f03f013bd61025845f6f17b</anchor>
      <arglist>(QKeyEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>keyReleaseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>b9cf2f9b2c44c136f11da2244ba7ac2d</anchor>
      <arglist>(QKeyEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual bool</type>
      <name>viewKeyPressEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a35d841027bd3ea68363b037b7e1ee09</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="enumeration">
      <name>ProjectionMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>410894db0a91cbe0db227fdfd1d7de10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ORTHOGRAPHIC_PROJECTION</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>410894db0a91cbe0db227fdfd1d7de10de297f12b9ffb4273aafdc6c817bc347</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PERSPECTIVE_PROJECTION</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>410894db0a91cbe0db227fdfd1d7de1079422bc64c445223b76b06ab4d729914</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>NormalsMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>adec7bceb301564e3ec6dfdba72334fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DONT_TOUCH_NORMALS</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>adec7bceb301564e3ec6dfdba72334fc1be493a002b76cae55a430acb6ad46dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NORMALIZE_NORMALS</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>adec7bceb301564e3ec6dfdba72334fc55f7de2ad77cd04402094e5f6b62d771</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>updateGL</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>24818991e6804b48e6c4a12109da19fb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>setHome</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5e86fc0e8248bcfac7ef9c0aaac7d36a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>home</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>fad20e7d26d32a5d7f4a775c389418ac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>viewAll</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>916e56338ded6e740746e806db8e9a28</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>flyTo</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ce2bc533bf496bd46a9a479f49fa1ea0</anchor>
      <arglist>(const QPoint &amp;_pos, bool _move_back)</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>flyTo</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>c13823fc6d7c827633a5ad434892ad00</anchor>
      <arglist>(const QPoint &amp;_pos)</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>flyFrom</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>fa7e2db85d8d707e4c4909ebfafdd562</anchor>
      <arglist>(const QPoint &amp;_pos)</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>flyTo</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>05466ae797f8efb3ea5cbb73963ab9d4</anchor>
      <arglist>(const ACG::Vec3d &amp;_position, const ACG::Vec3d &amp;_center, double _time=1000.0)</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>perspectiveProjection</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>832c32182b7803d7a6fc599e3113bfbc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>orthographicProjection</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f7a7cd1f49fecd1190e34aa3ea174395</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>toggleProjectionMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4c79b3661f49e815c7e94b1dd7e581ee</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>setView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9ee5f113d217408a147710be4eb21b81</anchor>
      <arglist>(const ACG::GLMatrixd &amp;_modelview, const ACG::GLMatrixd &amp;_inverse_modelview)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>actionDrawMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>2a4ae8521f7fd7dd12568376fb23899e</anchor>
      <arglist>(QAction *_action)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>actionPickMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9c5e09777a5a0501150d75a462ea8658</anchor>
      <arglist>(QAction *_action)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>actionPasteView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ab9400a60a728c6d7d59d2c22de4fef4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>actionCopyView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ee726408335b52430ad8a5d446bcf8aa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>snapshot</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>59d2a88c92cedabdac9a07add6476456</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>functionMenuUpdate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>0cdfbe2a1de94d34888ff174a33ff478</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>projectionModeChanged</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>057711652ef62fafa7d84e0371bba8c8</anchor>
      <arglist>(bool _ortho)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalMouseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a87c11591840eab0a4fe21947b0f219f</anchor>
      <arglist>(QMouseEvent *, const std::string &amp;)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalMouseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3dec380630eb571f7fafe5b0c7a66664</anchor>
      <arglist>(QMouseEvent *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalWheelEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>fb6949194894261df0716e0267b2dc67</anchor>
      <arglist>(QWheelEvent *, const std::string &amp;)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalMouseEventIdentify</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a7a0233b9b04661ee7ee3423077cbab2</anchor>
      <arglist>(QMouseEvent *)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalSceneGraphChanged</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e06c270eef78abd328a37c79bbf143da</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_root)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>signalCustomContextMenuRequested</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f7d40857f258091128fe9172dbd96236</anchor>
      <arglist>(const QPoint &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>glViewer</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a2cf4ae4c2c221a96e2b8eff892cf653</anchor>
      <arglist>(QtGLGraphicsScene *_scene, QGLWidget *_glWidget, QGraphicsWidget *_parent=0, const char *_name=0, QStatusBar *_statusBar=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~glViewer</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9c31464f316965958b2b3a45c06ea8cf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QSize</type>
      <name>sizeHint</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4db8872e1e8425d78aeef2051cc82a3e</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStatusBar</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>68aff31861e4cbdf4f2f168e67724547</anchor>
      <arglist>(QStatusBar *_sb)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>makeCurrent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>003e4066701ae292c8102af4c27f5730</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>swapBuffers</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>42d0ee8d7b2b12601229f39e03c95e4e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lockProjectionUpdate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>629706c05138e2dd46c76f5324255190</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unlockProjectionUpdate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5aac6542217b081831d504314b7cb44f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trackMouse</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>111d4749bf6cc3b98ccd9d1cb786b2c8</anchor>
      <arglist>(bool _track)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sceneGraph</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>717d9473f39adf79e6b2f7160e580173</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_root)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>projectionMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>81bd7d788711d21bc91b0cdf2a99b6dd</anchor>
      <arglist>(ProjectionMode _p)</arglist>
    </member>
    <member kind="function">
      <type>ProjectionMode</type>
      <name>projectionMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>faf1994ff92b14e9c75b04ddf294a37d</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setScenePos</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9e7ca32f4659d3a457f937af30f3282e</anchor>
      <arglist>(const ACG::Vec3d &amp;_center, double _radius)</arglist>
    </member>
    <member kind="function">
      <type>const ACG::Vec3d &amp;</type>
      <name>scene_center</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>16829cc5ec8f75cfeb9de1d67931c689</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>scene_radius</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e1d4aae9e66e99ec3e4e400244a27d9d</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>viewingDirection</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>58752cbd503d9e014c577f4b206f1b42</anchor>
      <arglist>(const ACG::Vec3d &amp;_dir, const ACG::Vec3d &amp;_up)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>normalsMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f7cbae18b60e8c5279db7180c0c66754</anchor>
      <arglist>(NormalsMode _mode)</arglist>
    </member>
    <member kind="function">
      <type>NormalsMode</type>
      <name>normalsMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>87e61d3777f9b00d3ff7da835bf4bb99</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>ACG::GLMatrixd &amp;</type>
      <name>light_matrix</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>04aadc295f7cf8b295b8efa7c46249fd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rotate_lights</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>34d692e4d81d3b468c216d7ff049d28d</anchor>
      <arglist>(ACG::Vec3d &amp;_axis, double _angle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update_lights</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>26ca3f4077ebb22e54a408df34ffb419</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>draw_lights</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3d9dd2f1ba2b3d759fec695e59fe3673</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copyToImage</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>afb3a04dd5fa1d800d0815776e5dff42</anchor>
      <arglist>(QImage &amp;_image, GLenum _buffer=GL_BACK)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copyToImage</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>56c6f5eec36d3c32a74c84705e5a66e1</anchor>
      <arglist>(QImage &amp;_image, unsigned int _left, unsigned int _top, unsigned int _width, unsigned int _height, GLenum _buffer)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>glWidth</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>67e9500c789b14e03096967260a80745</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>glHeight</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ae762c72733eab2ae4bdd202bb5946c9</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>glSize</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5712ae947d682be1bf9e2dd37ac302a4</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>glMapFromGlobal</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>667c6cac9c155d516b50ac97abffbb66</anchor>
      <arglist>(const QPoint &amp;_pos) const </arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>glMapToGlobal</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>b29318c79333130d56e3504f60e29b86</anchor>
      <arglist>(const QPoint &amp;_pos) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>85204078badebb60902fc841d04e20d5</anchor>
      <arglist>(unsigned int _mode)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>drawMode</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>673abd6f1aebe2f83d836bfe0d6bf2c2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>encodeView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>230e93e4308f6fa6746329d16ed6a6ac</anchor>
      <arglist>(QString &amp;_view)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>decodeView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a86ad933c05cb700a0417789710f1c98</anchor>
      <arglist>(const QString &amp;_view)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>initModelviewMatrix</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>632c7f6e2f5443fcebf418107133c06e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>grabGLArea</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>927d0d7ec8a8db5e4cb718d0e7862674</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>releaseGLArea</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9bbe625cf2970316e364e9473936e137</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawBlendedObjects</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d40bf80a8373314196a65e90bc9b689c</anchor>
      <arglist>(bool _status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>translate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>52c553a9c1cc8301a5e04e7ac9fd068a</anchor>
      <arglist>(const ACG::Vec3d &amp;trans)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rotate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>b388bab4401e34c8550c7176960999e5</anchor>
      <arglist>(const ACG::Vec3d &amp;axis, double angle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rotate</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>caef294fb3953261a12e8d7b496e57b4</anchor>
      <arglist>(const ACG::Vec3d &amp;axis, double angle, const ACG::Vec3d &amp;_center)</arglist>
    </member>
    <member kind="function">
      <type>QMenu *</type>
      <name>getPickMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3fbbb4e0d7c9ce574157d1b95a33c199</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QMenu *</type>
      <name>getDrawMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>449f8648a1c726b92a35ee6e12dfaf01</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>slotWheelX</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>c83763336ca57153b416b7ce46e99679</anchor>
      <arglist>(double _dAngle)</arglist>
    </member>
    <member kind="slot" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>slotWheelY</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>97914c75c475fd83ea9ca7917c0a5f17</anchor>
      <arglist>(double _dAngle)</arglist>
    </member>
    <member kind="slot" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>slotWheelZ</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>aba207a41fd6c100129571f9fe1a3855</anchor>
      <arglist>(double _dist)</arglist>
    </member>
    <member kind="slot" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>cleanupEventFilter</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>13e16fa2dd25b59b9c5687e2459448dc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>initializeGL</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e496c7f46e2001020468aa04ccd43609</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>paintGL</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3ee498f1a82cb698dad5618fcf559aec</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>resizeEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5d0dd2173cbd6dcd4abb01db1e52c2b0</anchor>
      <arglist>(QGraphicsSceneResizeEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>moveEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9f77b53640eeaa7d2cba609c2aefecf7</anchor>
      <arglist>(QGraphicsSceneMoveEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>glMousePressEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e5dac0efa27dd24fe48cf862f53269b7</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>glMouseDoubleClickEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>1f923264435cbc170830be39c6135e5e</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>glMouseMoveEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>2b2fc03c5ea61bef158f279f5545d189</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>glMouseReleaseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f5abda9d6851aa8edef84bb4ad949fe1</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>glMouseWheelEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>fd5ba310d2085bf12a5aeba292dea9a5</anchor>
      <arglist>(QWheelEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>contextMenuEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>54fefb36ba9ec5c1440bc0265f269ee1</anchor>
      <arglist>(QGraphicsSceneContextMenuEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>viewMouseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>003655d5cf214c2c1b25ec23745422d7</anchor>
      <arglist>(QMouseEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>viewWheelEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4763dde17ec77eb451c16353c88e91dc</anchor>
      <arglist>(QWheelEvent *_event)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>lightMouseEvent</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d39f54b96a1546aa3eb62c1ca50f3ab9</anchor>
      <arglist>(QMouseEvent *)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>updateProjectionMatrix</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>49c8ab97dcd94c3d20a411168a2b28a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>updatePickMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>c05c0bc62704a83fafe19262dee386bc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ACG::Vec3d</type>
      <name>scene_center_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>46578b6914aec849f9d43a93e17f4e5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ACG::Vec3d</type>
      <name>trackball_center_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e562a3dec669f150f04d1dbd76b689b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>scene_radius_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d11ef3eb3f0fc7dbfc85d103e322cfb6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>trackball_radius_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>004a4acc0b3639769781b89bb017eeb3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>GLdouble</type>
      <name>orthoWidth_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>cc8b069523cb527128967e21704c41f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>GLdouble</type>
      <name>near_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>74cc6449d22f1d2fc6ae39861a508573</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>GLdouble</type>
      <name>far_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>6879cd7d10d098d121eee60935042d63</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>GLdouble</type>
      <name>fovy_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>2542f1d760c2845075d030de9e51b391</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>isRotating_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5c3bc8459f255917177f028d5d830513</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>QStatusBar *</type>
      <name>statusbar_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>6be700f980852a4ac3875a1164d6aa4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot" protection="private">
      <type>void</type>
      <name>hidePopupMenus</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>73803dc79273f245c7f5f7de9e8c036b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>glViewer</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>5e0cfdf762eabf4ddf897de5039acaa0</anchor>
      <arglist>(const glViewer &amp;)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>glViewer &amp;</type>
      <name>operator=</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>22c16bc98678f02859c25e0c91ef58bf</anchor>
      <arglist>(const glViewer &amp;)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>createWidgets</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>767d36f04936f3e10f3bcb824709b05e</anchor>
      <arglist>(QStatusBar *_sb)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>drawScene</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>94eff5864439047e50f4b39745da1289</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>drawScene_mono</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>c4264b22ac7383a35e0a8b8344de1ed3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>drawScene_stereo</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>349d8fbade52abe39c08822b0b517f23</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>updatePopupMenu</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>e24b0eeef65628f682be7042d94c8eb5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::Vec3d</type>
      <name>home_center_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>1739023c9ebb66dda710e0700b8c8319</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>home_radius_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>af5300288859a5373f1a2784690fb13d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::GLMatrixd</type>
      <name>home_modelview_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4e5b1b5742878566310d3d0dbd9cbebf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::GLMatrixd</type>
      <name>home_inverse_modelview_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>aa38d2a9eb8ef89110f1e5c79c3bd189</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>homeOrthoWidth_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>b19a60338e6ba34ab59fe25bd1274248</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::GLMatrixd</type>
      <name>light_matrix_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>cc73292049ad87f600c435f7fbf1d523</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>NormalsMode</type>
      <name>normalsMode_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>adc25f7abae3617d48f86374d6d410e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ProjectionMode</type>
      <name>projectionMode_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f8568246dd2f9db01e9e03fc6065181e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>trackMouse_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>08de6f306773b5ad3cc8cb789dcca71d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>glareaGrabbed_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>82b268bb05dc7e02201de139c06d3bca</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>double</type>
      <name>frame_time_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>21c40c7d69d88cd9c420b9a0e3528c9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>pickMenu_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>9cc958d82ac5f7a77b463ce2b18fda8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QMenu *</type>
      <name>drawMenu_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>3c7097c1b356ea5842c0b2d3ea77b183</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::SceneGraph::BaseNode *</type>
      <name>sceneGraphRoot_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a06dcb6574da54fe191cb17d0f48a850</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>unsigned int</type>
      <name>curDrawMode_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d4c016d567c756dc3b68a723b945c6a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>unsigned int</type>
      <name>availDrawModes_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>7ad2097c90a565a0b78880c12f69563c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>projectionUpdateLocked_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>4dbf3a87a38e4e4576fc313d65cb8c9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>blending_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d0861134310331ea2f2baaa793bf65b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QtGLGraphicsScene *</type>
      <name>glScene_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>414a4a6f7f1ba957e54e554988d65dd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QGLWidget *</type>
      <name>glWidget_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a8d9728d2dceb8faec7d195b5415c0c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QtGLViewerLayout *</type>
      <name>glBaseLayout_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>8b40e82a551d8ead382769aaef6f16cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::QtWidgets::QtWheel *</type>
      <name>wheelX_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>44878f1de9090b69efc048d427e3431b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::QtWidgets::QtWheel *</type>
      <name>wheelY_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>ece8b2ddcb87cf2a3bec5ee673623bfc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>ACG::QtWidgets::QtWheel *</type>
      <name>wheelZ_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a8955ed54d3997766c68344c99cbddd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; QAction * &gt;</type>
      <name>drawMenuActions_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>f02f00a06049f1834182d5bfac144b22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QTime</type>
      <name>redrawTime_</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d9541ae0c0d2eb851e0b97256839fe87</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="protected">
      <type>friend class</type>
      <name>QtGLGraphicsScene</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>d9f8454cbfc6c3545a60f8aaad41f34b</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="protected">
      <type>friend class</type>
      <name>QtGLGraphicsView</name>
      <anchorfile>classglViewer.html</anchorfile>
      <anchor>a24555e8027e8185a689c90d12788f2d</anchor>
      <arglist></arglist>
    </member>
    <class kind="struct">glViewer::PickMode</class>
  </compound>
  <compound kind="struct">
    <name>glViewer::PickMode</name>
    <filename>structglViewer_1_1PickMode.html</filename>
    <member kind="function">
      <type></type>
      <name>PickMode</name>
      <anchorfile>structglViewer_1_1PickMode.html</anchorfile>
      <anchor>70823e685e647a7671487f8a14e3c3c1</anchor>
      <arglist>(const std::string &amp;_n, bool _t, bool _v, QCursor _c)</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>structglViewer_1_1PickMode.html</anchorfile>
      <anchor>f38208271942bb539fa129819b428ba9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>tracking</name>
      <anchorfile>structglViewer_1_1PickMode.html</anchorfile>
      <anchor>16b508991ac0ba448527d846a183b179</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>visible</name>
      <anchorfile>structglViewer_1_1PickMode.html</anchorfile>
      <anchor>43718c9f81fd5fef86eb9475f577c1c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QCursor</type>
      <name>cursor</name>
      <anchorfile>structglViewer_1_1PickMode.html</anchorfile>
      <anchor>39050fcc9c75cd04d14436fd72027610</anchor>
      <arglist></arglist>
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
      <anchor>204d2a4094b02c089a7bd3709ee4edaa</anchor>
      <arglist>(const GroupObject &amp;_object)</arglist>
    </member>
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
    <member kind="function">
      <type>BaseObject *</type>
      <name>copy</name>
      <anchorfile>classGroupObject.html</anchorfile>
      <anchor>31b5c637bc3b7ce91a595e613ea93e3f</anchor>
      <arglist>()</arglist>
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
      <anchor>5fa6cd1fda2c98f0a6103379f2b5ac6a</anchor>
      <arglist>(INIFile &amp;, int)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>saveIniFile</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>7a5f263bef710d0428e0e836392c15e6</anchor>
      <arglist>(INIFile &amp;, int)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>loadIniFileOptions</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>3e4a73ff680732b8cab88d36622888f1</anchor>
      <arglist>(INIFile &amp;)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>loadIniFileOptionsLast</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>c0061b86e0da5a1f576cd5d20923430f</anchor>
      <arglist>(INIFile &amp;)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>saveIniFileOptions</name>
      <anchorfile>classINIInterface.html</anchorfile>
      <anchor>5ebbd77a90db14dae3fc3f24ef751893</anchor>
      <arglist>(INIFile &amp;)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>KeyInterface</name>
    <filename>classKeyInterface.html</filename>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>slotKeyEvent</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>7fdc66890e3586a8ef2fce275c282d4f</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="slot" virtualness="virtual">
      <type>virtual void</type>
      <name>slotKeyReleaseEvent</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>0457bbb296f3738afae0309ba4f4f8aa</anchor>
      <arglist>(QKeyEvent *)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>registerKey</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>6384bba90067a6f01455ab70bc9a00c3</anchor>
      <arglist>(int, Qt::KeyboardModifiers, QString, bool=false)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~KeyInterface</name>
      <anchorfile>classKeyInterface.html</anchorfile>
      <anchor>6530d2ecdfa3538a0efc350974c24d9d</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoadSaveInterface</name>
    <filename>classLoadSaveInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>load</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>cea4d677acb12a2b9d9efc739bd41e77</anchor>
      <arglist>(QString, DataType, int &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addEmptyObject</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>75c050bbb947550bf158492009e5b414</anchor>
      <arglist>(DataType, int &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>save</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>4d48d999bd50e17e26c4b82776260cbb</anchor>
      <arglist>(int, QString)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>openedFile</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>854307e9ff2aa4ddb53b584c6e6e80ce</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>emptyObjectAdded</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>e5ea89e5ac2764bada47b44710baf2aa</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getAllFilters</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>45b6a3e59f49fe5d6c522d27fe102d86</anchor>
      <arglist>(QStringList &amp;)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>fileOpened</name>
      <anchorfile>classLoadSaveInterface.html</anchorfile>
      <anchor>4402a353c4ed5acacbb36f5fba1f4190</anchor>
      <arglist>(int)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoggerWidget</name>
    <filename>classLoggerWidget.html</filename>
    <member kind="function">
      <type></type>
      <name>LoggerWidget</name>
      <anchorfile>classLoggerWidget.html</anchorfile>
      <anchor>f6d7a2cba0c0a3aaf26f164e4a93e9ab</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>keyPressEvent</name>
      <anchorfile>classLoggerWidget.html</anchorfile>
      <anchor>b7629a4dc539aab69b3ab2daf2b24b9f</anchor>
      <arglist>(QKeyEvent *_event)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LoggingInterface</name>
    <filename>classLoggingInterface.html</filename>
    <member kind="signal" virtualness="pure">
      <type>virtual void</type>
      <name>log</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>a6485e09bd1b6697bbfebed072ba464e</anchor>
      <arglist>(Logtype, QString)=0</arglist>
    </member>
    <member kind="signal" virtualness="pure">
      <type>virtual void</type>
      <name>log</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>d9724024d8924ba7bb70f6ba8f8c2de9</anchor>
      <arglist>(QString)=0</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>logOutput</name>
      <anchorfile>classLoggingInterface.html</anchorfile>
      <anchor>e02e6d3c3b6fad33700dd449571b58e5</anchor>
      <arglist>(Logtype, QString)</arglist>
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
      <anchor>b1faf34097bb85003651d8cd1ec05fdf</anchor>
      <arglist>(QMenu *, MenuType)</arglist>
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
      <anchor>c633e305b6c0340f6624c28a73611dce</anchor>
      <arglist>(QWheelEvent *, const std::string &amp;)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotMouseEvent</name>
      <anchorfile>classMouseInterface.html</anchorfile>
      <anchor>bbc3a474f53f35eea73a96f284e674b8</anchor>
      <arglist>(QMouseEvent *)</arglist>
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
    <member kind="function">
      <type>void</type>
      <name>get_object_ids</name>
      <anchorfile>classObjectIDPointerManagerT.html</anchorfile>
      <anchor>78c920860d83bad4f13606d032cf52ce</anchor>
      <arglist>(std::vector&lt; int &gt; &amp;_ids)</arglist>
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
    <name>OptionsInterface</name>
    <filename>classOptionsInterface.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~OptionsInterface</name>
      <anchorfile>classOptionsInterface.html</anchorfile>
      <anchor>713235fe3f494de75f7df609a24665b4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>initializeOptionsWidget</name>
      <anchorfile>classOptionsInterface.html</anchorfile>
      <anchor>7d3d052659d4e7bb11e927f6104d7561</anchor>
      <arglist>(QWidget *&amp;)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>applyOptions</name>
      <anchorfile>classOptionsInterface.html</anchorfile>
      <anchor>c9847526732a7a6582edc037236c4c48</anchor>
      <arglist>()=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PickingInterface</name>
    <filename>classPickingInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>97de8ae0363a689d3b7777b01979eefc</anchor>
      <arglist>(const std::string)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addHiddenPickMode</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>3c7c84d0bda4cb93db62c95437910beb</anchor>
      <arglist>(const std::string)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>setPickModeCursor</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>622e08acaf09e8dd075a7b8ba52ddba4</anchor>
      <arglist>(const std::string, QCursor)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>setPickModeMouseTracking</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>9f3b139cd795495088d510fb6ae1bae6</anchor>
      <arglist>(const std::string, bool)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>slotPickModeChanged</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>5a59040e4fb0ada435249ccd363ddf86</anchor>
      <arglist>(const std::string &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~PickingInterface</name>
      <anchorfile>classPickingInterface.html</anchorfile>
      <anchor>e6db7c7b173ae542e477d0de9594ed68</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginInfo</name>
    <filename>classPluginInfo.html</filename>
    <member kind="function">
      <type></type>
      <name>PluginInfo</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>309ed8b80880a4a13488e7e00b47c4df</anchor>
      <arglist>(const PluginInfo &amp;_i)</arglist>
    </member>
    <member kind="variable">
      <type>QObject *</type>
      <name>plugin</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>c1338961e3473f964efff5adfead358b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>name</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>7945c4808c29fd7417e172dd670d2160</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>description</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>b023ec60a5face635cda00fff669166b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>version</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>adebbcf67924dc365fb2002e3d1ec674</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>path</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>c4dc50112776f9ff3930a452cc15053b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>rpcName</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>2c4e8ebb242f91b45b87554ef5554994</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QStringList</type>
      <name>rpcFunctions</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>56a485de7a432ad34c77f2bb36b49c9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QList&lt; SlotInfo &gt;</type>
      <name>slotInfos</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>c12cb9ea3f45624cbf5ee0535a85dffd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QList&lt; KeyBinding &gt;</type>
      <name>keys</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>e7dc23d56319dba1cb2ee94d0950f556</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QDockWidget *</type>
      <name>widget</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>df7f7ca8dfbfc093c7ed3e0de6377f70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QWidget *</type>
      <name>optionsWidget</name>
      <anchorfile>classPluginInfo.html</anchorfile>
      <anchor>f5529de5bd6b2d54549be70562c479c0</anchor>
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
    <name>QtGLGraphicsScene</name>
    <filename>classQtGLGraphicsScene.html</filename>
    <member kind="function">
      <type></type>
      <name>QtGLGraphicsScene</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>ab9ea27b34b8691562b5d5027d0e65f1</anchor>
      <arglist>(std::vector&lt; glViewer * &gt; *_views)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>drawBackground</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>943928a1c8340ce088851b2c9361b8be</anchor>
      <arglist>(QPainter *_painter, const QRectF &amp;_rect)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mousePressEvent</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>64562e825c46632764346e888ab1ecc3</anchor>
      <arglist>(QGraphicsSceneMouseEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mouseDoubleClickEvent</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>b0e40ece490c74bbe1d49ad0a0585620</anchor>
      <arglist>(QGraphicsSceneMouseEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mouseReleaseEvent</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>26c596fba264d002dc975d202c0f9857</anchor>
      <arglist>(QGraphicsSceneMouseEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mouseMoveEvent</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>738637d8cb9769f4c6c5b6d927492882</anchor>
      <arglist>(QGraphicsSceneMouseEvent *_e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>wheelEvent</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>0ad6cd993b336420a5d17aa32a4e461d</anchor>
      <arglist>(QGraphicsSceneWheelEvent *_e)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>glViewer *</type>
      <name>findView</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>2cd9123f872fd669320b3caafb66c710</anchor>
      <arglist>(const QPointF &amp;p)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; glViewer * &gt; *</type>
      <name>views_</name>
      <anchorfile>classQtGLGraphicsScene.html</anchorfile>
      <anchor>a03f0031f03479a525adbac1e8dbd29c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtGLGraphicsView</name>
    <filename>classQtGLGraphicsView.html</filename>
    <member kind="signal">
      <type>void</type>
      <name>sceneRectChanged</name>
      <anchorfile>classQtGLGraphicsView.html</anchorfile>
      <anchor>ccb6cb5917634a8cc9a14d9ae4b8f0c3</anchor>
      <arglist>(const QRectF &amp;rect)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>QtGLGraphicsView</name>
      <anchorfile>classQtGLGraphicsView.html</anchorfile>
      <anchor>00be1c483271b3ac6375c9b78d4fa8d5</anchor>
      <arglist>(QWidget *_parent)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>resizeEvent</name>
      <anchorfile>classQtGLGraphicsView.html</anchorfile>
      <anchor>b3127696f62af47c878fc11bef81bd3f</anchor>
      <arglist>(QResizeEvent *_event)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtGLViewerLayout</name>
    <filename>classQtGLViewerLayout.html</filename>
    <member kind="function">
      <type></type>
      <name>QtGLViewerLayout</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>bea5783722545bd49a823b113b0f6cf1</anchor>
      <arglist>(QGraphicsLayoutItem *_parent=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWheelX</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>28d7aa1a3db97eb6c2abd5e4c3ba7570</anchor>
      <arglist>(QGraphicsWidget *_item)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWheelY</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>672c9b52b0b9fa0f39ed2336d6110ea6</anchor>
      <arglist>(QGraphicsWidget *_item)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWheelZ</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>162e06ee9ea15068b7c44f8800847662</anchor>
      <arglist>(QGraphicsWidget *_item)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>count</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>94e62cd416ff8ee2709471a8e0ba1b00</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QGraphicsLayoutItem *</type>
      <name>itemAt</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>8d1e84f146dbe7b6922ca733d0b29857</anchor>
      <arglist>(int _i) const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>removeAt</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>1d6fab5d96cda9650d6968007c8d071c</anchor>
      <arglist>(int _index)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QSizeF</type>
      <name>sizeHint</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>b886f9f4a0fee5abcebfd4849ac81a27</anchor>
      <arglist>(Qt::SizeHint _which, const QSizeF &amp;_constraint=QSizeF()) const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setGeometry</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>31c26753437a6254c1974bd32584f69a</anchor>
      <arglist>(const QRectF &amp;rect)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>reLayout</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>9f649b6be8c6110ed36732db6950e677</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QGraphicsWidget *</type>
      <name>wheelX_</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>ef59b185c21a778cd513d5c461b0a18a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QGraphicsWidget *</type>
      <name>wheelY_</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>affb00618906338661e95cdea919d3fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QGraphicsWidget *</type>
      <name>wheelZ_</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>fca791505638ad95a6306d39b6d2ca22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QVector&lt; QGraphicsWidget * &gt;</type>
      <name>items_</name>
      <anchorfile>classQtGLViewerLayout.html</anchorfile>
      <anchor>e7d6a86fd7fb45000cc8f79a828ddfca</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtMultiViewLayout</name>
    <filename>classQtMultiViewLayout.html</filename>
    <member kind="enumeration">
      <name>MultiViewMode</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>148ea9ad7bcb8200b37ba604b43b7c90</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>QtMultiViewLayout</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>c0ba95928aa74c4faa7fbe25686ced68</anchor>
      <arglist>(QGraphicsLayoutItem *_parent=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addItem</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>4906656a16405e657995a404d24c29b6</anchor>
      <arglist>(QGraphicsWidget *_item, unsigned int _pos)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMode</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>90f5262eedbb9d62ab32275e75ae4f24</anchor>
      <arglist>(MultiViewMode _mode)</arglist>
    </member>
    <member kind="function">
      <type>MultiViewMode</type>
      <name>mode</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>3edc841f9e65c43c608de3ee3379cbec</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSpacing</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>9a1cd9a9e6223baa18c6b7f969b9dc7a</anchor>
      <arglist>(unsigned int _s)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>count</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>e7c257bc8386849c7c6b5692b5e5ab61</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QGraphicsLayoutItem *</type>
      <name>itemAt</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>0d254d243e27ebe3c1360dc0e7303f72</anchor>
      <arglist>(int _i) const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>removeAt</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>a3eaa9c1a62dd17655c8d2379fb5c03a</anchor>
      <arglist>(int _index)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QSizeF</type>
      <name>sizeHint</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>6a12e376b17a2b9b946b64a1ca9ba89b</anchor>
      <arglist>(Qt::SizeHint _which, const QSizeF &amp;_constraint=QSizeF()) const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setGeometry</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>58dfcb345ce0b28e6d3f6b2a4f47e2d9</anchor>
      <arglist>(const QRectF &amp;rect)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>reLayout</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>a9d11ec5d67710655b2e18465b3b62da</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>MultiViewMode</type>
      <name>mode_</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>474c1cb6266add443e12b508573f4f9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>unsigned int</type>
      <name>spacing_</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>3638f60a17ae2ac563799282e5e4e209</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>QGraphicsWidget *</type>
      <name>items_</name>
      <anchorfile>classQtMultiViewLayout.html</anchorfile>
      <anchor>fc5c5a9c9e8280dc5817c992722dd846</anchor>
      <arglist>[4]</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RPCInterface</name>
    <filename>classRPCInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>pluginExists</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>32b1a7342cf6cec12c3136a99be76485</anchor>
      <arglist>(QString, bool &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>functionExists</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>e3dc1157a96dc782ebdf89431ad4a6b8</anchor>
      <arglist>(QString, QString, bool &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>call</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>0692bad680b9c2e74c3de4dfb43bf559</anchor>
      <arglist>(QString, QString, bool &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>call</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>0fb51e1b53629d889ffe955a27ca25eb</anchor>
      <arglist>(QString, bool &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getValue</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>3e95978bcf6667eeaf53444764c1945b</anchor>
      <arglist>(QString, QVariant &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getPlugin</name>
      <anchorfile>classRPCInterface.html</anchorfile>
      <anchor>831515a71a6e6effe54d859732a073e1</anchor>
      <arglist>(QString, QObject *&amp;)</arglist>
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
      <anchor>becdd70663a2d2c5177f8bf1e9eaef8e</anchor>
      <arglist>(QString)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>executeScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>0d83718971027f7ea305e34afda15299</anchor>
      <arglist>(QString)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getScriptingEngine</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>0f67b896b2839095dae6748054d35b12</anchor>
      <arglist>(QScriptEngine *&amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getAvailableFunctions</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>f644d0f626d53f2633816814cdc3290a</anchor>
      <arglist>(QStringList &amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getDescription</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>4e79d06e3951a46dfdff03676919763a</anchor>
      <arglist>(QString, QString &amp;, QStringList &amp;, QStringList &amp;)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotScriptInfo</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>036652d1d3aec98f871a5d4157cc93f2</anchor>
      <arglist>(QString, QString)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotExecuteScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>07a4010f0dd2bbed2e8737c82a04c5a5</anchor>
      <arglist>(QString)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotExecuteFileScript</name>
      <anchorfile>classScriptInterface.html</anchorfile>
      <anchor>977741eaf99f0cf5b34333bb72d17d21</anchor>
      <arglist>(QString)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TextureInterface</name>
    <filename>classTextureInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>d696a930402f0b15bb7f44c11c33eaae</anchor>
      <arglist>(QString, QString, uint)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>ac80686b170ed3299b5998606ec82c34</anchor>
      <arglist>(QString, int)</arglist>
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
      <anchor>0464b6c7632b7f42629c7bd4e1ac7377</anchor>
      <arglist>(QString, QString)</arglist>
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
      <anchor>182ba6590381219a6ab1018523cbd811</anchor>
      <arglist>(QString, int)</arglist>
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
      <anchor>7cdc9798acb7249162dbad78d2daf480</anchor>
      <arglist>(QString, QString, uint)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotTextureUpdated</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>086283839cabb40032e18f7863793b54</anchor>
      <arglist>(QString, int)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotSetTextureMode</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>cd7c7955071106e3d4ce4f5ebfaf0af6</anchor>
      <arglist>(QString, QString)</arglist>
    </member>
    <member kind="slot" protection="private" virtualness="virtual">
      <type>virtual void</type>
      <name>slotSwitchTexture</name>
      <anchorfile>classTextureInterface.html</anchorfile>
      <anchor>dfb26b10cda3d53c70c1f7af17b995be</anchor>
      <arglist>(QString)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ToolbarInterface</name>
    <filename>classToolbarInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addToolbar</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>8c525462e65dc439a7f21a1511d81183</anchor>
      <arglist>(QToolBar *)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>removeToolbar</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>c2d3e0cdc7c0fac284f8d2460fdeb413</anchor>
      <arglist>(QToolBar *)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getToolBar</name>
      <anchorfile>classToolbarInterface.html</anchorfile>
      <anchor>5071fcc9ad6ab9e379bf51bbae393f6a</anchor>
      <arglist>(QString, QToolBar *&amp;)</arglist>
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
      <anchor>c35c704bd2f170f1d6751286768ca0bf</anchor>
      <arglist>(QString &amp;, QStringList &amp;)</arglist>
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
      <anchor>29a0d5cce8350b9ad40741e8dd80960f</anchor>
      <arglist>(QWidget *&amp;)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>toolboxActivated</name>
      <anchorfile>classToolboxInterface.html</anchorfile>
      <anchor>2c220f7bbae48edbbaadedaaee719db9</anchor>
      <arglist>(bool)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ViewInterface</name>
    <filename>classViewInterface.html</filename>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>getStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>49662427e98da03737e3bc2087f9d738</anchor>
      <arglist>(QString, QWidget *&amp;)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>updateStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>4b4b6a34c42b627b66416255df383394</anchor>
      <arglist>(QString, QWidget *)</arglist>
    </member>
    <member kind="signal" virtualness="virtual">
      <type>virtual void</type>
      <name>addStackWidget</name>
      <anchorfile>classViewInterface.html</anchorfile>
      <anchor>0fb0bff78c9c3b265fc9b87b8d4323a1</anchor>
      <arglist>(QString, QWidget *)</arglist>
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
      <type>DLLEXPORT ObjectIterator</type>
      <name>objects_end</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d1ebd6bab5cdb61704584c9f7a8fa1e3</anchor>
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
      <type>BaseObject *&amp;</type>
      <name>objectRoot</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>7db0b763935fa0d5933fac6f605b5cba</anchor>
      <arglist>()</arglist>
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
      <name>setViewers</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>8955188d50725b0bde4f048900bbd3dc</anchor>
      <arglist>(std::vector&lt; glViewer * &gt; _examiner_widgets)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b49f3df01bed203c614e7e6e748277fb</anchor>
      <arglist>(SeparatorNode *_root_node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSceneGraphRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>42b42db6bee14300b3c80a7e7a209041</anchor>
      <arglist>(SeparatorNode *_root_node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setActiveExaminer</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>b67d722dc9182c4973603519638e98f8</anchor>
      <arglist>(const unsigned int _id)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>activeExaminer</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c092e03e4f2fd8744c6ea4b721bf8c13</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>e20d1424a44165838009bc1828f982ff</anchor>
      <arglist>(ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr=0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scenegraph_pick</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>da942b5e50f2ca047ed4fc54b6834e51</anchor>
      <arglist>(const unsigned int _examiner, ACG::SceneGraph::PickTarget _pickTarget, const QPoint &amp;_mousePos, unsigned int &amp;_nodeIdx, unsigned int &amp;_targetIdx, ACG::Vec3d *_hitPointPtr=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>95bf07301b5f4e367e9d6909acbaf7fd</anchor>
      <arglist>(ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>traverse</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>d95c682f42a2936ed2a7ac62f3068f3d</anchor>
      <arglist>(const unsigned int _examiner, ACG::SceneGraph::MouseEventAction &amp;_action)</arglist>
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
      <name>pickMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>354c6201d4629d6607ebd0a43d2125e2</anchor>
      <arglist>(const unsigned int _examiner, std::string _mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>10625157f033ef4f8e3c73554450e1b0</anchor>
      <arglist>(Viewer::ActionMode _mode)</arglist>
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
      <type>Viewer::ActionMode</type>
      <name>actionMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a7ad2d5b69ca3cc449a3e702fb4e5d4f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Viewer::ViewerProperties &amp;</type>
      <name>viewerProperties</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>897bdc5ebf036348c0e543380c2a6cfa</anchor>
      <arglist>(int _id)</arglist>
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
      <name>getSceneGraphRootNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>db9160f9b0e24053d577b42efe1d37d4</anchor>
      <arglist>()</arglist>
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
      <name>addNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f0b8838cfc4bbe0cdb7b91815ecc8477</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addGlobalNode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>2458aef543b14b2ac0b06c50d168414e</anchor>
      <arglist>(ACG::SceneGraph::BaseNode *_node)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_picked_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5a5a6830d7f7289647fa6fecec7ab36c</anchor>
      <arglist>(const unsigned int _node_idx, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f781f21a8d2ac7ebb2c027284407ed0f</anchor>
      <arglist>(const int _id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteAll</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>5b045e2632c7378cc2e0150066233b47</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>copyObject</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>9e5cdfe8b47d65f62e51f87b88ef2bfa</anchor>
      <arglist>(const int _id)</arglist>
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
      <anchor>634e1aa080dbbd82e96144b0483fd085</anchor>
      <arglist>(const int _identifier, BaseObject *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>get_object</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>17125357788f11bf91380cd146aa6a3d</anchor>
      <arglist>(const int _identifier, BaseObjectData *&amp;_object)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>object_exists</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>924e6965de6c3a36291811c121ded9df</anchor>
      <arglist>(const int _identifier)</arglist>
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
      <type>QPoint</type>
      <name>mapToGlobal</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>499ada11f6b60e736f314d03395dd808</anchor>
      <arglist>(const QPoint _point)</arglist>
    </member>
    <member kind="function">
      <type>QPoint</type>
      <name>mapToLocal</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>6da01f4b28ec6cca2a6699cd7c93f244</anchor>
      <arglist>(const QPoint _point)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDrawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>a0d5f7587ac6b1cdd1ae5a84feb428ef</anchor>
      <arglist>(const unsigned int _mode)</arglist>
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
      <name>setGlobalDrawMode</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>47c8bcb0fcca09628f1f9470906d27df</anchor>
      <arglist>(const unsigned int _mode)</arglist>
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
      <name>setMainGLContext</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>bf5a1e13a33047b0af8399456e5363a0</anchor>
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
      <anchor>0b648e6b44a59282e5edfc8eda51c675</anchor>
      <arglist>(const ACG::Vec3d &amp;_center, const double _radius)</arglist>
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
      <anchor>5edf7564992ebddd3e5b5ed4207c72bc</anchor>
      <arglist>(const ACG::Vec3d &amp;_axis, const double _angle, const ACG::Vec3d &amp;_center)</arglist>
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
      <anchor>b06ba401a82fc57b90afa01f9e29af07</anchor>
      <arglist>(const ACG::Vec3d &amp;_position, const ACG::Vec3d &amp;_center, double _time)</arglist>
    </member>
    <member kind="function">
      <type>BaseObjectData *</type>
      <name>baseObjectData</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>ffb238a514f9754ca2a3680ccb4b59a4</anchor>
      <arglist>(BaseObject *_object)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static BaseObject *</type>
      <name>objectRoot_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>1becdbeb1c22e6b1a271ddb2226d0fe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::vector&lt; glViewer * &gt;</type>
      <name>examiner_widgets_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>6912bcdbedb83b624d8f3a9b7c171e23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static glViewer *</type>
      <name>examiner_widget_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>2e0fef01dee014b1d6be9603b3247fbc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static unsigned int</type>
      <name>activeExaminer_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>c0a023cf73492ad27d6f3141d783a954</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static SeparatorNode *</type>
      <name>root_node_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>f38b951861d0228b1a05909dff038ede</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static SeparatorNode *</type>
      <name>sceneGraph_root_node_</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>4db889b13389632daf3621ce0226c03e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static Viewer::ViewerProperties</type>
      <name>dummyProperties</name>
      <anchorfile>namespacePluginFunctions.html</anchorfile>
      <anchor>0034857058dc22d14b7ea49f473bc04b</anchor>
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
    <member kind="function" protection="private">
      <type>void</type>
      <name>proceedToNextBaseObjectData</name>
      <anchorfile>classPluginFunctions_1_1ObjectIterator.html</anchorfile>
      <anchor>007e520bda6bacfd4adeec1eb53ba261</anchor>
      <arglist>(BaseObject *&amp;_object)</arglist>
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
    <name>widgets/aboutWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/aboutWidget/</path>
    <filename>dir_594acadcb55f93dbf4e82c7ef03393dc.html</filename>
    <file>aboutWidget.cc</file>
    <file>aboutWidget.hh</file>
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
    <file>OptionsInterface.hh</file>
    <file>PickingInterface.hh</file>
    <file>PluginFunctions.cc</file>
    <file>PluginFunctions.hh</file>
    <file>PluginFunctionsCore.hh</file>
    <file>PluginFunctionsIterator.cc</file>
    <file>RPCInterface.hh</file>
    <file>ScriptInterface.hh</file>
    <file>StatusbarInterface.hh</file>
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
    <name>common/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/common/</path>
    <filename>dir_1be831ad6813636322aff3cfbb793ba7.html</filename>
    <dir>common/bsp/</dir>
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
    <file>ObjectIDPointerManagerT.cc</file>
    <file>ObjectIDPointerManagerT.hh</file>
    <file>perObjectData.cc</file>
    <file>perObjectData.hh</file>
    <file>RecentFiles.cc</file>
    <file>RecentFiles.hh</file>
    <file>Types.cc</file>
    <file>Types.hh</file>
    <file>ViewerProperties.cc</file>
    <file>ViewerProperties.hh</file>
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
    <file>ParseObj.cc</file>
    <file>PluginCommunication.cc</file>
    <file>PluginInfo.hh</file>
    <file>PluginLoader.cc</file>
    <file>RPC.cc</file>
    <file>saveFunctions.cc</file>
    <file>saveSettings.cc</file>
    <file>scripting.cc</file>
  </compound>
  <compound kind="dir">
    <name>widgets/coreWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/coreWidget/</path>
    <filename>dir_56a69f0fbb49fda25c91167fa202740d.html</filename>
    <file>About.cc</file>
    <file>ContextMenu.cc</file>
    <file>CoreWidget.cc</file>
    <file>CoreWidget.hh</file>
    <file>CoreWidgetLogging.cc</file>
    <file>CoreWidgetToolbar.cc</file>
    <file>dragAndDrop.cc</file>
    <file>Help.cc</file>
    <file>keyHandling.cc</file>
    <file>MenuBar.cc</file>
    <file>StackWidget.cc</file>
    <file>StatusBar.cc</file>
    <file>viewerControl.cc</file>
    <file>viewMode.cc</file>
  </compound>
  <compound kind="dir">
    <name>Doxygen/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Doxygen/</path>
    <filename>dir_15ee26eb48dc46c47c455f7f6deafd0c.html</filename>
    <dir>Doxygen/example/</dir>
    <file>building.docu</file>
    <file>dataFlow.docu</file>
    <file>dataStructure.docu</file>
    <file>Interfaces.docu</file>
    <file>mainpage.docu</file>
    <file>PluginProgramming.docu</file>
  </compound>
  <compound kind="dir">
    <name>Doxygen/example/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Doxygen/example/</path>
    <filename>dir_c7b53bd8fb9bdaa6c3d877caf61c300f.html</filename>
    <file>simplePlugin.cc</file>
    <file>simplePlugin.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/glWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/glWidget/</path>
    <filename>dir_79c40a2d092ad1d2d0a048801101ad10.html</filename>
    <file>QtBaseViewer.cc</file>
    <file>QtBaseViewer.hh</file>
    <file>QtBaseViewerPicking.cc</file>
    <file>QtBaseViewerStereo.cc</file>
    <file>QtGLGraphicsScene.cc</file>
    <file>QtGLGraphicsScene.hh</file>
    <file>QtGLGraphicsView.cc</file>
    <file>QtGLGraphicsView.hh</file>
    <file>QtGLViewerLayout.cc</file>
    <file>QtGLViewerLayout.hh</file>
    <file>QtMultiViewLayout.cc</file>
    <file>QtMultiViewLayout.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/helpBrowser/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/helpBrowser/</path>
    <filename>dir_8b62dc7f51b1e3f1ac6e79adf1dca1fc.html</filename>
    <file>findEdit.cc</file>
    <file>findEdit.hh</file>
    <file>helpWidget.cc</file>
    <file>helpWidget.hh</file>
    <file>webView.cc</file>
    <file>webView.hh</file>
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
  </compound>
  <compound kind="dir">
    <name>widgets/loggerWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/loggerWidget/</path>
    <filename>dir_3ad0f73889eb06bb2591237bc66f5857.html</filename>
    <file>loggerWidget.cc</file>
    <file>loggerWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>Logging/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Logging/</path>
    <filename>dir_b907f522836bf04bb84548b61670133c.html</filename>
    <file>Logging.hh</file>
    <file>PluginLogging.cc</file>
  </compound>
  <compound kind="dir">
    <name>widgets/optionsWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/optionsWidget/</path>
    <filename>dir_5b7a6c4897525f4cca230d90bfe7b131.html</filename>
    <file>downloader.cc</file>
    <file>keyBindings.cc</file>
    <file>optionsWidget.cc</file>
    <file>optionsWidget.hh</file>
    <file>ShortcutButton.cc</file>
    <file>ShortcutButton.hh</file>
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
    <file>prototypeIdList.cc</file>
    <file>prototypeIdList.hh</file>
    <file>prototypeMatrix4x4.cc</file>
    <file>prototypeMatrix4x4.hh</file>
    <file>prototypeVec3d.cc</file>
    <file>prototypeVec3d.hh</file>
  </compound>
  <compound kind="dir">
    <name>Scripting/scriptWrappers/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/Scripting/scriptWrappers/</path>
    <filename>dir_08300fed192bcb66f6137dbe84f57711.html</filename>
    <file>idListWrapper.cc</file>
    <file>idListWrapper.hh</file>
    <file>matrix4x4Wrapper.cc</file>
    <file>matrix4x4Wrapper.hh</file>
    <file>vec3dWrapper.cc</file>
    <file>vec3dWrapper.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/unloadPluginsWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/unloadPluginsWidget/</path>
    <filename>dir_3537f2718c44bf5387a0c1cd04e8438d.html</filename>
    <file>unloadPluginsWidget.cc</file>
    <file>unloadPluginsWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/viewModeWidget/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/viewModeWidget/</path>
    <filename>dir_d90a30b42ac5271b0fe6b95a4c309e07.html</filename>
    <file>viewModeWidget.cc</file>
    <file>viewModeWidget.hh</file>
  </compound>
  <compound kind="dir">
    <name>widgets/</name>
    <path>/data/home1/moebius/projects/OpenFlipper/OpenFlipper/widgets/</path>
    <filename>dir_7ca3df6b42838197b5d9bc81f4b4838a.html</filename>
    <dir>widgets/aboutWidget/</dir>
    <dir>widgets/addEmptyWidget/</dir>
    <dir>widgets/coreWidget/</dir>
    <dir>widgets/glWidget/</dir>
    <dir>widgets/helpBrowser/</dir>
    <dir>widgets/loadWidget/</dir>
    <dir>widgets/loggerWidget/</dir>
    <dir>widgets/optionsWidget/</dir>
    <dir>widgets/unloadPluginsWidget/</dir>
    <dir>widgets/viewModeWidget/</dir>
  </compound>
</tagfile>
