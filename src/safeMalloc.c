/* safeMalloc.h
 * Some wrappers around alloc functions that check if the allocation succeeded.
 */
 
 #include <stdlib.h>
 #include <stdio.h>

 void* safeMalloc(size_t num){
     void* p = malloc(num);
     if(p == NULL){
         printf("ERROR: malloc failed");
         exit(1);
     } else {
         return p;
     }
 }

 void* safeCalloc(size_t num, size_t size){
     void* p = calloc(num, size);
     if(p == NULL){
         printf("ERROR: malloc failed");
         exit(1);
     } else {
         return p;
     }
 }

 void* safeRealloc(void* p, size_t num){
     p = realloc(p, num);
     if(p == NULL){
         printf("ERROR: malloc failed");
         exit(1);
     } else {
         return p;
     }
 }
