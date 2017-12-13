/**********************************************************
 *  \!file protocol.cpp
 *  \!brief
 *  \!note	注意事项： 
 * 			1,类中的成员函数中的同名参数的含义完全相同。仅会注释其中一个函数，其他函数则不再重复注释。重名的参数意义不同时，会独立注解。 
 * 			2,第1条的规则同样适用于返回值的含义。 
 * 
 * \!version 
 * * \!author zheng39562@163.com
**********************************************************/
#include "fr_tcp/protocol.h"

#include "fr_tcp/tcp_define.h"

using namespace universal;

bool proto_pack(BinaryMemory &unpacket){
	if(unpacket.empty()){ return false; }

	Byte* pHead = new Byte[PROTO_HEAD_SIZE];
	proto_sizeToByte(unpacket.size(), pHead);
	// Verification
	pHead[PROTO_HEAD_VERIFY_INDEX] = ((unsigned char*)unpacket.buffer())[unpacket.size() - 1];

	BinaryMemory packetTmp((const void*)pHead, PROTO_HEAD_SIZE);
	unpacket = packetTmp + unpacket;

	if(pHead != NULL){
		delete pHead; pHead = NULL;
	}

	return true;
}

bool proto_unpack(BinaryMemory &packet){
	if(proto_Verification(packet) != 0){
		TCP_DEBUG_E("解包出错：验证包失败。");
		return false;
	}

	packet.del(0, PROTO_HEAD_SIZE);
	return true;
}

int proto_Verification(const universal::BinaryMemory &binary){
	short size = *(short*)(binary.buffer());
	if(binary.size() != size + PROTO_HEAD_SIZE){
		TCP_DEBUG_E("包头内的包长度[" << binary.size() << "]和实际包长度[" << size + PROTO_HEAD_SIZE << "]不一致。");
		return 1;
	}

	Byte dataVerify = ((Byte*)binary.buffer())[PROTO_HEAD_VERIFY_INDEX];
	Byte lastByte = ((Byte*)binary.buffer())[binary.size() - 1];
	if(dataVerify != lastByte){
		TCP_DEBUG_E("字节验证失败。验证字节[" << dataVerify << "] 待验证字节[" << lastByte << "]");
		return 2;
	}

	return 0;
}

proto_size proto_bypeToSize(Byte* pHead){
	if(pHead == NULL){
		TCP_DEBUG_E("包头未分配内存。");
		return 0;
	}
	proto_size* pSize = (proto_size*)pHead;
	return *pSize;
}

void proto_sizeToByte(proto_size size, Byte* pHead){
	if(pHead == NULL){
		TCP_DEBUG_E("包头未分配内存。");
		return ;
	}
	proto_size* pSize = (proto_size*)pHead;

	*pSize = size;
}



