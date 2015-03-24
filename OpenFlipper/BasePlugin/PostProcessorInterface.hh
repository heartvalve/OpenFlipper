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


#ifndef POSTPROCESSORINTERFACE_HH
#define POSTPROCESSORINTERFACE_HH

#include <ACG/GL/GLState.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <QAction>

/** \file PostProcessorInterface.hh
*
* Interface for adding global image post processor functions. \ref postProcessorInterfacePage
*/


struct DLLEXPORT PostProcessorInput
{
  PostProcessorInput(GLuint _colTex = 0, 
                     GLuint _depthTex = 0,
                     int _width = 0,
                     int _height = 0);

  // scene colors
  GLuint colorTex_;

  // non-linear depth values directly from the depth buffer
  GLuint depthTex_;

  int width, height;

  // multisampling count
  int sampleCount_;

  // format of color tex
  GLuint texfmt_;


  // view and projection matrix
  ACG::GLMatrixf view_;
  ACG::GLMatrixf proj_;
  float depthRange_[2];


  // bind color texture to a given texture-slot (0, 1, 2, ..)
  void bindColorTex(int _texSlot = 0) const;

  // bind non-linear depth texture to a given texture-slot (0, 1, 2, ..)
  void bindDepthTex(int _texSlot = 0) const;
};


struct DLLEXPORT PostProcessorOutput 
{
  PostProcessorOutput(GLuint _fbo = 0, 
    GLuint _drawBuffer = 0,
    int _width = 0,
    int _height = 0,
    const GLint* _viewport = 0);

  // opengl fbo id
  GLuint fbo_;

  // draw target of fbo: GL_BACK, GL_FRONT, GL_COLOR_ATTACHMENT0..
  GLuint drawBuffer_;

  // target viewport
  GLint viewport_[4];

  int width, height;


  // bind fbo, drawbuffer and viewport
  void bind() const;
};


// in/out format descriptor
struct PostProcessorFormatDesc
{
  PostProcessorFormatDesc(bool _multisampled = false) : supportMultisampling_(_multisampled)
  {}


  enum Format 
  {
    PostProcessorFormat_DONTCARE, // the postprocessor works for any format type (default)
    PostProcessorFormat_FLOAT,    // expect format in fp32 or fp16 format,  used by tonemapper for example
//    PostProcessorFormat_UNORM,    // expect format in normalized ubyte4 format
  };

  // expected format of each input
  std::vector<Format> inputFormats_;

  // post-processor supports multisampled input
  bool supportMultisampling_;

  // output format
  Format outputFormat_;
};


/** \brief Interface to add global image post processor functions from within plugins.
 *
 * \ref postProcessorInterfacePage "Detailed description"
 * \n
 * \n
 *
 * Interface for adding and controlling global image post processor functions from a plugin.\n
*/
class PostProcessorInterface {

  public:

    /// Destructor
    virtual ~PostProcessorInterface() {};
 
  public slots:

    /** \brief post processor function
     *
     */
    virtual void postProcess(ACG::GLState* _glState, const std::vector<const PostProcessorInput*>& _input, const PostProcessorOutput& _output) = 0;

    /** \brief announce name for the postProcessor function
     *
     * @return The name of the post processor
     */
    virtual QString postProcessorName() = 0;


    /** \brief does post processor resolve stereo buffer
     *
     * The post processor gets the left and right image as input and composes a combined stereo output.
     * 
     * @return true if implementation resolves stereo buffer, false otherwise
     */
    virtual bool isStereoProcessor() {return false;}


    /* \brief Provide in/out format info
     *
     * The postprocessor might require non-standard in/out formats, 
     * which can be described by initializing a descriptor with the in/out formats.
     * This is entirely optional, is is assumed that the postprocessor does not care about the formats per default.
     *
    */
    virtual void getFormatDesc(PostProcessorFormatDesc* _desc) {}


    /** \brief Return options menu
     *
     * If you want an options Menu or menu entry, you can return your action here.
     * It will be shown on top of the post processors list in the options menu.
     *
     *
     * @return Action for a menu or menu entry
     */
    virtual QAction* optionsAction() { return 0; };

  private slots:

    /** \brief Check OpenGL capabilities
     *
     * This function has to be implemented and checks, if all required OpenGL extensions are available.
     * If this is not the case, the plugin will be refused by the core to avoid crashes due to insufficient
     * OpenGL support.
     *
     * You can get the version information in the following way:
     *
     * \code
     *
     * // Check version
     * if ( ! ACG::openGLVersion(2, 1) )
     *   return QString("Insufficient OpenGL Version! OpenGL 2.1 or higher required");
     *
     * //Get OpenGL extensions
     * QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
     *
     * // Collect missing extension
     * QString missing = "";
     *
     * if ( !glExtensions.contains("GL_ARB_vertex_buffer_object") )
     *   missing += "Missing Extension GL_ARB_vertex_buffer_object\n";
     *
     * if ( !glExtensions.contains("GL_ARB_vertex_program") )
     *   missing += "Missing Extension GL_ARB_vertex_program\n";
     *
     * return missing;
     * \endcode
     *
     * @return Return an empty string if everything is fine, otherwise return, what features are missing.
     */
    virtual QString checkOpenGL() = 0;
};

/** \page postProcessorInterfacePage Post Processor Interface
\image html PostProcessorInterface.png
\n

The PostProcessor Interface can be used to add additional image post processing functions
to OpenFlipper. For example you get the rendered image in the usual buffers. Afterwards you can
add a post processor doing for example edge detection or taking the depth image from the
buffer and render it to the main image.


Example Code for functions:
\code
 
 // fetch shader from cache:
 //  default screenquad vertex shader
 //  custom postprocessor fragment shader stored in "Shaders/MyPostProcessor/shader.glsl"
 GLSL::Program* shader = ACG::ShaderCache::getInstance()->getProgram("ScreenQuad/screenquad.glsl", "MyPostProcessor/shader.glsl");


 // bind color texture input to texture-unit 0
 _input[0]->bindColorTex(0);


 // bind output FBO
 _output.bind();


 // setup shader uniforms
 shader->use();
 shader->setUniform("param0", someValue);
 shader->setUniform("texInput", 0); // color texture was bound to unit 0

 // execute by drawing a screen quad covering the full viewport
 ACG::ScreenQuad::draw(shader);

\endcode


To use the PostProcessorInterface:
<ul>
<li> include PostProcessorInterface.hh in your plugins header file
<li> derive your plugin from the class PostProcessorInterface
<li> add Q_INTERFACES(PostProcessorInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(PostProcessorInterface,"OpenFlipper.PostProcessorInterface/1.0")

#endif // RPCINTERFACE_HH
