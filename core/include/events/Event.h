#pragma once

#include "pch.h"
#include <typeindex>

enum class EventType {
    None = 0,
    WindowResize, WindowClose,
    KeyPressed, KeyReleased, KeyRepeat,
    MouseButtonPressed, MouseButtonReleased, MouseMoved
};

enum EventCategory : uint32_t {
    NONE        = 0,
    WINDOW      = 1 << 1,
    KEYBOARD    = 1 << 2,
    MOUSE       = 1 << 3
};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::type; }; \
                                virtual EventType get_event_type() const override { return get_static_type(); }; \
                                virtual const char* get_name() const override { return #type; }; \

#define EVENT_CLASS_CATEGORY(category) virtual uint32_t get_category_flags() const override { return category; }

#define CREATE_TYPE_INDEX(T) virtual std::type_index get_typeid() const override { return typeid(T); }

class Event {
    friend class EventBus;
public:
    virtual ~Event() {}

    virtual EventType get_event_type() const = 0;
    virtual const char* get_name() const = 0;
    virtual uint32_t get_category_flags() const = 0;

    inline bool is_in_category(EventCategory category) {
        return get_category_flags() & category;
    }

    virtual std::type_index get_typeid() const = 0;

private:
    bool m_handled = false;
};

template<class T>
using EventCallback = std::function<void (T*)>;

class FunctionHandlerBase {
public:
    virtual ~FunctionHandlerBase() {}

    void execute(Event *event) {
        m_call(event);
    }

protected:
    virtual void m_call(Event *event) = 0;
};

template<class T>
class FunctionHandler : public FunctionHandlerBase {
public:
    FunctionHandler(EventCallback<T> member_function) : m_member_function(member_function) {}

    virtual void m_call(Event *event) override {
        m_member_function(static_cast<T*>(event));
    }

private:
    EventCallback<T> m_member_function;
};

using HandlerVec = std::vector<std::unique_ptr<FunctionHandlerBase>>;
class EventBus {
public:
    void publish(Event *event) {
        auto handlers = m_subscribers[event->get_typeid()];
        if (handlers == nullptr) return;
    
        for (auto &handler : *handlers) {
            if (handler != nullptr) {
                handler->execute(event);
            }
        }
    }

    template<typename T>
    void subscribe(EventCallback<T> callback) {
        auto handlers = m_subscribers[typeid(T)];

        if (handlers == nullptr || handlers->size() == 0) {
            handlers = std::make_shared<HandlerVec>();
            m_subscribers[typeid(T)] = handlers;
        }
        handlers->push_back(std::make_unique<FunctionHandler<T>>(callback));
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<HandlerVec>> m_subscribers;
};