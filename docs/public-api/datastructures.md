# Datastructures

## Introduction

Control Manual comes with several datastructures right out of the box. Each one of them follows the naming scheme of `structure_do_action`.

**Note:** Not all datastructures have the same features or methods! A more used type, like `vector`, has more associated functions with it opposed to `list`.

-   Allocation methods are always in the form of `struct_new`
-   Deallocation is always done through `struct_free`

## Vectors

Vectors are the simplest datastructure inside of Control Manual. They store two things:

-   An array of `data` references
-   That array's size

At the end of the vectors life (`vector_free` is called), each reference is deallocated.

### Appending and insertion

Appending is done through `vector_append`:

```c
vector* v = vector_new();
vector_append(v, STACK_DATA("hello world"));
vector_free(v);
```

Insertion is done through `vector_insert`:

```c
vector* v = vector_new();
vector_append(v, STACK_DATA("1"));
vector_append(v, STACK_DATA("2"));
vector_append(v, STACK_DATA("4"));
vector_insert(v, 2, STACK_DATA("3"));
vector_free(v);
```

Appending is always O(1), while insertion is O(n) (unless the target index is the length of the vector, which it will then just append).

### Getting values

Because of the way references work inside of Control Manual, vectors have two main methods for getting values: `vector_get` and `vector_get_data`.
