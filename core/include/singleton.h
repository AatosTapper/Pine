#pragma once

#define IMPL_SINGLETON_DISPATCHER(CLASS)    \
    static CLASS &instance()                \
    {                                       \
        static CLASS instance;              \
        return instance;                    \
    }                                   

#define IMPL_VIRTUAL_SINGLETON_DISPATCHER(DERIVED, BASE)    \
    IMPL_SINGLETON_DISPATCHER(DERIVED)                      \
    static BASE &get_base_instance()                        \
    {                                                       \
        return dynamic_cast<BASE&>(instance());             \
    }

#define IMPL_NO_COPY(CLASS)                 \
    CLASS(CLASS const&) = delete;           \
    void operator=(CLASS const&) = delete;