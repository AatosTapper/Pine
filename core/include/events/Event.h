#pragma once

#include "pch.h"
#include <typeindex>

enum class EventType {
    None = 0,
    WindowResize, WindowClose,
    KeyPressed, KeyReleased, KeyRepeat,
    MouseButtonPressed, MouseButtonReleased, MouseMoved,
    CustomLua
};

enum EventCategory : uint32_t {
    NONE        = 0,
    WINDOW      = 1 << 1,
    KEYBOARD    = 1 << 2,
    MOUSE       = 1 << 3,
    CUSTOM      = 1 << 4
};

#define EVENT_CLASS_TYPE(type) static constexpr EventType get_static_type() { return EventType::type; }; \
                                virtual constexpr EventType get_event_type() const override { return get_static_type(); }; \
                                virtual constexpr const char* get_name() const override { return #type; }; \

#define EVENT_CLASS_CATEGORY(category) virtual constexpr uint32_t get_category_flags() const override { return category; }

#define CREATE_TYPE_INDEX(T) virtual std::type_index get_typeid() const override { return typeid(T); }

class Event {
public:
    virtual ~Event() {}

    virtual constexpr EventType get_event_type() const = 0;
    virtual constexpr const char* get_name() const = 0;
    virtual constexpr uint32_t get_category_flags() const = 0;

    inline constexpr bool is_in_category(EventCategory category) {
        return get_category_flags() & category;
    }

    virtual std::type_index get_typeid() const = 0;
};

enum class HandlerPersistence {
    None = 0,
    Single,
    Continuous
};

template<class T>
using EventCallback = std::function<HandlerPersistence (T*)>;

class FunctionHandlerBase {
public:
    virtual ~FunctionHandlerBase() {}

    constexpr HandlerPersistence execute(Event *event) {
        return m_call(event);
    }

protected:
    virtual constexpr HandlerPersistence m_call(Event *event) = 0;
};

template<class T>
class FunctionHandler : public FunctionHandlerBase {
public:
    FunctionHandler(EventCallback<T> member_function) noexcept : m_member_function(member_function) {}

    virtual constexpr HandlerPersistence m_call(Event *event) override {
        return m_member_function(static_cast<T*>(event));
    }

private:
    EventCallback<T> m_member_function;
};

using HandlerVec = std::vector<std::unique_ptr<FunctionHandlerBase>>;
class EventBus {
public:
    void publish(Event *event) {
        assert(event);
        auto handlers = m_subscribers[event->get_typeid()].get();
        if (handlers == nullptr) return;
    
        for (auto &handler : *handlers) {
            if (handler != nullptr) {
                HandlerPersistence remove = handler->execute(event);
                if (remove == HandlerPersistence::Single) {
                    handler = nullptr;
                }
            }
        }
        handlers->erase(
            std::remove_if(
                handlers->begin(),
                handlers->end(),
                [](const auto &handler) { return handler == nullptr; }
            ), 
            handlers->end()
        );
    }

    template<typename T>
    void subscribe(EventCallback<T> callback) {
        auto handlers = m_subscribers[typeid(T)].get();

        if (handlers == nullptr) [[unlikely]] {
            m_subscribers[typeid(T)] = std::make_unique<HandlerVec>();
            handlers = m_subscribers[typeid(T)].get();
        }
        handlers->push_back(std::make_unique<FunctionHandler<T>>(callback));
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<HandlerVec>> m_subscribers;
};