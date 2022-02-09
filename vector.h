//
#include <malloc.h>
#include <string.h>
//#include <assert.h>

typedef struct{
    void** items;
    size_t length;
    size_t capacity;
} Vector;

Vector _tcs_split(const _TCHAR* str, const _TCHAR* delims) {
    Vector Str;
    _TCHAR *str_t, *p;
    void *pp;
    //
    str_t = _tcsdup(str);
    //
    Str.length = 0;
    Str.items = (void**)calloc(1, sizeof(void**));
    Str.items[0] = str_t;
    //
    p = _tcstok(str_t, delims);
    while (p != NULL) {
        pp = realloc(Str.items, sizeof(void*) * (Str.length+1));
        //assert(pp != NULL);
        Str.items = (void**)pp;
        Str.items[Str.length] = p;
        Str.length++;
        p = _tcstok(NULL, delims);
    }
    if (Str.length == 0) {Str.length = 1;}
    return Str;
}

int array_in_array(Vector *arr_v, Vector *arr_s) {
    size_t i, j;
    int flag_t, flag_i;
    flag_t = 0;
    for (i = 0; i < arr_s->length; i++) {
        flag_i = 0;
        for (j = 0; j < arr_v->length; j++) {
            if (_tcscmp((_TCHAR*)arr_s->items[i], (_TCHAR*)arr_v->items[j]) == 0) {
                flag_i = 1;
                break;
            }
        }
        if (flag_i == 1) {
            flag_t = 1;
        }
        else {
            flag_t = 0;
            break;
        }
    }
    return flag_t;
}
