/**********************************************************
 *  \file frrpc_pb2js.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc_pb2js.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// 为什么需要document.GetAllocator

using namespace std;
using namespace google::protobuf;
using namespace rapidjson;

namespace frrpc{

static Document d;
void FormatRepeatedField(Value& value, const ::google::protobuf::Message& msg, const google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection);

void FormatToJson(Value& value, const ::google::protobuf::Message& msg){
	const Descriptor* descriptor = msg.GetDescriptor();
	const Reflection* reflection = msg.GetReflection();

	const int count = descriptor->field_count();
	for(int field_index = 0; field_index < count; ++field_index){
		const FieldDescriptor* field = descriptor->field(field_index);
		if(field->is_repeated()){
			if(reflection->FieldSize(msg, field) > 0){
				Value valueArray(kArrayType);
				FormatRepeatedField(valueArray, msg, field, reflection);
				value.AddMember(Value(field->name().c_str(), d.GetAllocator()).Move(), valueArray.Move(), d.GetAllocator());
			}
			continue;
		}

		if(!reflection->HasField(msg, field)){
			continue;
		}

		Value key(field->name().c_str(), d.GetAllocator());
		switch(field->type()){
			case FieldDescriptor::TYPE_MESSAGE:{
					const Message& tmp_msg = reflection->GetMessage(msg, field);
					if(0 != tmp_msg.ByteSize()){
						FormatToJson(value[field->name().c_str()], tmp_msg);
					}
				}
				break;
			case FieldDescriptor::TYPE_INT32:
				value.AddMember(key, reflection->GetInt32(msg, field), d.GetAllocator());
				break;
			case FieldDescriptor::TYPE_UINT32:
				value.AddMember(key, reflection->GetUInt32(msg, field), d.GetAllocator());
				break;
			case FieldDescriptor::TYPE_INT64:
				value.AddMember(key, reflection->GetInt64(msg, field), d.GetAllocator());
				break;
			case FieldDescriptor::TYPE_UINT64:
				value.AddMember(key, reflection->GetUInt64(msg, field), d.GetAllocator());
				break;
			case FieldDescriptor::TYPE_STRING:
			case FieldDescriptor::TYPE_BYTES:
				value.AddMember(key, Value(reflection->GetString(msg, field).c_str(), d.GetAllocator()).Move(), d.GetAllocator());
				break;
			default: break;
		}
	}
}

void FormatRepeatedField(Value& value, const ::google::protobuf::Message& msg, const google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection){
	if(NULL == field || NULL == reflection){
		FormatToJson(value, msg);
	}

	for(int field_index = 0; field_index < reflection->FieldSize(msg, field); ++field_index){
		Value tmp_value;
		switch(field->type()){
			case FieldDescriptor::TYPE_MESSAGE:{
					const Message& tmp_msg = reflection->GetRepeatedMessage(msg, field, field_index);
					if(0 != tmp_msg.ByteSize()){
						FormatToJson(tmp_value, tmp_msg);
					}
				}
				break;
			case FieldDescriptor::TYPE_INT32:
				tmp_value = reflection->GetRepeatedInt32(msg, field, field_index);
				break;
			case FieldDescriptor::TYPE_UINT32:
				tmp_value = reflection->GetRepeatedUInt32(msg, field, field_index);
				break;
			case FieldDescriptor::TYPE_INT64:
				tmp_value = reflection->GetRepeatedInt64(msg, field, field_index);
				break;
			case FieldDescriptor::TYPE_UINT64:
				tmp_value = reflection->GetRepeatedUInt64(msg, field, field_index);
				break;
			case FieldDescriptor::TYPE_STRING:
			case FieldDescriptor::TYPE_BYTES:{
					const string& smsg = reflection->GetRepeatedString(msg, field, field_index);
					tmp_value.SetString(smsg.c_str(), smsg.size(), d.GetAllocator());
				 }
				break;
			default: break;
		}
		value.PushBack(tmp_value, d.GetAllocator());
	}
}

std::string pbToJson(const google::protobuf::Message& pb_msg){
	Value value(kObjectType);
	FormatToJson(value, pb_msg);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	value.Accept(writer);

	return string(buffer.GetString());
}

} // namespace frrpc

