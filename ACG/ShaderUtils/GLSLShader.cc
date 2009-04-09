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

//==============================================================================

#include <QApplication>
#include <QDir>
#include <QString>

#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>

#include <../GL/gl.hh>

#include "gldebug.h"
#include "GLSLShader.hh"

#ifdef WIN32
#define snprintf sprintf_s
#endif

//==============================================================================

namespace GLSL {

    //--------------------------------------------------------------------------
    // Generic shader
    //--------------------------------------------------------------------------

    /** \brief Creates a new shader.
     *
     * \param[in]  shaderType   Can be one of GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     */
    Shader::Shader(GLenum shaderType)
        : m_shaderId(0)
    {
        this->m_shaderId = glCreateShader(shaderType);
        assert(this->m_shaderId && "could not create shader");
    }

    /** \brief Deletes the shader object.
     */
    Shader::~Shader()
    {
        if (this->m_shaderId) {
            glDeleteShader(m_shaderId);
        }
    }

    /** \brief Upload the source of the shader.
     */
    void Shader::setSource(StringList source)
    {
        assert(this->m_shaderId && "shader not initialized");

        const char **stringArray = new const char*[source.size()];

        int index = 0;
        for (StringList::const_iterator it = source.begin();it != source.end();++it) {
            stringArray[index] = (*it).c_str();
            ++index;
        }

        glShaderSource(this->m_shaderId, source.size(), stringArray, 0);

        delete[] stringArray;
    }

    /** \brief Compile the shader object.
     *
     * \returns True if compilation was successful, or false if it failed. Also
     * it prints out the shader source and the error message.
     */
    bool Shader::compile()
    {
        assert(this->m_shaderId && "shader not initialized");

        glCompileShader(this->m_shaderId);

        GLint compileStatus;
        glGetShaderiv(this->m_shaderId, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE) {
            GLchar *errorLog = new GLchar[GLSL_MAX_LOGSIZE];
            GLsizei errorLength;
            char shaderSource[32768];
            glGetShaderSource(this->m_shaderId, 32768, &errorLength, shaderSource);
            std::cout << "shader source: " << std::endl << shaderSource << std::endl;
            glGetShaderInfoLog(this->m_shaderId, GLSL_MAX_LOGSIZE, &errorLength, errorLog);
            std::cout << "GLSL compile error:" << std::endl << errorLog << std::endl;
            delete[] errorLog;

            return false;
        }
        return true;
    }

    //--------------------------------------------------------------------------
    // Vertex shader
    //--------------------------------------------------------------------------

    VertexShader::VertexShader() : Shader(GL_VERTEX_SHADER) {}
    VertexShader::~VertexShader() {}

    //--------------------------------------------------------------------------
    // Fragment shader
    //--------------------------------------------------------------------------

    FragmentShader::FragmentShader() : Shader(GL_FRAGMENT_SHADER) {}
    FragmentShader::~FragmentShader() {}

    //--------------------------------------------------------------------------
    // Shader program object
    //--------------------------------------------------------------------------

    /** \brief Creates a new GLSL program object.
     */
    Program::Program()
    {
        this->m_programId = glCreateProgram();
        assert(this->m_programId && "could not create GLSL program");
    }

    /** \brief Deletes the GLSL program object and frees the linked shader objects.
     */
    Program::~Program()
    {
        if (this->m_programId) {
            glDeleteProgram(this->m_programId);
        }
        // free linked shaders
        this->m_linkedShaders.clear();
    }

    /** \brief Attaches a shader object to the program object.
     */
    void
    Program::attach(PtrConstShader shader)
    {
        assert(this->m_programId && "attach invalid program");
        assert(shader->m_shaderId && "attach invalid shader");

        glAttachShader(this->m_programId, shader->m_shaderId);
        m_linkedShaders.push_back(shader);
    }

    /** \brief Detaches a shader object from the program object.
     */
    void Program::detach(PtrConstShader shader)
    {
        assert(this->m_programId && "detach invalid program");
        assert(shader->m_shaderId && "detach invalid shader");

        glDetachShader(this->m_programId, shader->m_shaderId);
        m_linkedShaders.remove(shader);
    }

    /** \brief Links the shader objects to the program.
     */
    void Program::link()
    {
        glLinkProgram(this->m_programId);
        checkGLError2("link program failed");
    }

    /** \brief Enables the program object for using.
     */
    void Program::use()
    {
        glUseProgram(this->m_programId);
        checkGLError2("use program failed");
    }

    /** \brief Resets to standard rendering pipeline
     */
    void Program::disable() {
        glUseProgram(0);
        checkGLError2("shader disable failed");
    }

