--- Pine Lua API definitions:

--- NAMING CONVENTIONS
--- These follow the naming convention:
--- pine_"name_here"
--- type members DO NOT have "pine_" in front of them

--- AVOIDING BUGS
--- When a function is called incorrectly or fails, 
--- the application terminates with an error message.
--- However, if the function is called with wrong arguments or name,
--- there might NOT be any message so BE CAREFUL!

--- PERFORMANCE
--- For maximum performance, avoid recalling engine functions
--- when you could store the result and use it again. For example:
--- NO:
---     ent:get_component_Transform().x = 10
---     ent:get_component_Transform().y = 5
--- YES:
---     local transform = ent:get_component_Transform()
---     transfom.x = 10
---     transfom.y = 5

--- IF A FUNCTION IS PARTICULARLY SLOW, IT IS MARKED AS SUCH






--- #application and lifetime

--- Starts the application gameloop
function pine_run() end

--- Gets the fixed update loop's delta time
---@return number
function pine_tick_dt() end

--- Gets the real frametime including non update frames
--- WARNING: DO NOT USE IN GAMEPLAY LOGIC!!!
---@return number
function pine_frame_time() end

--- Gets the index of the current frame from application start (can be used for rng seeding etc.)
---@return integer
function pine_frame_index() end






--- #input
--- input can be checked with events aswell, but use these when possible 

--- Checks for keyboard and mousebutton input
---@param key integer @ keycode as "Cone.Key._the key you want" example: Cone.Key._SPACE
---@return boolean
function pine_get_input(key) end

--- Gets current mouse cursor position
---@return vec2
function pine_get_mouse_pos() end






--- #scene

--- Scene
---@class pine_Scene
pine_Scene = {}

--- Creates a new scene
---@param name string @WARNING: must be unique
---@return pine_Scene
function pine_new_scene(name)
    assert(name ~= nil, "cannot create a scene without a name")
    return pine_Scene.new(name) -- wrapper for type specificity
end

--- Adds an entity to the scene
---@param name string
---@return pine_Entity
function pine_Scene:add_entity(name) end

--- Gets an entity from the scene with the name
--- WARNING: check the return value for nil
---@param name string
---@return pine_Entity|nil @if not found, returns nil
function pine_Scene:get_entity(name) end

--- Gets all entities in the scene
---@return table<integer, pine_Entity>
function pine_Scene:get_entities() end

--- Gets all other entities within a radius
--- Doesn't get the passed in entity
--- Uses a spatial hash so overhead is very low
---@param ent pine_Entity
---@param radius number
---@return table<integer, pine_Entity>
function pine_Scene:get_close_entities(ent, radius) end

--- Gets all entities in an area
---@param x number
---@param y number
---@param radius number
---@return table<integer, pine_Entity>
function pine_Scene:spatial_lookup(x, y, radius) end

--- Gets the camera of the scene
---@return pine_Camera
function pine_Scene:get_camera() end

--- Gives access to component's parent entity in script file
--- Script file = lua script attached to some component like Script or CustomBehavior
--- Calling this outside a script is undefined and probably crashes
---@return pine_Entity
function pine_get_script_parent_entity() end

--- Get a copy of the entity and it's state for passing into another scene
--- WARNING: Return type can't be used for anything else other than passing into the
--- corresponding inport function
---@param entity pine_Entity
---@return userdata std::unique_ptr<SceneNode>
function pine_Scene:export_entity(entity) end

--- Transfer an exported entity into this scene
---@param node userdata std::unique_ptr<SceneNode>&
---@return pine_Entity
function pine_Scene:import_entity(node) end






--- #scene managing
--- loading and getting scenes etc

--- Gets the current scene
---@return pine_Scene
function pine_get_scene() end

--- Sets the current scene AND unloads the last one
--- WARNING: DO NOT CALL THIS ON THE CURRENT SET SCENE
---@param scene pine_Scene
---@return pine_Scene
function pine_set_scene(scene) end

--- Sets a temporary scene
--- Main usecase is when the old scene needs to stay in memory
---@param scene pine_Scene
---@return pine_Scene
function pine_set_temp_scene(scene) end

--- Removes the temporary scene
function pine_remove_temp_scene() end

--- Saves the currently active scene to the app/scenes directory
--- the scene filename will be "scene name.xml"
function pine_save_current_scene() end

--- Loads, selects and returns the scene from disc
--- If scene doesn't exist, doesn't do anything and returns nil
---@param name string
---@return pine_Scene|nil @optional, returns nil if scene doesn't exist
function pine_load_scene(name) end






--- #camera

--- Camera
---@class pine_Camera
pine_Camera = {}

--- Gets the camera position
---@return vec2
function pine_Camera:get_pos() end

--- Sets the camera position
---@param pos vec2
function pine_Camera:set_pos(pos) end

--- Moves the camera left
---@param amount number
function pine_Camera:left(amount) end

--- Moves the camera right
---@param amount number
function pine_Camera:right(amount) end

--- Moves the camera up
---@param amount number
function pine_Camera:up(amount) end

