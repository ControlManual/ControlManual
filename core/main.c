#include <cm.h>
#include <stdio.h>

int main() {
    node* list = linked_list_new("0");
    node* a = linked_list_append(list, "1");
    node* b = linked_list_append(a, "2");
    node* c = linked_list_append(b, "3");
    node* d = linked_list_append(c, "4");
    puts(linked_list_get(a, 3));

    linked_list_free(list);

    return 0;
}