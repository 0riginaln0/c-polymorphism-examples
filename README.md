# c-polymorphism-examples

Different ways to write polymorphic code in C

- `tagged_union.c`: Polymorphism via switch statement that dispatches based on the tag.
- `base_embedding.c`: Polymorphism via an embedded struct containing one or more callback function pointers. Specific behavior is placed into these slots, and dispatching occurs by invoking the appropriate callback.
- `multimethods.c`: Polymorphism via a dynamic dispatch system using a registry of named multimethods. Each multimethod maps string keys to function pointers (methods), enabling runtime selection and invocation of the appropriate method based on the dispatch value.
