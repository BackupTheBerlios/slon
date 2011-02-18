#include "stdafx.h"
#include "Graphics/Effect/Detail/EffectShaderProgram.h"
#include "Graphics/Common.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystemManager.h"
#include <map>

using namespace slon;
using namespace slon::graphics;

namespace {

    inline std::string mergeShaderNames( const std::string&   vertexShaderFileName,
                                         const std::string&   fragmentShaderFileName,
                                         const std::string&   geometryShaderFileName )
    {
        std::string fileNames = "( ";
        if ( !vertexShaderFileName.empty() ) {
            fileNames += vertexShaderFileName + ", ";
        }
        if ( !fragmentShaderFileName.empty() ) {
            fileNames += fragmentShaderFileName + ", ";
        }
        if ( !geometryShaderFileName.empty() ) {
            fileNames += geometryShaderFileName + ", ";
        }
        fileNames[ fileNames.length() - 2 ] = ' ';
        fileNames[ fileNames.length() - 1 ] = ')';

        return fileNames;
    }

    inline std::string readFile(log::Logger& logger, const std::string& fileName)
	{
		std::string source;

        filesystem::file_ptr file( asFile( filesystem::currentFileSystemManager().getNode(fileName.c_str()) ) );
		if ( file && file->open(filesystem::File::in) ) 
		{
			source.resize( (size_t)file->size() );
			file->read( &source[0], file->size() );
		}

		return source;
    }

    inline sgl::Shader::TYPE getShaderType(log::Logger& logger, const std::string& fileName)
    {
        size_t length = fileName.length();
        if (length >= 4)
        {
            if ( fileName.substr(length - 4, 4) == "vert" ) {
                return sgl::Shader::VERTEX;
            }
            else if ( fileName.substr(length - 4, 4) == "frag" ) {
                return sgl::Shader::FRAGMENT;
            }
            else if ( fileName.substr(length - 4, 4) == "geom" ) {
                return sgl::Shader::GEOMETRY;
            }
        }

        throw shader_error(logger, "Can't determin shader type by file extension: " + fileName);
    }

    // make uber program from hash
    struct make_uber_program :
        public std::unary_function<void, EffectShaderProgram::program_ptr>
    {
        make_uber_program(log::Logger& _logger, const EffectShaderProgram::uber_program_hash& _hash) :
            logger(_logger),
            hash(_hash)
        {}

        EffectShaderProgram::program_ptr operator () ()
        {
            typedef EffectShaderProgram::uber_program_hash uber_program_hash;

            sgl::Device* device = currentDevice();

            // make definitions source
            std::string definitions;
            for (uber_program_hash::const_string_iterator iter  = hash.firstDefinition();
                                                          iter != hash.endDefinition();
                                                          ++iter )
            {
                definitions += (*iter) + "\n";
            }

            // make program
            sgl::Program* program = device->CreateProgram();
            {
                std::string shaderNames;
                for (uber_program_hash::const_string_iterator iter  = hash.firstShader();
                                                              iter != hash.endShader();
                                                              ++iter )
                {
                    std::string shaderSource = definitions + readFile(logger, *iter);

                    sgl::Shader::DESC desc;
                    desc.type   = getShaderType(logger, *iter);
                    desc.source = shaderSource.c_str();

                    // compile shader
                    sgl::Shader* shader = device->CreateShader(desc);
                    if (!shader)
                    {
                        std::string errorMsg( sglGetErrorMsg() );
                        throw shader_error(logger, "Can't create shader(" + *iter + "):" + errorMsg);
                    }
                    logger << log::S_NOTICE << "Shader compilation log(" + *iter + "):\n"
                                             << shader->CompilationLog() << std::endl;

                    program->AddShader(shader);
                    shaderNames += *iter + ",";
                }

                // remove last comma
                if (shaderNames.length() > 0) {
                    shaderNames.resize(shaderNames.length() - 1);
                }

                // compile program
                if ( sgl::SGL_OK != program->Dirty() ) {
                    throw shader_error(logger, "Can't link shader program(" + shaderNames + "):\n" + program->CompilationLog() );
                }
                logger << log::S_NOTICE << "Program compilation log(" + shaderNames + "):\n"
                                         << program->CompilationLog() << std::endl;

                // bind all attributes to the default slots
                for (size_t i = 0; i<program->NumAttributes(); ++i)
                {
                    sgl::Program::ATTRIBUTE attribute = program->Attribute(i);
                    bindings.push_back( detail::currentAttributeTable().queryAttribute( unique_string(attribute.name) ) );

                    if (bindings.back()->index != attribute.index) {
                        program->BindAttributeLocation(attribute.name, bindings.back()->index);
                    }
                }

                // relink program
                if ( sgl::SGL_OK != program->Dirty() ) {
                    throw shader_error(logger, "Can't link shader program(" + shaderNames + "):\n" + program->CompilationLog() );
                }
            }
            return sgl::ref_ptr<sgl::Program>(program);
        }

    private:
        log::Logger&                                    logger;
        const EffectShaderProgram::uber_program_hash&   hash;

    public:
        EffectShaderProgram::binding_vector             bindings;
    };

