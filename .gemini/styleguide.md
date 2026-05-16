# Epic C++ Coding Standard for Unreal Engine

## Class Organization

Classes should be organized with the reader in mind rather than the writer. Since most readers will use the public interface of the class, the public implementation should be declared first, followed by the class's private implementation.

## Naming Conventions

All code and comments should use **U.S. English** spelling and grammar.

The first letter of each word in a name (such as type name or variable name) is capitalized. There is usually no underscore between words. For example, `Health` and `UPrimitiveComponent` are correct, but `lastMouseCoordinates` or `delta_coordinates` are not. This is **PascalCase** formatting.

Type names are prefixed with an additional upper-case letter to distinguish them from variable names. For example, `FSkin` is a type name, and `Skin` is an instance of type `FSkin`.

### Type Prefixes

| Prefix | Usage                                                          |
|--------|----------------------------------------------------------------|
| `T`    | Template classes                                               |
| `U`    | Classes that inherit from `UObject`                            |
| `A`    | Classes that inherit from `AActor`                             |
| `S`    | Classes that inherit from `SWidget`                            |
| `I`    | Abstract interface classes                                     |
| `C`    | Epic's concept-alike struct types                              |
| `E`    | Enums                                                          |
| `b`    | Boolean variables (e.g., `bPendingDestruction`, `bHasFadedIn`) |
| `F`    | Most other classes (structs, etc.)                             |

### Typedefs

Typedefs should be prefixed by whatever is appropriate for that type:

- `F` for typedef of a struct
- `U` for typedef of a `UObject`

A typedef of a particular template instantiation is no longer a template and should be prefixed accordingly. Prefixes are omitted in C#.

Unreal Header Tool requires the correct prefixes in most cases, so it's important to provide them.

Type template parameters and nested type aliases based on those template parameters are **not** subject to the above prefix rules, as the type category is unknown. Prefer a `Type` suffix after a descriptive term. Disambiguate template parameters from aliases by using an `In` prefix.

### Naming Rules

- Type and variable names are **nouns**.
- Method names are **verbs** that either describe the method's effect, or the return value of a method without an effect.
- Macro names should be **fully capitalized** with words separated by underscores, and prefixed with `UE_`.
- Variable, method, and class names should be **clear**, **unambiguous**, and **descriptive**. The greater the scope of the name, the greater the importance of a good, descriptive name. Avoid over-abbreviation.
- All variables should be declared on their own line so that you can provide a comment on the meaning of each variable.
- All functions that return a bool should ask a true/false question, such as `IsVisible()` or `ShouldClearBuffer()`.
- A procedure (a function with no return value) should use a strong verb followed by an Object. Names to avoid include those beginning with "Handle" and "Process" because the verbs are ambiguous.
- Prefix function parameter names with `Out` if: the parameters are passed by reference **and** the function is expected to write to that value. This makes it obvious the value will be replaced by the function.
- If an `In` or `Out` parameter is also a boolean, put `b` before the `In`/`Out` prefix, such as `bOutResult`.
- Functions that return a value should describe the return value. The name should make clear what value the function returns. This is particularly important for boolean functions.

## Portable C++ Code

The `int` and `unsigned int` types vary in size across platforms. They are guaranteed to be at least 32 bits in width and are acceptable when integer width is unimportant. Explicitly-sized types are used in serialized or replicated formats.

### Preferred Types

| Type     | Description                                                              |
|----------|--------------------------------------------------------------------------|
| `bool`   | Boolean values (NEVER assume the size of bool). `BOOL` will not compile. |
| `TCHAR`  | A character (NEVER assume the size of TCHAR).                            |
| `uint8`  | Unsigned bytes (1 byte)                                                  |
| `int8`   | Signed bytes (1 byte)                                                    |
| `uint16` | Unsigned shorts (2 bytes)                                                |
| `int16`  | Signed shorts (2 bytes)                                                  |
| `uint32` | Unsigned ints (4 bytes)                                                  |
| `int32`  | Signed ints (4 bytes)                                                    |
| `uint64` | Unsigned quad words (8 bytes)                                            |
| `int64`  | Signed quad words (8 bytes)                                              |
| `float`  | Single precision floating point (4 bytes)                                |
| `double` | Double precision floating point (8 bytes)                                |
| `PTRINT` | An integer that may hold a pointer (NEVER assume the size of PTRINT)     |

