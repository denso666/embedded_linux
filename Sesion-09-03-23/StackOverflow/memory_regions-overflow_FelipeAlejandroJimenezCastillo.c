#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var3 = 0;
void foo2(void) {
    printf("I've just printfed \n");
}

void foo(int var4) {
    static int var1 = 0;
    int var2 = 0;
    int* var5;
    void (*var6)(void) = &foo2;
    var1++;
    var2++;
    var3++;
    var4++;
    
    var5 = (int*)malloc(sizeof(int));
    
    (*var5)++;
    
    printf("var1: %d at %p\n",var1, &var1);
    printf("var2: %d at %p\n",var2, &var2);
    printf("var3: %d at %p\n",var3, &var3);
    printf("var4: %d at %p\n",var4, &var4);
    printf("var5: %d at %p\n",*var5, var5);
    printf("var6: - at %p\n", var6);

    /*  Creating a very large array, we overflow the (fixed size) stack, since local scope variables are allocated in this structure  */
    foo(var4);
}

int main() {
    for (int i = 0 ; i < 5; i++) foo(0);
    return 0;
}
