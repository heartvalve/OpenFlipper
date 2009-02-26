/*******************************************************************************
 * GLSLShader.h
 *
 * Utility classes for GLSL shaders.
 *
 * (C)2007
 * Lehrstuhl I8 RWTH-Aachen, http://www-i8.informatik.rwth-aachen.de
 * Author: Markus Tavenrath <speedygoo@speedygoo.de>
 *
 ******************************************************************************/

#ifndef GLSLSHADER_H
#define GLSLSHADER_H

//==============================================================================

#include "../Config/ACGDefines.hh"
#include <ACG/Math/VectorT.hh>

#include <list>
#include <string>

//==============================================================================

/** \brief This namespace contains all the classes and functions for handling
 * GLSL shader and program objects.
 */
namespace GLSL {

#define GLSL_MAX_LOGSIZE 16384

    typedef std::list<std::string> StringList;

    /** \brief A generic shader base class
     */
    class ACGDLLEXPORT Shader
    {
    public:
        Shader(GLenum shaderType);
        virtual ~Shader();
        void setSource(StringList source);

        // FIXME implement StringList getSource();
        bool compile();

    protected:
        GLuint m_shaderId;

        friend class Program;
    };

    typedef Shader* PtrShader;
    typedef const Shader* PtrConstShader;

    //--------------------------------------------------------------------------

    /** \brief GLSL vertex shader.
     */
    class ACGDLLEXPORT VertexShader
        : public Shader
    {
    public:
        VertexShader();
        virtual ~VertexShader();
    };

    typedef VertexShader* PtrVertexShader;
    typedef const VertexShader* PtrVertexConstShader;

    //--------------------------------------------------------------------------

    /** \brief GLSL fragment shader.
     */
    class ACGDLLEXPORT FragmentShader : public Shader {
    public:
        FragmentShader();
        virtual ~FragmentShader();
    };

    typedef FragmentShader* PtrFragmentShader;
    typedef const FragmentShader* PtrConstFragmentShader;

    //--------------------------------------------------------------------------

    /** \brief GLSL program class.
     *
     * A GLSL program links together the vertex and fragment shaders.
     */
    class ACGDLLEXPORT Program
    {
    public:
        Program();
        virtual ~Program();

        void attach(PtrConstShader shader);
        void detach(PtrConstShader shader);
        void link();

        void setUniform(const char *name, GLint value);
        void setUniform(const char *name, GLfloat value);
        void setUniform(const char *name, const ACG::Vec3f &value);

        void setUniform(const char *name, GLint *value, int count);

        // set element of array
        void setUniform(const char *name, int index, bool value);
        void setUniform(const char *name, int index, int value);
        void setUniform(const char *name, int index, float value);

        void bindAttributeLocation(unsigned int index, const char *name);
        int getAttributeLocation(const char *name);
        int getUniformLocation(const char *name);

        void use();

        void disable();

        bool isActive();

    private:

        std::list<PtrConstShader> m_linkedShaders;
        GLint m_programId;
    };

    typedef Program* PtrProgram;
    typedef const Program* PtrConstProgram;

    //--------------------------------------------------------------------------

    GLSL::StringList loadShader(const char *filename);

    GLSL::PtrVertexShader loadVertexShader(const char *name);
    GLSL::PtrFragmentShader loadFragmentShader(const char *name);

}


//==============================================================================

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

//==============================================================================

// GLSLSHADER_H
#endif