## Use of Standard Libraries

Historically, UE avoided direct use of the C and C++ standard libraries to replace slow implementations with custom ones, add new functionality before it was widely available, make desirable non-standard behavioral changes, maintain consistent syntax, and avoid constructs incompatible with UE's idioms.

However, the standard library has matured. When there is a choice between a standard library feature and a UE one, prefer the option that gives superior results. Consistency is valued — if a legacy UE implementation is no longer serving a purpose, it may be deprecated and migrated toward the standard library.

Avoid mixing UE idioms and standard library idioms in the same API.

| Idiom                          | Recommendation                                                                                                   |
|--------------------------------|------------------------------------------------------------------------------------------------------------------|
| `<atomic>`                     | Use in new code; migrate old code when touched. `TAtomic` is only partially implemented.                         |
| `<type_traits>`                | Use where there's overlap with legacy UE traits. New traits should use lowercase `value`/`type` for composition. |
| `<initializer_list>`           | Must be used to support braced initializer syntax. No alternative.                                               |
| `<regex>`                      | May be used directly, but should be encapsulated within editor-only code.                                        |
| `<limits>`                     | `std::numeric_limits` can be used in its entirety.                                                               |
| `<cmath>`                      | All floating point functions may be used.                                                                        |
| `<cstring>: memcpy()/memset()` | May be used instead of `FMemory::Memcpy`/`FMemory::Memset` when there is a demonstrable performance benefit.     |

Standard containers and strings should be avoided except in interop code.

## Comments

Comments are communication and communication is vital.

### Guidelines

- **Write self-documenting code.** Use clear variable names so the code speaks for itself.
- **Write useful comments.** Comments should explain *why*, not *what*.
- **Do not over-comment bad code — rewrite it instead.**
- **Do not contradict the code.** A comment that says the opposite of what the code does is worse than no comment.

### Comment Formatting (Javadoc Style)

UE uses a JavaDoc-based system to extract comments and build documentation automatically.

- **Class comments** should include: a description of the problem this class solves, and why it was created.
- **Multi-line method comments** should include:
    - **Function purpose:** Documents the problem this function solves. Comments document *intent*; code documents *implementation*.
    - **Parameter comments:** Each parameter comment should include units of measure, the range of expected values, "impossible" values, and the meaning of status/error codes.
    - **Return comment:** Documents the expected return value. An explicit `@return` comment should not be used if the function's sole purpose is to return this value and it is already documented in the function purpose.
    - **Extra information:** `@warning`, `@note`, `@see`, and `@deprecated` can optionally be used. Each should be on its own line following the rest of the comments.
- Two parameter comment styles are supported: the `@param` multi-line style, and integrating parameter documentation into the function description for simple functions.
- Method comments should only be included **once**: where the method is publicly declared. They should only contain information relevant to callers.
- Details about the implementation and its overrides that are not relevant to callers should be commented within the method implementation.

## Const Correctness

Const is documentation as much as it is a compiler directive. All code should strive to be const-correct:

- Pass function arguments by `const` pointer or reference if not intended to be modified by the function.
- Flag methods as `const` if they do not modify the object.
- Use `const` iteration over containers if the loop isn't intended to modify the container.
- `const` is also preferred for by-value function parameters and locals. This tells the reader the variable will not be modified in the body of the function.
- One exception is pass-by-value parameters that are moved into a container (see "Move Semantics").
- Put the `const` keyword on the **end** when making a pointer itself const (rather than what it points to). References can't be "reassigned" and so can't be made `const` in the same way.
- **Never use `const` on a return type.** This inhibits move semantics for complex types and gives compile warnings for built-in types. This rule only applies to the return type itself, not the target type of a pointer or reference being returned.

## Modern C++ Language Syntax

