/*
*   Copyright (C) 2010
*   Chair of Process Control Engineering,
*   D-52056 Aachen, Germany.
*   All rights reserved.
*
*
*   This file is part of the ACPLT/OV Package 
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/
/******************************************************************************
*
*   FILE
*   ----
*   RIGTH.c
*
*   History
*   -------
*   2010-11-30   File created
*
*******************************************************************************
*
*   This file is generated by the 'fb_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_iec61131stdfb
#define OV_COMPILE_LIBRARY_iec61131stdfb
#endif


#include "iec61131stdfb.h"
#include "libov/ov_macros.h"
#include "libov/ov_logfile.h"
#include "libov/ov_string.h"
#include "libov/ov_malloc.h"


OV_DLLFNCEXPORT OV_RESULT iec61131stdfb_RIGHT_IN_set(
    OV_INSTPTR_iec61131stdfb_RIGHT          pobj,
    const OV_STRING  value
) {
    return ov_string_setvalue(&pobj->v_IN,value);
}

OV_DLLFNCEXPORT OV_RESULT iec61131stdfb_RIGHT_L_set(
    OV_INSTPTR_iec61131stdfb_RIGHT          pobj,
    const OV_UINT  value
) {
    pobj->v_L = value;
    return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_STRING iec61131stdfb_RIGHT_OUT_get(
    OV_INSTPTR_iec61131stdfb_RIGHT          pobj
) {
    return pobj->v_OUT;
}


OV_DLLFNCEXPORT void iec61131stdfb_RIGHT_destructor(OV_INSTPTR_ov_object pobj) {

	OV_INSTPTR_iec61131stdfb_RIGHT pinst = Ov_StaticPtrCast(iec61131stdfb_RIGHT, pobj);
	
	ov_string_setvalue(&pinst->v_IN, "");
	ov_string_setvalue(&pinst->v_OUT, "");
	fb_functionblock_destructor(pobj);
}


OV_DLLFNCEXPORT void iec61131stdfb_RIGHT_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
	
	unsigned int i;
	unsigned int in_length;
	
	OV_STRING p_char;
	
    OV_INSTPTR_iec61131stdfb_RIGHT pinst = Ov_StaticPtrCast(iec61131stdfb_RIGHT, pfb);
	
	ov_string_setvalue(&pinst->v_OUT, "");	//free memory
	in_length = ov_string_getlength(pinst->v_IN);
	if(pinst->v_L <= in_length)
	{
		p_char = (OV_STRING) ov_malloc((pinst->v_L + 1) * sizeof(char));
		if(p_char != NULL)
		{
			for(i=0; i < pinst->v_L; i++)
				p_char[i] = pinst->v_IN[(in_length - pinst->v_L) + i];
			p_char[pinst->v_L] = 0;
			if(Ov_Fail(ov_string_setvalue(&pinst->v_OUT, p_char)))
				ov_logfile_error("%s: allocation of memory failed, no operation performed", pinst->v_identifier);
			ov_free(p_char);
		}
		else
		{
			ov_logfile_error("%s: allocation of memory failed, no operation performed", pinst->v_identifier);
			return;
		}
	}
	else
	{
		ov_string_setvalue(&pinst->v_OUT, pinst->v_IN);
		ov_logfile_warning("%s: specified length is larger than length of input string, copying input", pinst->v_identifier);
	}
	
    return;
}

