#ifndef META_ASM_HPP
#define META_ASM_HPP

#include <cstddef>
#include <type_traits>

/**
* @brief A dependent false type so that the compilation will fail 
* only if the template with static_assert(false) is instatiated, not before.
* Value is always false.
* 
* 
* @tparam T Specific value does not matter only has to be dependent type or value.
* Value is always false. Parameter only there so that the value is dependent.
*/
template<typename T>
struct type_dependent_false_type : std::false_type 
{
};

template<size_t T>
struct num_dependent_false_type : std::false_type
{
};

/**
* @brief A dependent false value so that the compilation will fail
* only if the template with static_assert(false) is instatiated, not before.
* Used for error messages when the template specialization instatiated 
* indicates an error in the program.
* 
* @tparam T Specific value does not matter only has to be dependent type or value. 
* Value is always false. Parameter only there so that the value is dependent.
*/
template<typename T>
constexpr bool type_dependent_false = type_dependent_false_type<T>::value;

template<size_t N>
constexpr bool num_dependent_false = num_dependent_false_type<N>::value;

/**
* @brief Create a static type list filled with Elements
* 
* @tparam Elements types to populate the list with
*/
template<class ...Elements>
struct static_list 
{
};

/**
* @brief Add a new element to a static type list and store the new list under the alias type
* 
* @tparam NewElement The new element to add to the list
* @tparam OldList The list to add the new element to
*/
template<class NewElement, class OldList>
struct static_list_add_impl;

template<class NewElement, class ...Elements>
struct static_list_add_impl<NewElement, static_list<Elements...>> 
{
    using type = static_list<NewElement, Elements...>;
};

/**
* @brief Add a new element to a static type list and return the new static type list
*
* @tparam NewElement The new element to add to the list
* @tparam OldList The list to add the new element to
*/
template<class NewElement, class OldList>
using static_list_add_t = typename static_list_add_impl<NewElement, OldList>::type;

/**
* @brief Get the value of an element in the static type list which has the matching static member id.
* If the element with the given id is not found then the compilation process fails.
* Stores the value of the searched for element in the static member field value
* 
* @tparam ElementId The Id of the element to get the value from
* @tparam List The list to search through
*/
template<size_t ElementId, class List>
struct static_list_get_by_id_impl;

template<size_t ElementId, class FirstElement, class ...TailElements>
struct static_list_get_by_id_impl<ElementId, static_list<FirstElement, TailElements...>>
{
private:
    static constexpr int get_value() {
        if constexpr (FirstElement::id == ElementId) {
            return FirstElement::value;
        }
        else {
            return static_list_get_by_id_impl<ElementId, static_list<TailElements...>>::value;
        }
    }

public:
    static constexpr int value = get_value();
};

template<size_t ElementId, class FirstElement>
struct static_list_get_by_id_impl<ElementId, static_list<FirstElement>>
{
    static_assert(ElementId == FirstElement::id, "No element with given ElementId found (Variable with given ID does not exist)");
    static constexpr int value = FirstElement::value;
};

template<size_t ElementId>
struct static_list_get_by_id_impl<ElementId, static_list<>>
{
    static_assert(num_dependent_false<ElementId>, "No elements in the list to get (No variables specified)");
};


/**
* @brief Returns the value of an element in the static type list which has the matching static member id.
* If the element with the given id is not found then the compilation process fails.
*
* @tparam ElementId The Id of the element to get the value from
* @tparam List The list to search through
*/
template<size_t ElementId, class List>
constexpr int static_list_get_by_id_v = static_list_get_by_id_impl<ElementId, List>::value;

/**
* @brief Get whether an element with the given Id is already present in the list
* 
* @tparam ElementId The Id of the element to check for
* @tparam List The list to search through
*/
template<size_t ElementId, class List>
struct static_list_id_present_impl;

template<size_t ElementId, class FirstElement, class ...Elements>
struct static_list_id_present_impl<ElementId, static_list<FirstElement, Elements...>>
    : std::conditional_t<FirstElement::id == ElementId,
        std::true_type,
        static_list_id_present_impl<ElementId, static_list<Elements...>>>
{
};

template<size_t ElementId>
struct static_list_id_present_impl<ElementId, static_list<>>
    : std::false_type
{
};

template<size_t ElementId, class List>
constexpr bool static_list_id_present_v = static_list_id_present_impl<ElementId, List>::value;

/**
* @brief Get the list from the Idx on. Get a static list with the elements up to the index Idx removed.
* Stores the resulting list under the member alias type.
*
* @tparam Idx From which index to start the new list
* @tparam List The old list to shorten
*/
template<size_t Idx, class List>
struct static_list_get_rest_impl;