--- Moves the camera down
---@param amount number
function pine_Camera:down(amount) end






--- #entity

--- Entity
---@class pine_Entity
pine_Entity = {}

--- Removes the entity and it's component from a scene
--- Removal is deferred to the end of the frame
function pine_Entity:remove() end

--- Each component type has these corresponing functions: (arrow means return value)
--- entity:add_component_type(optional constructor arguments) -> created component
--- entity:get_component_type() -> component
--- entity:has_component_type() -> bool
--- entity:remove_component_type()

--- Exact component definitions come later




--- Gets the Tag component
--- A default component, it can't be added or removed
---@return pine_comp_Tag
function pine_Entity:get_component_Tag() end




--- Gets the Transform component
--- A default component, it can't be added or removed
---@return pine_comp_Transform
function pine_Entity:get_component_Transform() end




--- Adds a Script component to entity
--- Replaces the old component if exists
---@param path string|nil @optional
---@return pine_comp_Script
function pine_Entity:add_component_Script(path) end

--- Gets the Script component
---@return pine_comp_Script
function pine_Entity:get_component_Script() end

--- Tells if entity has a Script component
---@return boolean
function pine_Entity:has_component_Script() end

--- Removes the Script component
function pine_Entity:remove_component_Script() end




--- Adds a Table component to entity
--- Replaces the old component if exists
---@param tbl table|nil @optional
---@return pine_comp_Table
function pine_Entity:add_component_Table(tbl) end

--- Gets the Table component
---@return pine_comp_Table
function pine_Entity:get_component_Table() end

--- Tells if entity has a Table component
---@return boolean
function pine_Entity:has_component_Table() end

--- Removes the Table component
function pine_Entity:remove_component_Table() end




--- Adds a CustomBehavior component to entity
--- Replaces the old component if exists
---@param path string|nil @optional
---@return pine_comp_CustomBehavior
function pine_Entity:add_component_CustomBehavior(path) end

--- Gets the CustomBehavior component
---@return pine_comp_CustomBehavior
function pine_Entity:get_component_CustomBehavior() end

--- Tells if entity has a CustomBehavior component
---@return boolean
function pine_Entity:has_component_CustomBehavior() end

--- Removes the CustomBehavior component
function pine_Entity:remove_component_CustomBehavior() end




--- Adds a Sprite component to entity
--- Replaces the old component if exists
---@param path string|nil @optional
---@return pine_comp_Sprite
function pine_Entity:add_component_Sprite(path) end

--- Gets the Sprite component
---@return pine_comp_Sprite
function pine_Entity:get_component_Sprite() end

--- Tells if entity has a Sprite component
---@return boolean
function pine_Entity:has_component_Sprite() end

--- Removes the Sprite component
function pine_Entity:remove_component_Sprite() end




--- Adds a StateFlags component to entity
--- Replaces the old component if exists
---@param flags table<integer, string>|nil @optional
---@return pine_comp_StateFlags
function pine_Entity:add_component_StateFlags(flags) end

--- Gets the StateFlags component
---@return pine_comp_StateFlags
function pine_Entity:get_component_StateFlags() end

--- Tells if entity has a StateFlags component
---@return boolean
function pine_Entity:has_component_StateFlags() end

--- Removes the StateFlags component
function pine_Entity:remove_component_StateFlags() end




--- Adds a Collider component to entity
--- Replaces the old component if exists
---@return pine_comp_Collider
function pine_Entity:add_component_Collider() end

--- Gets the Collider component
---@return pine_comp_Collider
function pine_Entity:get_component_Collider() end

--- Tells if entity has a Collider component
---@return boolean
function pine_Entity:has_component_Collider() end

--- Removes the Collider component
function pine_Entity:remove_component_Collider() end




--- Sets the entity to be ignored during serialization, 
--- any data will be lost on application close including the component's existance
function pine_Entity:disable_serialization() end

--- Sets the entity back to being serialized
--- SERIALIZATION IS THE DEFAULT STATE so this function is for
--- allowing it again after calling disable_serialization
function pine_Entity:enable_serialization() end






--- #components

--- User definable handle for an entity, name doesn't need to be unique
--- DEFAULT: EVERY ENTITY HAS THIS ALREADY
--- Use this for debugging, entity identification, etc...
---@class pine_comp_Tag
---@field name string
pine_comp_Tag = {}




--- Entity transforms in world space
--- DEFAULT: EVERY ENTITY HAS THIS ALREADY
---@class pine_comp_Transform
--- position
---@field x number
---@field y number
--- scale
---@field sx number
---@field sy number
--- rotation in radians
---@field rr number
pine_comp_Transform = {}

--- Sets transfom x and y
---@param x number
---@param y number
function pine_comp_Transform:set_pos(x, y) end

--- Sets transfom width and height
---@param x number
---@param y number
function pine_comp_Transform:set_scale(x, y) end




--- Defines custom update and destroy scripts
--- Use this to define per-update game logic
---@class pine_comp_CustomBehavior
pine_comp_CustomBehavior = {}

