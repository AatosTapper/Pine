# Pine 
*work in progress*
## a 2D Game engine for Lua

Pine is built with a design philosophy of being as **simple and elegant** as possible
while allowing the **maximum amount of flexibility, freedom and power** fully within Lua.

### __The goal is to provide the smallest possible API that can build any game with an arbitary amount of complexity.__

The engine core is built with C++23 following modern game engine architecture conventions, like data oriented design. The entity system is based on ENTT. Pine does not rely on C++23 specific features, but benefits from the extended constexpr support.