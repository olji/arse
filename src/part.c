#include <stdio.h>
#include <stdlib.h>
#include "part.h"
#include "piece.h"

#include "debug.h"

struct part *part_create(struct piece *first, struct piece *last, int deepcopy){
  debug("Creating part\n");
  debug("first: %p\n-last: %p\n", (void*)first, (void*)last);
  struct part *p = malloc(sizeof(struct part));
  if(deepcopy){
    debug("Copying object %p\n", (void*)first);
    p->first = piece_copy(first);
    struct piece *tmp = first;
    struct piece *p_tmp = p->first;
    while(tmp != last){
      tmp = tmp->next;
      debug("Copying object %p\n", (void*)tmp);
      struct piece *next = piece_copy(tmp);
      p_tmp->next = next;
      next->previous = p_tmp;
      p_tmp = next;
    }
    /* Copy last */
    p->last = p_tmp;
  } else {
    p->first = piece_copy(first);
    p->last = piece_copy(last);
  }
  debug("Part created\n");
  return p;

}
void part_delete(struct part *p, int deep){
  if(deep){
    piece_delete_to(p->first, p->last);
  }
  free(p);
}
int part_length(struct part *p){
  return 0;
}