--- Sets the script that gets called on every fixed update
---@param path string
function pine_comp_CustomBehavior:set_on_update(path) end

--- Sets the script that gets called when the component dies
--- (either by removal or scene unload)
---@param path string
function pine_comp_CustomBehavior:set_on_remove(path) end




--- Defines any number of general script files that can be run whenever
--- Use this to define more specialised one-time game logic (like entity spawning etc.)
--- (scripts can be run many times too but CustomBehavior is more for that)
---@class pine_comp_Script
pine_comp_Script = {}

--- Attaches a new script to the script component
--- The script can be ran with the returned id
--- (path is from the App/ directory)
---@param path string
---@return integer
function pine_comp_Script:push_script(path) end

--- Runs the script corresponding to passed id
---@param id integer|nil @optional, defaults to 0 (the first script)
function pine_comp_Script:run(id) end

--- Runs all scripts attached to the component
function pine_comp_Script:run_all() end




--- Store any data within a lua table
--- Use this for general entity data, like player health or level
---@class pine_comp_Table
---@field table table
pine_comp_Table = {}




--- General image component for drawing
--- Use this when if entity needs to be drawn
---@class pine_comp_Sprite
pine_comp_Sprite = {}

--- Sets the sprite texture
--- If already has a texture, the old one is removed
---@param path string
function pine_comp_Sprite:set_texture(path) end

--- Sets the "render layer" aka z offset of the sprite
--- This is useful for setting the back to front order of sprites
---@param val number @default = 0.0 (negative = further away)
function pine_comp_Sprite:set_render_layer(val) end




--- Define custom flag strings to identify and sort components
--- Use this in ECS like systems etc.
--- Example: FireSpreadSystem needs to know which entities are flammable.
---          You can set a flag "FLAMMABLE" for entities, and in the system do this:
---          if comp:has_flags({ "FLAMMABLE" }) then comp:set_flags({ "ON_FIRE" }) etc.... end
---@class pine_comp_StateFlags
pine_comp_StateFlags = {}

--- Sets the flags
--- If already has the flags, everything stays the same
---@param flags table<integer, string>
function pine_comp_StateFlags:set_flags(flags) end

--- Returns true if the entity has ALL of the flags specified
---@param flags table<integer, string>
---@return boolean
function pine_comp_StateFlags:has_flags(flags) end

--- Removes all flags that appear in the input list
---@param flags table<integer, string>
function pine_comp_StateFlags:remove_flags(flags) end




--- Detect entity collisions
--- Collider dimensions and position are inferred from entity's Transform
--- Supports box collider (AABB) and circle collider types (box by default)
---@class pine_comp_Collider
--- Prevents the entities from intersecting
---@field resolve_collisions boolean @default = false
pine_comp_Collider = {}

--- Container for per-collision info,
--- returned from some functions
---@class Collider_CollisionData
---@field ent pine_Entity @the entity collided with
---@field normal vec2 @collision normal from current entity's point of view
Collider_CollisionData = {}

--- Sets the collider type to Box (AABB)
function pine_comp_Collider:type_box() end

--- Sets the collider type to Circle
--- Circle radius is defined by the Transform width (sx)
function pine_comp_Collider:type_circle() end

--- Gets the entities in contact and the collision normals
--- from current entity's midpoint to the point of intersection
---@return table<integer, Collider_CollisionData>
function pine_comp_Collider:get_colliding_entities() end






--- #vector
--- these are translated glm::vec2 and glm::vec3, mostly for C++ - Lua communication ease

--- 2D Vector
---@class vec2
---@field x number
---@field y number
vec2 = {}

--- Creates a new vec2 instance
---@param x number
---@param y number
---@return vec2
function vec2.new(x, y) end

--- Sets the components of the vector
---@param x number
---@param y number
function vec2:set(x, y) end

--- Converts a vec3 to vec2
---@param other vec3
---@return vec2
function vec2.from(other) end

--- Returns the length of the vector
---@return number
function vec2:length() end

--- Returns the vector as normalized
--- Doesn't affect the original vector
---@return vec2
function vec2:normalize() end

--- Computes the dot product with another vector
---@param other vec2
---@return number
function vec2:dot(other) end




--- 3D Vector
---@class vec3
---@field x number
---@field y number
---@field z number
vec3 = {}

--- Creates a new vec3 instance
---@param x number
---@param y number
---@param z number
---@return vec3
function vec3.new(x, y, z) end

--- Sets the components of the vector
---@param x number
---@param y number
---@param z number
function vec3:set(x, y, z) end

--- Converts a vec2 to vec3
---@param other vec2
---@return vec3
function vec3.from(other) end

--- Returns the length of the vector
---@return number
function vec3:length() end

--- Returns the vector as normalized
--- Doesn't affect the original vector
---@return vec3
function vec3:normalize() end

--- Computes the dot product with another vector
---@param other vec3
---@return number
function vec3:dot(other) end

--- Computes the cross product with another vector
---@param other vec3
---@return vec3
function vec3:cross(other) end