Unreal Engine compiles with **C++20 by default** and requires a minimum version of C++20. Unless specified below, you should not use compiler-specific language features unless they are wrapped in preprocessor macros or conditionals and used sparingly.

### Static Assert

The `static_assert` keyword is valid for use where you need a compile-time assertion.

### Override and Final

The `override` and `final` keywords are valid for use, and their use is **strongly encouraged**.

### Nullptr

Use `nullptr` instead of the C-style `NULL` macro in all cases. One exception is in C++/CX builds (such as for Xbox One), where you should use the `TYPE_OF_NULLPTR` macro instead of `decltype(nullptr)`.

### Auto

You **should not use `auto`** in C++ code except for the following cases:

- When you need to bind a lambda to a variable, as lambda types are not expressible in code.
- For iterator variables, but only where the iterator's type is verbose and would impair readability.
- In template code, where the type of an expression cannot easily be discerned.

When using `auto` acceptably, always correctly use `const`, `&`, or `*` just as you would with the type name. It's important that types are clearly visible to someone reading the code.

### Range-Based For

Range-based for loops are **preferred** to keep code easier to understand and more maintainable. When migrating code that uses old `TMap` iterators, be aware that the old `Key()` and `Value()` functions are now `Key` and `Value` fields of the underlying `TPair`.

### Lambdas and Anonymous Functions

Lambdas can be used freely but come with additional safety concerns. The best lambdas should be no more than a couple of statements in length, particularly when used as a predicate in a generic algorithm.

Be aware that stateful lambdas can't be assigned to function pointers. Non-trivial lambdas should be documented the same way as regular functions. Lambdas can also be used as Delegates for deferred execution using functions like `BindWeakLambda`.

#### Captures and Return Types

**Explicit captures** should be used rather than automatic capture (`[&]` and `[=]`). This is important for readability, maintainability, safety, and performance reasons, particularly with large lambdas and deferred execution.

Explicit captures declare the author's intent, so mistakes are caught during code review. Incorrect captures can cause serious bugs and crashes. Additional considerations:

- By-reference capture and by-value capture of pointers (including `this`) can cause data corruption and crashes if lambda execution is deferred. Local and member variables should **never** be captured by reference for deferred lambdas.
- By-value capture can be a performance concern if it makes unnecessary copies for a non-deferred lambda.
- Accidentally captured `UObject` pointers are invisible to the garbage collector. Automatic capture catches this implicitly.
- Delegate wrappers like `CreateWeakLambda` and `CreateSPLambda` should be used for deferred execution — they will automatically unbind if the `UObject` or shared pointer are freed.
- Other shared objects can be captured as `TWeakObjectPtr` or `TWeakPtr` and then validated inside the lambda.
- Any deferred lambda use that does not follow these guidelines must have a comment explaining why the lambda capture is safe.

Explicit return types should be used for large lambdas or when returning the result of another function call.

### Strongly-Typed Enums

Enum classes (`enum class`) are the replacement for old-style namespaced enums, both for regular enums and `UENUMs`.

- Enums exposed to Blueprints must continue to be based on `uint8`.
- Enum classes used as flags can use the `ENUM_CLASS_FLAGS(EnumType)` macro to automatically define all bitwise operators.
- All enum flags should have an enumerator called `None` which is set to `0` for comparisons (required due to a language limitation in truth contexts).

### Move Semantics

All main container types — `TArray`, `TMap`, `TSet`, `FString` — have move constructors and move assignment operators. These are often used automatically when passing or returning these types by value. They can also be explicitly invoked using `MoveTemp`, UE's equivalent of `std::move`.

Returning containers or strings by value can be beneficial for expressivity without the usual cost of temporary copies.

### Default Member Initializers

Default member initializers can be used to define the defaults of a class inside the class itself. Benefits:

- No need to duplicate initializers across multiple constructors.
- Impossible to mix initialization order and declaration order.
- Member type, property flags, and default values are all in one place.

Downsides:

- Any change to defaults requires a rebuild of all dependent files.
- Headers can't change in patch releases of the engine.
- Some things can't be initialized this way (base classes, UObject subobjects, pointers to forward-declared types, values deduced from constructor arguments, members initialized over multiple steps).
- Mixing initializers in headers and constructors in `.cpp` files can reduce readability.