    /** \brief Returns if the program object is currently active.
     */
    bool Program::isActive()
    {
        GLint programId;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
        return programId == this->m_programId;
    }

    /** \brief Sets a uniform variable for the program.
     */
    void Program::setUniform(const char *name, GLint value)
    {
        checkGLError();
        GLint location = glGetUniformLocation(this->m_programId, name);
        checkGLError2(name);
        glUniform1i(location, value);
        checkGLError2(name);
    }

    /** \brief Sets a uniform variable for the program.
     */
    void Program::setUniform(const char *name, GLfloat value)
    {
        checkGLError2("prev opengl error");
        GLint location = glGetUniformLocation(this->m_programId, name);
        checkGLError2(name);
        glUniform1f(location, value);
        checkGLError2(name);
    }

    /** \brief Sets a uniform variable for the program.
     */
    void Program::setUniform(const char *name, const ACG::Vec3f &value)
    {
        checkGLError();
        GLint location = glGetUniformLocation(this->m_programId, name);
        checkGLError2(name);
        glUniform3fv(location, 1, value.data());
        checkGLError();
    }

    void Program::setUniform(const char *name, GLint *values, int count)
    {
        checkGLError();
        GLint location = glGetUniformLocation(this->m_programId, name);
        checkGLError2(name);
        glUniform1iv(location, count, values);
        checkGLError();
    }

    void Program::setUniform(const char *name, int index, bool value)
    {
        char varName[1024];
        snprintf(varName, 1024, "%s[%d]", name, index);
        setUniform(varName, (GLint) value);
    }

    void Program::setUniform(const char *name, int index, int value)
    {
        char varName[1024];
        snprintf(varName, 1024, "%s[%d]", name, index);
        setUniform(varName, (GLint) value);
    }

    void Program::setUniform(const char *name, int index, float value)
    {
        char varName[1024];
        snprintf(varName, 1024, "%s[%d]", name, index);
        setUniform(varName, value);
    }

    void Program::bindAttributeLocation(unsigned int index, const char *name) {
        glBindAttribLocation(this->m_programId, index, name);
        checkGLError2(name);
    }

    int Program::getAttributeLocation(const char *name) {
        int attributeLocation = glGetAttribLocationARB(this->m_programId, name);
        checkGLError2(name);
        return attributeLocation;
    }

    int Program::getUniformLocation(const char *name) {
        int attributeLocation = glGetUniformLocationARB(this->m_programId, name);
        checkGLError2(name);
        return attributeLocation;
    }

    /** \brief Loads the shader source
     *
     * The shader is assumed to be placed in ../shader relative to the
     * executable's installation directory, if the path is a relativ
     * one. If it is determined that the path is absolute, the path is
     * taken as is.
     */
    GLSL::StringList loadShader(const char *filename)
    {
        QString path_file;
        if (QDir(filename).isRelative()) {
            path_file = qApp->applicationDirPath() + QString("/../shader/")
                + QString(filename);
        } else {
            path_file = QString::fromAscii(filename);
        }
        std::ifstream iShader(path_file.toAscii());
        if (!iShader) {
            std::cout << "ERROR: Could not open file " << path_file.toStdString() << std::endl;
            return GLSL::StringList();
        }

        GLSL::StringList shaderSource;

        while (!iShader.eof()) {
            std::string strLine;
            std::getline(iShader, strLine);
            strLine += "\n";
            shaderSource.push_back(strLine);
        }
        iShader.close();
        return shaderSource;
    }

    /** \brief Loads, compiles and installs a new vertex shader.
     */
    GLSL::PtrVertexShader loadVertexShader(const char *name)
    {
        PtrVertexShader vertexShader = 0;
        StringList sourceVertex = loadShader(name);

        if (sourceVertex.size() != 0) {
            vertexShader = GLSL::PtrVertexShader(new GLSL::VertexShader());
            vertexShader->setSource(sourceVertex);
            vertexShader->compile();
        }
        return vertexShader;
    }

    /** \brief Loads, compiles and installs a new vertex shader.
     */
    GLSL::PtrFragmentShader loadFragmentShader(const char *name)
    {
        PtrFragmentShader fragmentShader = 0;
        StringList sourceVertex = loadShader(name);

        if (sourceVertex.size() != 0) {
            fragmentShader = GLSL::PtrFragmentShader(new GLSL::FragmentShader());
            fragmentShader->setSource(sourceVertex);
            fragmentShader->compile();
        }
        return fragmentShader;
    }

}


//==============================================================================

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

//==============================================================================
