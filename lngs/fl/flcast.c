#include "utils.h"
#include "gen_attr.h"
#include <string.h>
#include "atttree_helper.h"
#include "atrex.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "fl.h"
#include "scalc.h"

#include "table.h"

                                                                                                                                                                                
int                                                                                                                                                                               
try_cast(struct exec_descr* ctx, struct attnode* node, struct fl_context* input, union fl_result* cast)                                                                           
{                                                                                                                                                                                 
      struct fl_function *stack;                                                                                                                                                 
      if (cast->function->argcount - cast->function->closed)                                                                                                                     
              return 0;                                                                                                                                                          
      stack = input->closure;                                                                                                                                                    
      input->closure = cast->function;                                                                                                                                           
      if (! call_atree(ctx, cast->function->expr, input))                                                                                                                        
              return 0;                                                                                                                                                          
      input->closure = stack;
      return 1;                                                                                                                                                                  
}