Default member initializers make more sense with in-game code than engine code. Consider using config files for default values.

## Third Party Code

Whenever you modify code to a library used in the engine, tag your changes with a `//@UE5` comment plus an explanation of why you made the change. This makes merging easier and ensures licensees can find modifications.

Any third party code included in the engine should be marked with comments formatted to be easily searchable, using the `// @third party code - BEGIN [LibName]` and `// @third party code - END [LibName]` pattern.

## Code Formatting

### Braces

Braces always go on a **new line**, regardless of the size of the function or block. Always include braces even in single-statement blocks.

### If - Else

Each block of execution in an `if-else` statement should always be in braces. This prevents editing mistakes where an extra line is added that falls outside the `if` expression.

A multi-way `if` statement should be indented with each `else if` indented the same amount as the first `if`.

### Tabs and Indenting

- Indent code by execution block.
- Use **tabs** for whitespace at the beginning of a line, not spaces. Set your tab size to 4 characters.
- Spaces are sometimes necessary to keep code aligned regardless of tab size (e.g., when aligning code that follows non-tab characters).
- In C#, also use tabs, not spaces. Visual Studio defaults to spaces for C# files — remember to change this setting.

### Switch Statements

- Except for empty cases (multiple cases having identical code), each `case` should explicitly either include a `break` or a "falls through" comment.
- Other control-transfer commands (`return`, `continue`, etc.) are fine.
- Always have a `default` case with a `break` in case someone adds a new case after the default.

### Namespaces

- Most UE code is not wrapped in a global namespace.
- Be careful to avoid collisions in the global scope, especially when using or including third party code.
- Namespaces are **not supported** by `UnrealHeaderTool`.
- Namespaces should **not** be used when defining `UCLASSes`, `USTRUCTs`, and so on.
- New APIs that aren't `UCLASSes`, `USTRUCTs` etc. should be placed in a `UE::` namespace, ideally a nested one (e.g., `UE::Audio::`).
- Implementation details not part of the public-facing API should go in a `Private` namespace (e.g., `UE::Audio::Private::`).
- Do **not** put `using` declarations in the global scope, even in a `.cpp` file (it causes problems with the "unity" build system).
- `using` declarations are okay within another namespace or within a function body.
- Forward-declared types need to be declared within their respective namespace.
- Macros cannot live in a namespace. They should be prefixed with `UE_` instead (e.g., `UE_LOG`).

## Physical Dependencies

- File names should not be prefixed where possible (e.g., `Scene.cpp` instead of `UScene.cpp`).
- All headers should protect against multiple includes with the `#pragma once` directive.
- Minimize physical coupling. Avoid including standard library headers from other headers.
- Forward declarations are preferred to including headers.
- When including a header, be as fine-grained as possible. Do not include `Core.h`; include the specific headers you need.
- Include every header you need directly. Do not rely on headers included indirectly by another header.
- Modules have `Private` and `Public` source directories. Definitions needed by other modules must be in the `Public` directory. Everything else goes in `Private`.
- Do not worry about setting up headers for precompiled header generation — `UnrealBuildTool` handles this.
- Split large functions into logical sub-functions to help compiler optimization and reduce build times.
- Do not use a large number of inline functions. Inline functions force rebuilds even in files that don't use them. Use them only for trivial accessors and where profiling shows a benefit.
- Be conservative with `FORCEINLINE`. All code and local variables will expand into the calling function, causing the same build time problems as large functions.

## Encapsulation

Enforce encapsulation with the protection keywords. Class members should almost always be declared `private` unless they are part of the public/protected interface to the class.

- If particular fields are only intended to be usable by derived classes, make them `private` and provide `protected` accessors.
- Use `final` if your class is not designed to be derived from.

## General Style Issues

