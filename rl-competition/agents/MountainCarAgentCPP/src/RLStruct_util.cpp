/*
 *  RLStruct_util.cpp
 *  
 *
 *  Created by Leah Hackman on 13/06/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "RLStruct_util.h"


void copyRLStruct(RL_abstract_type& oldStruct, RL_abstract_type newStruct)
{
//Copy the contents of an old RL_abstract_type to the new RL_abstract_type
	unsigned int i =0;
	oldStruct.numInts = newStruct.numInts;
	oldStruct.numDoubles = newStruct.numDoubles;
	for(i=0; i<newStruct.numInts; i++)
	oldStruct.intArray[i] = newStruct.intArray[i];
	for(i=0;i<newStruct.numDoubles;i++)
	oldStruct.doubleArray[i] = newStruct.doubleArray[i];
}