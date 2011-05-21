#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_EFFECT_SHADER_PROGRAM_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_EFFECT_SHADER_PROGRAM_H__

#include <sgl/Program.h>
#include <functional>
#include <string>
#include "../../../Utility/error.hpp"
#include "../../Detail/AttributeTable.h"

namespace slon {
namespace graphics {

/** Base class for uber shaders */
class EffectShaderProgram :
    public Referenced
{
public:
    struct uber_program_hash
    {
    public:
        typedef std::vector<std::string>        string_vector;
        typedef string_vector::iterator         string_iterator;
        typedef string_vector::const_iterator   const_string_iterator;

    public:
        template<typename DefinitionIter, typename ShaderIter>
        uber_program_hash(DefinitionIter    definitionsBegin,
                          DefinitionIter    definitionsEnd,
                          ShaderIter        shadersBegin,
                          ShaderIter        shaderEnd) :
            definitions(definitionsBegin, definitionsEnd),
            shaders(shadersBegin, shaderEnd)
        {
            // create hash
            stringHash = 5381;
            {
                for (size_t i = 0; i<definitions.size(); ++i)
                {
                    for (size_t j = 0; j<definitions[i].length(); ++j)
                    {
                        int c = definitions[i][j];
                        stringHash = ((stringHash << 5) + stringHash) + c;
                    }
                }

                for (size_t i = 0; i<shaders.size(); ++i)
                {
                    for (size_t j = 0; j<shaders[i].length(); ++j)
                    {
                        int c = shaders[i][j];
                        stringHash = ((stringHash << 5) + stringHash) + c;
                    }
                }
            }
        }

        // for storing in map
        bool operator < (const uber_program_hash& rhs) const
        {
            // num definitions
            if ( definitions.size() < rhs.definitions.size() ) {
                return true;
            }
            else if ( definitions.size() > rhs.definitions.size() ) {
                return false;
            }

            // num shaders
            if ( shaders.size() < rhs.shaders.size() ) {
                return true;
            }
            else if ( shaders.size() > rhs.shaders.size() ) {
                return false;
            }
/*
            // hash
            if ( stringHash < rhs.stringHash ) {
                return true;
            }
*/
            // brute force check
            for (size_t i = 0; i<definitions.size(); ++i) 
            {
                if ( definitions[i] < rhs.definitions[i] ) {
                    return true;
                }
                else if ( definitions[i] > rhs.definitions[i] ) {
                    return false;
                }
            }

            for (size_t i = 0; i<shaders.size(); ++i) 
            {
                if ( shaders[i] < rhs.shaders[i] ) {
                    return true;
                }
            }

            return false;
        }

        const_string_iterator firstDefinition() const   { return definitions.begin(); }
        const_string_iterator endDefinition() const     { return definitions.end(); }

        const_string_iterator firstShader() const       { return shaders.begin(); }
        const_string_iterator endShader() const         { return shaders.end(); }

    private:
        string_vector   definitions;
        string_vector   shaders;
        unsigned long   stringHash;
    };

    typedef sgl::ref_ptr<sgl::Program>                          program_ptr;
    typedef std::vector<detail::AttributeTable::binding_ptr>    binding_vector;

public:
    /** Can throw shader_error, file_not_found_error. Log compilation status if it is not empty. */
    EffectShaderProgram(const log::logger_ptr& effectLogger);
    virtual ~EffectShaderProgram() {}

    /** Add preprocessor definition to the program. */
    void addDefinition(const std::string& definition);

    /** Add preprocessor definition to the program using printf style.
     * @return true if definition succesfully formatted and added.
     */
    template<typename T0>
    bool addDefinition(const std::string& format, T0 value0);

    /** Add preprocessor definition to the program using printf style.
     * @return true if definition succesfully formatted and added.
     */
    template<typename T0, typename T1>
    bool addDefinition(const std::string& format, T0 value0, T1 value1);
    
    /** Add preprocessor definition to the program using printf style.
     * @return true if definition succesfully formatted and added.
     */
    template<typename T0, typename T1, typename T2>
    bool addDefinition(const std::string& format, T0 value0, T1 value1, T2 value2);
   
    /** Remove preprocessor definition from the program. */
    bool removeDefinition(const std::string& definition);

    /** Remove all definitions from the program. */
    void removeDefinitions();

    /** Add shader to the program.
     * @param shader - shader file name.
     */
    void addShader(const std::string& shader);
    
    /** Remove shader from the program.
     * @param shader - shader file name.
     */
    bool removeShader(const std::string& shader);

    /** Remove all shaders from the program. */
    void removeShaders();

    /** Construct uber program */
    void constructProgram();

    /** Get shader program. */
    const sgl::Program* getProgram() const;

    /** Load uniform. */
    template<typename type>
    sgl::Uniform<type>* loadUniform(const char* uniformName)
    {
        return sgl::Program::GetUniform<type>( program.get(), uniformName );
    }

    /** Load sampler uniform. */
    template<typename type>
    sgl::SamplerUniform<type>* loadSamplerUniform(const char* uniformName)
    {
        return sgl::Program::GetSamplerUniform<type>( program.get(), uniformName );
    }

    /** Load uniform. throw error, if uniform not loaded. */
    template<typename type>
    sgl::Uniform<type>* strictLoadUniform(const char* uniformName)
    {
        sgl::Uniform<type>* uniform = sgl::Program::GetUniform<type>( program.get(), uniformName );
        if (!uniform) {
            throw shader_error(effectLogger, std::string("Can't get shader uniform ") + uniformName);
        }
        return uniform;
    }

    /** Load uniform. throw error, if uniform not loaded. */
    template<typename sampler_type>
    sgl::SamplerUniform<sampler_type>* strictLoadSamplerUniform(const char* uniformName)
    {
        sgl::SamplerUniform<sampler_type>* uniform = sgl::Program::GetSamplerUniform<sampler_type>( program.get(), uniformName );
        if (!uniform) {
            throw shader_error(effectLogger, std::string("Can't get shader uniform ") + uniformName);
        }
        return uniform;
    }

protected:
    /** Redirect uniforms after shader construction */
    virtual void redirectUniforms() {};

protected:
    // error logging
    mutable log::logger_ptr     effectLogger;
    bool                        dirty;

    // shader
    program_ptr                 program;
    std::vector<std::string>    definitions;
    std::vector<std::string>    shaders;

    // used attributes
    binding_vector              bindings;
};

template<typename T0>
bool EffectShaderProgram::addDefinition(const std::string&  format,
                                        T0                  value0)
{
    std::string buffer(256, ' ');
    int written = sprintf(&buffer[0], format.c_str(), value0);
    if (written > 0)
    {
        buffer.resize(written);
        addDefinition(buffer);
        return true;
    }
    return false;
}

template<typename T0, typename T1>
bool EffectShaderProgram::addDefinition(const std::string&  format,
                                        T0                  value0, 
                                        T1                  value1)
{
    std::string buffer(256, ' ');
    int written = sprintf(&buffer[0], format.c_str(), value0, value1);
    if (written > 0)
    {
        buffer.resize(written);
        addDefinition(buffer);
        return true;
    }
    return false;
}

template<typename T0, typename T1, typename T2>
bool EffectShaderProgram::addDefinition(const std::string&  format,
                                        T0                  value0, 
                                        T1                  value1, 
                                        T2                  value2)
{
    std::string buffer(256, ' ');
    int written = sprintf(&buffer[0], format.c_str(), value0, value1, value2);
    if (written > 0)
    {
        buffer.resize(written);
        addDefinition(buffer);
        return true;
    }
    return false;
}

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_EFFECT_SHADER_PROGRAM_H__
