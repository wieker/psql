#include "type_cast.h"
#include "table.h"

int
type_cast(union cpt_result* cpt, int dst_type)
{
	union cpt_result* sv;
	//cpl_printf("test %d %d\n", cpt->type, dst_type);
	if (dst_type == cpt->type)
		return 1;
	
	if (cpt->type == CPT_RES_NAME && dst_type == CPT_RES_NUMBER) {
		if (! table_get_element(variables, cpt->name.name, (void **) &sv))
			return 0;
		*cpt = *sv;
		return 1;
	}
	
	return 0;
}