template<size_t Idx, class FirstElement, class ...Elements>
struct static_list_get_rest_impl<Idx, static_list<FirstElement, Elements ...>>
    : static_list_get_rest_impl<Idx - 1, static_list<Elements ...>> {
};

template<class FirstElement, class ...Elements>
struct static_list_get_rest_impl<1, static_list<FirstElement, Elements ...>> {
    using type = static_list<FirstElement, Elements...>;
};

template<size_t Idx>
struct static_list_get_rest_impl<Idx, static_list<>>
{
    static_assert(num_dependent_false<Idx>, "Index outside of the list bounds (Instruction to jump to is outside the bounds of the program)");
};


/**
* @brief Return the given list with the elements up to the index Idx removed.
* 
* @tparam Idx From which index to start the new list
* @tparam List The old list to shorten
*/
template<size_t Idx, class List>
using static_list_get_rest_t = typename static_list_get_rest_impl<Idx, List>::type;

/**
* @brief Removes the element with the given id from the list.
* Stores the resulting list under the member alias type.
*
* @tparam ElementId The Id of the element to remove
* @tparam List The list to remove the element from
*/
template<size_t ElementId, class List>
struct static_list_remove_id_impl;

template<size_t ElementId, class FirstElement, class ...Elements>
struct static_list_remove_id_impl<ElementId, static_list<FirstElement, Elements...>>
    : std::conditional_t<ElementId == FirstElement::id,
        static_list_remove_id_impl<ElementId, static_list<Elements...>>,
        static_list_add_impl<FirstElement, typename static_list_remove_id_impl<ElementId, static_list<Elements...>>::type>>
{
};

template<size_t ElementId, class FirstElement>
struct static_list_remove_id_impl<ElementId, static_list<FirstElement>>
{
    using type = std::conditional_t<ElementId == FirstElement::id,
        static_list<>, 
        static_list<FirstElement>>;
};

template<size_t ElementId>
struct static_list_remove_id_impl<ElementId, static_list<>>
{
    static_assert(num_dependent_false<ElementId>, "No elements in the list to remove (No variables specified)");
};

/**
* @brief Returns a static type list with the element with the given id removed.
*
* @tparam ElementId The Id of the element to remove
* @tparam List The list to remove the element from
*/
template <size_t ElementId, class List>
using static_list_remove_id_t = typename static_list_remove_id_impl<ElementId, List>::type;

/**
 * @brief Initialize a variable with identifier `VariableId` with a given initial value
 *
 * @tparam VariableId A unique identifier
 * @tparam InitValue Initial value of the variable
 */
template<size_t VariableId, int InitValue>
struct variable
{
    static constexpr size_t id = VariableId;
    static constexpr int value = InitValue;
};


/**
* @brief Set the value of the element specified with the ElementId in the list to the specified value
* We first remove the element with given ElementId and then add a new element with the same ElementId and NewValue.
* Stores the resulting list under the alias type.
* 
* @tparam ElementId The Id of the element to set to the new value
* @tparam NewValue The new value to set the element to
* @tparam List The list in which the element to change resides
*/
template<size_t ElementId, int NewValue, class List>
struct variable_list_set_value_impl
{
    using type = static_list_add_t<variable<ElementId, NewValue>, static_list_remove_id_t<ElementId, List>>;
};

/**
* @brief Returns a list with the value of the element specified with the ElementId set to the specified value
*
* @tparam ElementId The Id of the element to set to the new value
* @tparam NewValue The new value to set the element to
* @tparam List The list in which the element to change resides
*/
template<size_t ElementId, int NewValue, class List>
using variable_list_set_value_t = typename variable_list_set_value_impl<ElementId, NewValue, List>::type;

/**
 * @brief Store a value of source variable into a destination variable
 * 
 * @tparam DestinationVariableId An identifier of destination variable
 * @tparam SourceVariableId An identifier of source variable
 */
template<size_t DestinationVariableId, size_t SourceVariableId>
struct store 
{
    static constexpr size_t dest = DestinationVariableId;
    static constexpr size_t source = SourceVariableId;
};

/**
 * @brief Add a value of source variable into a value in the destination variable
 * 
 * @tparam DestinationVariableId An identifier of destination variable
 * @tparam SourceVariableId An identifier of source variable
 */
template<size_t DestinationVariableId, size_t SourceVariableId>
struct add {
    static constexpr size_t dest = DestinationVariableId;
    static constexpr size_t source = SourceVariableId;
};

/**
 * @brief Jump the given command if the value in variable is not zero, otherwise continue with next instruction
 * 
 * @tparam VariableId An identifier of variable to check
 * @tparam CommandNumber Number of the command where we jump to
 */
template<size_t VariableId, size_t CommandNumber>
struct jump_if_not_zero 
{
    static constexpr size_t id = VariableId;
    static constexpr size_t line_num = CommandNumber;
};