    typedef std::map<EffectShaderProgram::uber_program_hash, EffectShaderProgram::program_ptr> program_cache;
    static program_cache programCache;

    template<typename ProgramContructor>
    EffectShaderProgram::program_ptr load_program(const EffectShaderProgram::uber_program_hash& hash, 
                                                  ProgramContructor                             constructor)
    {
        program_cache::iterator programIter = programCache.find(hash);
        if ( programIter != programCache.end() ) {
            return programIter->second;
        }
        
        EffectShaderProgram::program_ptr program = constructor();
        programCache.insert( program_cache::value_type(hash, program) );

        return program;
    }

} // anonymous namesapce

EffectShaderProgram::EffectShaderProgram(log::Logger& _effectLogger) :
    effectLogger(_effectLogger),
    dirty(true)
{
}

void EffectShaderProgram::addDefinition(const std::string& definition)
{
    for (size_t i = 0; i<definitions.size(); ++i)
    {
        if (definitions[i] == definition) {
            return;
        }
    }

    definitions.push_back(definition);
    dirty = true;
}

bool EffectShaderProgram::removeDefinition(const std::string& definition)
{
    for (size_t i = 0; i<definitions.size(); ++i)
    {
        if (definitions[i] == definition)
        {
            std::swap( definitions[i], definitions.back() );
            definitions.pop_back();
            return dirty = true;
        }
    }

    return false;
}

void EffectShaderProgram::removeDefinitions()
{
    definitions.clear();
    dirty = true;
}

void EffectShaderProgram::addShader(const std::string& shader)
{
    for (size_t i = 0; i<shaders.size(); ++i)
    {
        if (shaders[i] == shader) {
            return;
        }
    }

    shaders.push_back(shader);
    dirty = true;
}

bool EffectShaderProgram::removeShader(const std::string& shader)
{
    for (size_t i = 0; i<shaders.size(); ++i)
    {
        if (shaders[i] == shader)
        {
            std::swap( shaders[i], shaders.back() );
            shaders.pop_back();
            return dirty = true;
        }
    }

    return false;
}

void EffectShaderProgram::removeShaders()
{
    shaders.clear();
    dirty = true;
}

void EffectShaderProgram::constructProgram()
{
    // make hash
    if (dirty)
    {
        std::sort( definitions.begin(), definitions.end() );
        std::sort( shaders.begin(), shaders.end() );

        uber_program_hash hash( definitions.begin(),
                                definitions.end(),
                                shaders.begin(),
                                shaders.end() );

        make_uber_program programConstructor(effectLogger, hash);
        program    = load_program(hash, programConstructor);
        bindings   = programConstructor.bindings;

        redirectUniforms();
        dirty = false;
    }
}
/** Get shader program. */
const sgl::Program* EffectShaderProgram::getProgram() const 
{ 
    if (dirty) {
        const_cast<EffectShaderProgram*>(this)->constructProgram();
    }

    return program.get();
}
