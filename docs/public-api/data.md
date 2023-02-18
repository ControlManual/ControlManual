# Data

## What is the data API?

In Control Manual, the `data` type refers to a structure that acts as a reference to a `void*`.
It's purpose it to keep memory management simple inside of Control Manual.

When we create a new `data` object, we need to specify how we would like it to be handled once our reference (`data*`) is freed.

There are two main macros for creating a new reference:

```c
data* d = STACK_DATA("some data");
data* d2 = HEAP_DATA(strdup("some heap data"));

/* ... */

data_free(d);
data_free(d2);
```

In the above example, we create two references, `d` and `d2`. Then, once we are done using them, we free them through `data_free`.

We marked `d` as data on the stack, so we don't try and free the `"some data"` string inside of it. On the other hand, the `"some heap data"` string inside of `d2` **does** get freed, since we marked it as heap data.

Once something has been passed into a `HEAP_DATA`, you are telling Control Manual that it's responsible for managing that memory through `data_free` later. **Attempting to free the content of heap data will result in a double free (unless the data itself was never freed).**

If you don't want Control Manual to free it and you would like to manage the memory yourself, you should use the `NOFREE_DATA` macro:

```c
data* d = STACK_DATA("some data");
char* my_str = strdup("some heap data");
data* d2 = NOFREE_DATA(my_str);

/* ... */

data_free(d);
data_free(d2);
free(my_str);
```

**`NOFREE_DATA` is exactly the same as `STACK_DATA`**, but `STACK_DATA` is reserved for items that are truly on the stack.

You may also pass your own deallocator through the `CUSTOM_DATA` macro:

```c
typedef struct STRUCT_SOMETHING {
    char* foo;
} something;

void something_dealloc(something* s) {
    free(s->foo);
}

PLUGIN_NAME(foo)
PLUGIN() {
    something s = {
        .foo = strdup("bar")
    };
    data* d = CUSTOM_DATA(&s, something_dealloc);
    data_free(d);
}
```

## How is data used?

Data is typically used in structs, and is freed once the parent struct is deallocated.

Here's an example using `vector`:

```c
vector* v = vector_new();
vector_append(v, STACK_DATA("hello world"));
vector_free(v);
/* both v and the stack data passed into it are now freed */
```

Passing any sort of data into a Control Manual API function will be freed later.

## Accessing data

You can access data through `data_content`. Note that `data_content` is an inline function, so don't feel too bad about calling it multiple times in a row.

Here's an example:

```c
data* my_data = STACK_DATA("hi");
puts((char*) data_content(my_data));
data_free(my_data);
```

In the above code, you can see one of the large drawbacks of the data API. `data_content` returns `void*`, so we need to cast it to a `char*` in our example. This can get a bit ugly pretty quickly, especially with structs:

```c
data* my_data = HEAP_DATA(some_struct);
puts(((some_struct*) data_content(my_data))->some_param);
data_free(my_data);
```

To clean things up a bit, you can resort to using the `CONTENT_CAST` macro:

```c
data* my_data = HEAP_DATA(some_struct);
puts(CONTENT_CAST(my_data, some_struct*)->some_param);
data_free(my_data);
```

If your data is a `char*`, you may alternatively use `CONTENT_STR`:

```c
data* my_data = STACK_DATA("hi");
puts(CONTENT_STR(my_data));
data_free(my_data);
```

## Reference counts

There may be a case where you either need your data to outlive its parent or survive long enough to work in something else. You can do this by generating a new data object via `data_from`.

The underlying object will never be freed as long as their is another reference to it. Let's once again use vectors as an example:

```c
char* my_str = strdup("heap data");
data* my_str_data = HEAP_DATA(my_str);
vector* v = vector_new();
vector* v2 = vector_new();

/* first, we generate a new reference through data_from, and this reference is then stolen by vector_append */
vector_append(v, data_from(my_str_data));
/* now, we can give away our original reference */
vector_append(v2, my_str_data);

vector_free(v);
puts(CONTENT_STR(vector_get(v2, 0))); // our string is still alive!
vector_free(v2);
// our string is now safely deallocated
```

If you really need to, you may also manually call `data_inc` and `data_dec` to mess with the data's reference count:

```c
char* my_str = strdup("heap data");
data* my_str_data = HEAP_DATA(my_str);
data_inc(my_str_data);

/* ... */

data_dec(my_str_data);
data_free(my_str_data);
```

## Object data

In some cases, you might need a reference to an `object*`. For this, you need to use the `OBJECT_DATA` macro in order to make sure it's deallocated properly later:

```c
object* my_int = integer_from(42);
vector* v = vector_new();
vector_append(v, OBJECT_DATA(my_int));
/* ... */
vector_free(v)
/* object_dealloc gets called */
```

## Terminology

Throughout the documentation, we will talk about stealing, borrowing, and creating new references, but what do these terms actually mean and what do they look like in the context of the data API?

### New References

A new reference means that **the underlying `data` reference is yours.** It's your job to free it or pass it off to something else.

For example, a new reference is generated every time you call `data_new` or `data_from`:

```c
data* my_data = HEAP_DATA(strdup("hi")) // calls data_new
// my_data is our own reference, we need to manage it
```

### Borrowed References

A borrowed reference is a bit more complicated. When dealing with a borrowed reference, **the underlying reference count is not increased.** However, we know that the `data` will not be freed in the duration that we use it.

Typically, a borrowed reference in Control Manual just uses the underlying `void*` inside of the `data`:

```c
data* d = STACK_DATA("hi");
char* ref = CONTENT_STR(d); // borrow a reference to d
/* do something with ref */
data_free(d);
```

**A borrowed reference should never be stored or outlive the `data`.**

### Stolen References

A stolen reference **does not increase the reference count, and should no longer be used by the user.**

Whenever a function takes in a `data*` parameter, it is _stealing_ that reference. Use `vector_append` as an example:

```c
vector* v = vector_new();
data* d = STACK_DATA("hi");
vector_append(v, d);
/* d gets stolen by the vector, we should no longer use it */
vector_free(d);
```