- **Minimize dependency distance.** Set a variable's value right before using it. Initializing a variable at the top of a block and using it 100 lines later creates room for accidental changes.
- **Split methods into sub-methods where possible.** It is easier to understand a simple method that calls a sequence of well-named sub-methods than to understand an equivalent monolithic method.
- In function declarations or call sites, **do not add a space** between the function's name and the parentheses preceding the argument list.
- **Address compiler warnings.** Fix what the compiler warns about. If absolutely necessary, use `#pragma` to suppress warnings as a last resort.
- **Leave a blank line at the end of the file.** All `.cpp` and `.h` files should include a blank line to coordinate with gcc.
- **Debug code should either be useful and polished, or not checked in.** Debug code intermixed with other code makes it harder to read.
- **Always use the `TEXT()` macro** around string literals. Without it, code constructing `FStrings` from literals will cause an undesirable string conversion process.
- **Avoid repeating the same operation redundantly in loops.** Move common subexpressions out of loops. Use statics in some cases to avoid globally-redundant operations across function calls.
- **Be mindful of hot reload.** Minimize dependencies to cut down on iteration time. Don't use inlining or templates for functions likely to change over a reload.
- **Use intermediate variables to simplify complicated expressions.** Assign sub-expressions to named intermediate variables that describe the meaning of the sub-expression.
- **Pointers and references should only have one space to the right** of the pointer or reference symbol (e.g., `FShaderType* Ptr`). This makes Find-in-Files searches easier.
- **Shadowed variables are not allowed.** C++ allows variables to be shadowed from an outer scope, but this makes usage ambiguous to a reader.
- **Avoid using anonymous literals in function calls.** Prefer named constants that describe their meaning. This makes intent obvious and avoids having to look up the function declaration.
- **Avoid defining non-trivial static variables in headers.** Non-trivial static variables cause an instance to be compiled into every translation unit that includes that header. Use `extern` declarations in headers and define in `.cpp` files instead.
- **Avoid making extensive changes that do not change code behavior** (e.g., changing whitespace or mass renaming private variables). These cause unnecessary noise in source history and are disruptive when merging. If such a change is important, submit it on its own, not mixed with behavioral changes.

## API Design Guidelines

- **Avoid boolean function parameters.** Boolean flags tend to multiply over time as APIs get extended. Instead, prefer an enum (see Strongly-Typed Enums). It is acceptable to use `bool` when it is the complete state passed to a function like a setter (e.g., `void FWidget::SetEnabled(bool bEnabled)`).
- **Avoid overly-long function parameter lists.** If a function takes many parameters, consider passing a dedicated struct instead.
- **Avoid overloading functions by `bool` and `FString`.** This can have unexpected behavior since string literals may resolve to the `bool` overload.
- **Interface classes should always be abstract.** Interface classes are prefixed with `I` and must not have member variables. Interfaces are allowed to contain methods that are not pure-virtual, and can contain non-virtual or static methods if they are implemented inline.
- **Use both `virtual` and `override` keywords** when declaring an overriding method in a derived class.
- **UObjects should be passed around by pointer, not reference.** If null is not expected by a function, this should be documented by the API or handled appropriately.

## Platform-Specific Code

Platform-specific code should always be abstracted and implemented in platform-specific source files in appropriately named subdirectories (e.g., `Engine/Platforms/[PLATFORM]/Source/Runtime/Core/Private/[PLATFORM]PlatformMemory.cpp`).

- In general, avoid adding any uses of `PLATFORM_[PLATFORM]` outside of a directory named `[PLATFORM]`. Instead, extend the hardware abstraction layer (e.g., in `FPlatformMisc`) to add a static function. Force-inlining the function gives it the same performance characteristics as a define.
- Where a define is absolutely necessary, create new `#define` directives that describe particular platform properties (e.g., `PLATFORM_USE_PTHREADS`). Set the default value in `Platform.h` and override for specific platforms in their platform-specific `Platform.h`.
- Centralizing platform-specific details makes it easier to maintain the engine across multiple platforms and port code to new platforms without scouring the codebase for platform-specific defines.
- Keeping platform code in platform-specific folders is also a **requirement** for NDA platforms such as PlayStation, Xbox, and Nintendo Switch.
- Ensure that code compiles and runs regardless of whether the `[PLATFORM]` subdirectory is present. Cross-platform code should never be dependent on platform-specific code.