/**
 * @brief Return value of a given variable and exit the program.
 * 
 * @tparam VariableId The identifier of the variable that returns the value.
 */
template<size_t VariableId>
struct ret
{
    static constexpr size_t id = VariableId;
};

/**
* @brief Execute the instructions in the RemainingCommandsList until we hit a return statement.
* Stores the result of the execution in the static member value
* 
* @tparam VariableList List of all declared variables
* @tparam CommandList List of all commands used in the jump instruction
* @tparam RemainingCommandsList The List of instructions we are to execute if the program was linear (i.e. no flow control)
*/
template<class VariableList, class CommandList, class RemainingCommandsList>
struct execute_impl;

template<class VariableList, class CommandList>
struct execute_impl<VariableList, CommandList, static_list<>>
{
    static_assert(type_dependent_false<VariableList>, "Program ended without a return statement");
};

/** 
* @brief Specialization for adding a variable.
* We create a new variable list with the new variable added and pass the new variable list onward
*/
template<class VariableList, class CommandList, size_t VariableId, int InitValue, class ...TailCommands>
struct execute_impl<VariableList, CommandList, static_list<variable<VariableId, InitValue>, TailCommands...>>
{
private:
    static_assert(!static_list_id_present_v<VariableId, VariableList>, "Variable with the same ID already present");
    using new_variable_list = static_list_add_t<variable<VariableId, InitValue>, VariableList>;

public:
    static constexpr int value = execute_impl<new_variable_list, CommandList, static_list<TailCommands...>>::value;
};

/**
* @brief Specialization for returning a variable.
* We store the value of the specified variable in the static member value
*/
template<class VariableList, class CommandList, size_t VariableId, class ...TailCommands>
struct execute_impl<VariableList, CommandList, static_list<ret<VariableId>, TailCommands...>>
{
    static constexpr int value = static_list_get_by_id_v<VariableId, VariableList>;
};

/**
* @brief Specialization for storing the value of one variable into another variable .
* Retrieve the value of the source variable and set the value of the destination variable to the new value
*/
template<class VariableList, class CommandList, size_t DestinationVariableId, size_t SourceVariableId, class ...TailCommands>
struct execute_impl<VariableList, CommandList, static_list<store<DestinationVariableId, SourceVariableId>, TailCommands...>>
{
private:
    static constexpr int source_value = static_list_get_by_id_v<SourceVariableId, VariableList>;
    using new_variable_list = variable_list_set_value_t<DestinationVariableId, source_value, VariableList>;

public:
    static constexpr int value = execute_impl<new_variable_list, CommandList, static_list<TailCommands...>>::value;
};

/**
* @brief Specialization for adding the value of one variable to the value of another variable.
* Retrieve the value of the source and destination variables 
* and set the value of destination variable to the sum of these values
*/
template<class VariableList, class CommandList, size_t DestinationVariableId, size_t SourceVariableId, class ...TailCommands>
struct execute_impl<VariableList, CommandList, static_list<add<DestinationVariableId, SourceVariableId>, TailCommands...>>
{
private:
    static constexpr int source_value = static_list_get_by_id_v<SourceVariableId, VariableList>;
    static constexpr int dest_value = static_list_get_by_id_v<DestinationVariableId, VariableList>;
    using new_variable_list = variable_list_set_value_t<DestinationVariableId, dest_value + source_value, VariableList>;

public:
    static constexpr int value = execute_impl<new_variable_list, CommandList, static_list<TailCommands...>>::value;
};

/**
* @brief Specialization for conditional jump.
* Retrieve the value of the specified variable and depending on whether the value of the variable is zero
* we set the commands to be executed to a new list that begins at the specified offset from the start of the program.
*/
template<class VariableList, class CommandList, size_t VariableId, size_t CommandNumber, class ...TailCommands>
struct execute_impl<VariableList, CommandList, static_list<jump_if_not_zero<VariableId, CommandNumber>, TailCommands...>>
{
private:
    // Moved the computation into a constexpr function so that we can use if constexpr
    static constexpr int get_execution_result() {
        constexpr int variable_value = static_list_get_by_id_v<VariableId, VariableList>;

        if constexpr (variable_value != 0) {
            using new_remaining_command_list = static_list_get_rest_t<CommandNumber, CommandList>;
            return execute_impl<VariableList, CommandList, new_remaining_command_list>::value;
        }
        else {
            return execute_impl<VariableList, CommandList, static_list<TailCommands...>>::value;
        }
    }

public:
    static constexpr int value = get_execution_result();
};

/**
 * @brief A template variable containing a return value after execution of the program.
 * 
 * @tparam Commands Instructions that should be executed.
 */
template<class ...Commands>
constexpr int execute_v = execute_impl<static_list<>, static_list<Commands...>, static_list<Commands...>>::value;

#endif // META_ASM_HPP
