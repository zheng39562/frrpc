/**********************************************************
 *  \file frrpc_function.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _frrpc_function_H
#define _frrpc_function_H

#include "public_define.h"
#include "frrpc_define.h"

namespace frrpc{

void AskToQuit();
bool IsAskedToQuit();

// T must a number(int long shor etc).
template <typename Integer>
uint32_t GetNumberLength(Integer integer){
	uint32_t length(0);
	do{
		++length;
		integer /= 10;
	}while((integer - 0) > 0.0000001);

	return length;
}


}// namespace frrpc{


#endif 

