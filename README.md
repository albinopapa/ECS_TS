# ECS_TS
C++17 ECS ( Entity, Component, System ) design using std::variant

Testing out C++17 std::variant using ECS design pattern.  Currently in it's beginning stages, very template heavy.  

I'm always looking for ways to reduce the amount of code needed to write, but for now there is a lot to take in.

List of main library files
"ECS_Algorithms.h" - shortcuts for std::find_if and remove/erase pattern ( erase_if )
"ECS_Component.h"  - Template component base class ( empty ), just used as a common interface
"ECS_Entity.h"     - Entity class for adding,removing,finding and verifying components
"ECS_Factory.h"    - Factory for creating each of the types used in the ECS library
"ECS_Message.h"    - Template message base class, and template message_filter base class, just used as a common interface
"ECS_Mailbox.h"    - Template base class for send/receive interface
"ECS_System.h"     - Template class used to add/remove/store list of entity resources
"ECS_Receiver.h"   - Template class used for accepting and storing messages
"ECS_Sender.h"     - Template class used for sending messages and storing receivers
"ECS_Utilities.h"  - Template utilities, most importantly shared_pool, resource and ref_counter
"ECS_World.h"      - Template class used to store and execute system logic.

There are some requirements when instantiating parts of the library.  
The ECS_Entity for instance requires a tag just to differentiate the entity types, a variant of component types
and a variant of message types.  The variant of message types is forwarded to the ECS_Mailbox base class ( not implented yet for entities ).
Using aliases or some other method will be very necessary, or else you have to type out the variant and all it's parameters in each 
place used.

The tags are there to allow for forward declaration of types.

Example:

```cpp
struct player_tag{};
struct wall_tag{};

struct position_tag{};
struct velocity_tag{};

struct component_added_tag{};
struct component_removed_tag{};

using component_t = std::variant<screws::ECS_Component<position_tag>, screws::ECS_Component<velocity_tag>>;
using message_t = std::variant<screws::ECS_Message<component_added_tag>,screws::ECS_Message<component_removed_tag>>;
using entity_t = std::variant<screws::ECS_Entity<player_tag, component_t, message_t>, screws::ECS_Entity<wall_tag, component_t, message_t>>;
```

Other parts are created in the same manner, just look at Includes.h to get ideas.

One of the most important patterns of using std::variant is the visitor pattern.  
Seeing as how this is a library for general use, this pattern is what is necessary.  You provide the implementation of
world and system logic through providing "visitors".

Example of a world visitor
```cpp
class WorldDispatcher
{
public:
	WorldDispatcher( float _dt, Graphics& _graphics, Keyboard& _keyboard );
	void operator()( Movable& _system );
	void operator()( Drawable& _system )const;
	void operator()( Collidable& _system );
	void operator()( Controllable& _system );
private:
	float dt;
	Graphics& gfx;
	Keyboard& kbd;
};

// How it's defined
WorldDispatcher::WorldDispatcher( float _dt, Graphics & _graphics, Keyboard& _keyboard )
	:
	dt( _dt ),
	gfx( _graphics ),
	kbd( _keyboard )
{
}

void WorldDispatcher::operator()( Movable& _system )
{
	_system.execute( MovableMessageDispatcher( _system ), MovableExecuteDispatcher( dt ) );
}

void WorldDispatcher::operator()( Drawable& _system ) const
{
	_system.execute( DrawableMessageDispatcher( _system ), DrawableExecuteDispatcher( gfx ) );
}

void WorldDispatcher::operator()( Collidable& _system )
{
	_system.execute( CollidableMessageDispatch( _system ), CollidableExecuteDispatch( _system ) );
}

void WorldDispatcher::operator()( Controllable& _system )
{
	_system.execute( ControllableMessageDispatch( _system ), ControllableExecuteDispatch( _system, kbd ) );
}


// How it's called/used
// Only world instance, so it's safe to use std::get directly, if there were multiple world objects, you'd want to verify
// which version you were getting.
std::get<World>( *vworld ).tick( WorldDispatcher( .016f, gfx, wnd.kbd ) );

```
In this case, the visitor is a class with the call operator overloaded for each system the ECS_World object will hold.  
The members are passed to specific system visitors.  So for Movable, it needs the dt member, drawable needs the gfx member 
and Controllable needs the kbd member.

You may have noticed the *vworld in the std::get line.  First off, the v is just a prefix I used to signify it's a variant 
and not an actual world object.  Secondly, the * prefixing it is because it's not directly a variant of a world object either lol.
It is instead a ref counted resource, though not using shared_ptr.  
In ECS_Utilities.h, there are three classes of note; shared_pool, resource and ref_counter;

Shared_pool has three std::vectors, one that holds variants, one that holds the address of an element in the variant container,
and one for ref_counters.  The first container holds the actual variant, nothing more.  The second holds a resource object, pointing
to the address of the same index ( backer[i] points to std::addressof( container[i] ) ).  The third, holds a reference count
of each backed resource with the same index ( counters[i] holds ref count of backer[i] ).  Aside from the vector allocations, there 
are no other dynamic allocations.  This means that all the entities are contiguous, all systems are contiguous, all components are 
contiguous.  Since all the shared_pool objects are in ECS_Factory, you must use the factory to create each of the objects of 
the library. 

```cpp
// Alias for a screws::ECS_Factory
using Factory = screws::ECS_Factory<
	component_t,
	entity_t,
	message_t,
	system_t,
	world_t>;

// How to create a factory, you could of course use the Factory::instance() method directly also
inline Factory& g_factory = Factory::instance();

// create_world() returns a shared_resource<world_t> which is a variant of worlds, though only one exists in this example.
vworld = g_factory.create_world<World>()
```
The screws::ECS_System class takes in among other template parameters, a MessageDispatch visitor.  This allows you to handle 
messages sent to the system.

```cpp
void WorldDispatcher::operator()( Controllable& _system )
{
	_system.execute( ControllableMessageDispatch( _system ), ControllableExecuteDispatch( _system, kbd ) );
}
```
Here, the WorldDispatcher passes ControllableMessageDispatch and ControllableExecuteDispatch objects to the execute() function.  
Inside the screws::ECS_System::execute() function, it forwards the message dispatch object to the process_messages() function and 
visits the ExecuteDispatch object passing in each entity as a parameter.

This is still a work in progress, so interfaces may change.

