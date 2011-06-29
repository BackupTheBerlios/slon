#ifndef SLON_ENGINE_DATABASE_COLLADA_H
#define SLON_ENGINE_DATABASE_COLLADA_H

#include "../Storage.h"
#include "../Loader.h"
#include "ColladaCore.h"
#include "ColladaFX.h"
#include "ColladaPhysics.h"

namespace slon {
namespace database {

// forward
class ColladaDocument;

/** Representc collada <scene> element */
class collada_scene
{
public:
	typedef std::vector<collada_instance_visual_scene>	visual_scene_vector;
	typedef std::vector<collada_instance_physics_scene>	physics_scene_vector;

	XMLPP_ELEMENT_SERIALIZATION(collada_scene, ColladaDocument)

public:
	visual_scene_vector		visualScenes;
	physics_scene_vector	physicsScenes;
};

/** Collada document helps to load files from the collada files */
class ColladaDocument :
    public xmlpp::document
{
private:
    // noncopyable
	ColladaDocument(const ColladaDocument& rhs);
	ColladaDocument& operator = (const ColladaDocument&);

public:
	typedef xmlpp::document base_type;

public:
	ColladaDocument() :
	    skipErrors(true)
	{}

	// overload xmlpp::document
	void set_file_source(const std::string& fileName);

	/** Setup error skipping flag. Bad nodes will be ignored if this flag is true. */
	void setSkipErrorFlag(bool _skipErrors) { skipErrors = _skipErrors; }

	/** Get error skipping flag.
	 * @see setSkipErrorFlag.
	 */
	bool getSkipErrorFlag() const { return skipErrors; }

	/** After document has been loaded find library elements. */
    void on_load();
	
	/** Save libraries. */
	void on_save();

	template<typename ElementType>
    inline collada_library<ElementType>& get_library();

public:
	// flags
	bool   skipErrors;

	// library elements
	mutable collada_library_images				libraryImages;
	mutable collada_library_effects				libraryEffects;
	mutable collada_library_materials			libraryMaterials;
	mutable collada_library_animations			libraryAnimations;
	mutable collada_library_geometries			libraryGeometries;
	mutable collada_library_visual_scenes		libraryVisualScenes;
	mutable collada_library_controllers			libraryControllers;
	mutable collada_library_physics_scenes		libraryPhysicsScenes;
	mutable collada_library_physics_models		libraryPhysicsModels;
	mutable collada_library_physics_materials	libraryPhysicsMaterials;

	// main scene element
	mutable collada_scene						scene;
};


template<> inline collada_library<collada_image>&				ColladaDocument::get_library() { return libraryImages; }
template<> inline collada_library<collada_material>&			ColladaDocument::get_library() { return libraryMaterials; }
template<> inline collada_library<collada_effect>&				ColladaDocument::get_library() { return libraryEffects; }
template<> inline collada_library<collada_geometry>&			ColladaDocument::get_library() { return libraryGeometries; }
template<> inline collada_library<collada_visual_scene>&		ColladaDocument::get_library() { return libraryVisualScenes; }
template<> inline collada_library<collada_controller>&			ColladaDocument::get_library() { return libraryControllers; }
template<> inline collada_library<collada_physics_scene>&		ColladaDocument::get_library() { return libraryPhysicsScenes; }
template<> inline collada_library<collada_physics_model>&		ColladaDocument::get_library() { return libraryPhysicsModels; }
template<> inline collada_library<collada_physics_material>&	ColladaDocument::get_library() { return libraryPhysicsMaterials; }

// declare default instance loader function
template<typename ElementType>
void collada_instance<ElementType>::serialize(ColladaDocument&  document, 
                                              xmlpp::element&   elem,
                                              xmlpp::s_state    state)
{
    collada_serializer serializer;
    serializer &= xmlpp::make_nvp( "target",    xmlpp::as_attribute(target) );
    serializer &= xmlpp::make_nvp( "url",       xmlpp::as_attribute(target) );
	serializer.load(document, elem);

	// get target
    if ( state == xmlpp::LOAD ) {
		base_type::element = document.get_library<ElementType>().get_element( target.substr(1) );
    }
}

/** Loader of COLLADA files for DatabaseManager */
class ColladaLoader :
    public LibraryLoader
{
public:
    library_ptr load(filesystem::File* file);
};

} // namespace database
} // namespace slon

#endif // SLON_ENGINE_DATABASE_COLLADA_H
