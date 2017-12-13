/**********************************************************
 *  \!file protocol.cpp
 *  \!brief
 *  \!note	ע����� 
 * 			1,���еĳ�Ա�����е�ͬ�������ĺ�����ȫ��ͬ������ע������һ���������������������ظ�ע�͡������Ĳ������岻ͬʱ�������ע�⡣ 
 * 			2,��1���Ĺ���ͬ�������ڷ���ֵ�ĺ��塣 
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
		TCP_DEBUG_E("���������֤��ʧ�ܡ�");
		return false;
	}

	packet.del(0, PROTO_HEAD_SIZE);
	return true;
}

int proto_Verification(const universal::BinaryMemory &binary){
	short size = *(short*)(binary.buffer());
	if(binary.size() != size + PROTO_HEAD_SIZE){
		TCP_DEBUG_E("��ͷ�ڵİ�����[" << binary.size() << "]��ʵ�ʰ�����[" << size + PROTO_HEAD_SIZE << "]��һ�¡�");
		return 1;
	}

	Byte dataVerify = ((Byte*)binary.buffer())[PROTO_HEAD_VERIFY_INDEX];
	Byte lastByte = ((Byte*)binary.buffer())[binary.size() - 1];
	if(dataVerify != lastByte){
		TCP_DEBUG_E("�ֽ���֤ʧ�ܡ���֤�ֽ�[" << dataVerify << "] ����֤�ֽ�[" << lastByte << "]");
		return 2;
	}

	return 0;
}

proto_size proto_bypeToSize(Byte* pHead){
	if(pHead == NULL){
		TCP_DEBUG_E("��ͷδ�����ڴ档");
		return 0;
	}
	proto_size* pSize = (proto_size*)pHead;
	return *pSize;
}

void proto_sizeToByte(proto_size size, Byte* pHead){
	if(pHead == NULL){
		TCP_DEBUG_E("��ͷδ�����ڴ档");
		return ;
	}
	proto_size* pSize = (proto_size*)pHead;

	*pSize = size;
}



