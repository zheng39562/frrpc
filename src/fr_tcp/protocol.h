/**********************************************************
 *  \file protocol.h
 *  \brief
 * 	\note	���Ķ��壺������ͷ�����ݵ�������������
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _protocol_H
#define _protocol_H

// ǰ��ʹ�á�
#ifndef _CLIENT
#include "fr_public/pub_define.h"
#include "fr_public/pub_memory.h"
#else
#include "common/Common.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include "pub_memory.h"
#endif

#define PROTO_HEAD_SIZE	3
#define PROTO_HEAD_PACKET_INDEX	0
#define PROTO_PACKET_SIZE_BYTE 2
#define PROTO_HEAD_VERIFY_INDEX 2
#define PROTO_VERIFY_SIZE_BYTE 1

typedef short proto_size;
//! \brief	�����
//! \param[in,out] unpacket ���ݰ��������ֱ�ӷ��ء�
//! \return true �ɹ��� falseʧ�ܡ�������falseʱ����δ�����ݰ����κβ�����
bool proto_pack(universal::BinaryMemory &unpacket);
//! \brief	���
//! \note	���ʱ��԰���������У�顣
//! \return true �ɹ��� falseʧ�ܡ�������falseʱ����δ�����ݰ����κβ�����
bool proto_unpack(universal::BinaryMemory &packet);
//! \brief	��֤һ�����������ԡ�
//! \retval 0 ��������1 �����Ȳ���ȷ��2������δͨ��У�顣
int proto_Verification(const universal::BinaryMemory &binary);
//! \brief	�Ӱ�ͷ�л�ȡ�����С��
proto_size proto_bypeToSize(Byte* pHead);
//! \brief	�������С�����ݴ����ͷ��
void proto_sizeToByte(proto_size size, Byte* pHead);

#endif 

