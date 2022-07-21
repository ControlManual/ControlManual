#include <core.h>

arbitrary_data* data_new(bool should_free, void* contents) {
    arbitrary_data* data = malloc(sizeof(arbitrary_data));
    if (!data) NOMEM("heap_data_new");

    data->should_free = true;
    data->contents = contents;
    return data;
}

void data_free(arbitrary_data* data) {
    NONULL(data, "data_free");
    if (data->should_free) free(data->contents);

    free(data);
}