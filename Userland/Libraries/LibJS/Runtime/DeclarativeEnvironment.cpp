/*
 * Copyright (c) 2020-2022, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Interpreter.h>
#include <LibJS/Runtime/DeclarativeEnvironment.h>
#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Value.h>

namespace JS {

DeclarativeEnvironment::DeclarativeEnvironment()
    : Environment(nullptr)
{
}

DeclarativeEnvironment::DeclarativeEnvironment(Environment* parent_scope)
    : Environment(parent_scope)
{
}

DeclarativeEnvironment::~DeclarativeEnvironment()
{
}

void DeclarativeEnvironment::visit_edges(Visitor& visitor)
{
    Base::visit_edges(visitor);
    for (auto& binding : m_bindings)
        visitor.visit(binding.value);
}

// 9.1.1.1.1 HasBinding ( N ), https://tc39.es/ecma262/#sec-declarative-environment-records-hasbinding-n
ThrowCompletionOr<bool> DeclarativeEnvironment::has_binding(FlyString const& name, Optional<size_t>* out_index) const
{
    auto index = m_names.find_first_index(name);
    if (!index.has_value())
        return false;
    if (!is_permanently_screwed_by_eval() && out_index)
        *out_index = *index;
    return true;
}

// 9.1.1.1.2 CreateMutableBinding ( N, D ), https://tc39.es/ecma262/#sec-declarative-environment-records-createmutablebinding-n-d
ThrowCompletionOr<void> DeclarativeEnvironment::create_mutable_binding(GlobalObject&, FlyString const& name, bool can_be_deleted)
{
    // 2. Create a mutable binding in envRec for N and record that it is uninitialized. If D is true, record that the newly created binding may be deleted by a subsequent DeleteBinding call.
    m_bindings.append(Binding {
        .value = {},
        .strict = false,
        .mutable_ = true,
        .can_be_deleted = can_be_deleted,
        .initialized = false,
    });
    m_names.append(name);

    // 1. Assert: envRec does not already have a binding for N.
    // NOTE: We skip this to avoid O(n) traversal of m_names.

    // 3. Return NormalCompletion(empty).
    return {};
}

// 9.1.1.1.3 CreateImmutableBinding ( N, S ), https://tc39.es/ecma262/#sec-declarative-environment-records-createimmutablebinding-n-s
ThrowCompletionOr<void> DeclarativeEnvironment::create_immutable_binding(GlobalObject&, FlyString const& name, bool strict)
{
    // 2. Create an immutable binding in envRec for N and record that it is uninitialized. If S is true, record that the newly created binding is a strict binding.
    m_bindings.append(Binding {
        .value = {},
        .strict = strict,
        .mutable_ = false,
        .can_be_deleted = false,
        .initialized = false,
    });
    m_names.append(name);

    // 1. Assert: envRec does not already have a binding for N.
    // NOTE: We skip this to avoid O(n) traversal of m_names.

    // 3. Return NormalCompletion(empty).
    return {};
}

// 9.1.1.1.4 InitializeBinding ( N, V ), https://tc39.es/ecma262/#sec-declarative-environment-records-initializebinding-n-v
ThrowCompletionOr<void> DeclarativeEnvironment::initialize_binding(GlobalObject&, FlyString const& name, Value value)
{
    auto index = m_names.find_first_index(name);
    VERIFY(index.has_value());
    auto& binding = m_bindings[*index];

    // 1. Assert: envRec must have an uninitialized binding for N.
    VERIFY(binding.initialized == false);

    // 2. Set the bound value for N in envRec to V.
    binding.value = value;

    // 3. Record that the binding for N in envRec has been initialized.
    binding.initialized = true;

    // 4. Return NormalCompletion(empty).
    return {};
}

// 9.1.1.1.5 SetMutableBinding ( N, V, S ), https://tc39.es/ecma262/#sec-declarative-environment-records-setmutablebinding-n-v-s
ThrowCompletionOr<void> DeclarativeEnvironment::set_mutable_binding(GlobalObject& global_object, FlyString const& name, Value value, bool strict)
{
    // 1. If envRec does not have a binding for N, then
    auto index = m_names.find_first_index(name);
    if (!index.has_value()) {
        // a. If S is true, throw a ReferenceError exception.
        if (strict)
            return vm().throw_completion<ReferenceError>(global_object, ErrorType::UnknownIdentifier, name);

        // b. Perform envRec.CreateMutableBinding(N, true).
        MUST(create_mutable_binding(global_object, name, true));

        // c. Perform envRec.InitializeBinding(N, V).
        MUST(initialize_binding(global_object, name, value));

        // d. Return NormalCompletion(empty).
        return {};
    }

    // 2-5. (extracted into a non-standard function below)
    TRY(set_mutable_binding_direct(global_object, *index, value, strict));

    // 6. Return NormalCompletion(empty).
    return {};
}

ThrowCompletionOr<void> DeclarativeEnvironment::set_mutable_binding_direct(GlobalObject& global_object, size_t index, Value value, bool strict)
{
    auto& binding = m_bindings[index];
    if (binding.strict)
        strict = true;

    if (!binding.initialized)
        return vm().throw_completion<ReferenceError>(global_object, ErrorType::BindingNotInitialized, m_names[index]);

    if (binding.mutable_) {
        binding.value = value;
    } else {
        if (strict)
            return vm().throw_completion<TypeError>(global_object, ErrorType::InvalidAssignToConst);
    }

    return {};
}

// 9.1.1.1.6 GetBindingValue ( N, S ), https://tc39.es/ecma262/#sec-declarative-environment-records-getbindingvalue-n-s
ThrowCompletionOr<Value> DeclarativeEnvironment::get_binding_value(GlobalObject& global_object, FlyString const& name, bool strict)
{
    // 1. Assert: envRec has a binding for N.
    auto index = m_names.find_first_index(name);
    VERIFY(index.has_value());

    // 2-3. (extracted into a non-standard function below)
    return get_binding_value_direct(global_object, *index, strict);
}

ThrowCompletionOr<Value> DeclarativeEnvironment::get_binding_value_direct(GlobalObject& global_object, size_t index, bool)
{
    auto& binding = m_bindings[index];

    // 2. If the binding for N in envRec is an uninitialized binding, throw a ReferenceError exception.
    if (!binding.initialized)
        return vm().throw_completion<ReferenceError>(global_object, ErrorType::BindingNotInitialized, m_names[index]);

    // 3. Return the value currently bound to N in envRec.
    return binding.value;
}

// 9.1.1.1.7 DeleteBinding ( N ), https://tc39.es/ecma262/#sec-declarative-environment-records-deletebinding-n
ThrowCompletionOr<bool> DeclarativeEnvironment::delete_binding(GlobalObject&, FlyString const& name)
{
    // 1. Assert: envRec has a binding for the name that is the value of N.
    auto index = m_names.find_first_index(name);
    VERIFY(index.has_value());

    auto& binding = m_bindings[*index];

    // 2. If the binding for N in envRec cannot be deleted, return false.
    if (!binding.can_be_deleted)
        return false;

    // 3. Remove the binding for N from envRec.
    // NOTE: We keep the entries in m_bindings and m_names to avoid disturbing indices.
    binding = {};
    m_names[*index] = {};

    // 4. Return true.
    return true;
}

ThrowCompletionOr<void> DeclarativeEnvironment::initialize_or_set_mutable_binding(GlobalObject& global_object, FlyString const& name, Value value)
{
    auto index = m_names.find_first_index(name);
    VERIFY(index.has_value());
    auto& binding = m_bindings[*index];
    if (!binding.initialized)
        TRY(initialize_binding(global_object, name, value));
    else
        TRY(set_mutable_binding(global_object, name, value, false));
    return {};
}

void DeclarativeEnvironment::initialize_or_set_mutable_binding(Badge<ScopeNode>, GlobalObject& global_object, FlyString const& name, Value value)
{
    MUST(initialize_or_set_mutable_binding(global_object, name, value));
}

}
