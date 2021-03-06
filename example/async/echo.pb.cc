// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: echo.proto

#include "echo.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

namespace example {
class emptyDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<empty> _instance;
} _empty_default_instance_;
class requestDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<request> _instance;
} _request_default_instance_;
class responseDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<response> _instance;
} _response_default_instance_;
}  // namespace example
static void InitDefaultsempty_echo_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::example::_empty_default_instance_;
    new (ptr) ::example::empty();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::example::empty::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_empty_echo_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsempty_echo_2eproto}, {}};

static void InitDefaultsrequest_echo_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::example::_request_default_instance_;
    new (ptr) ::example::request();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::example::request::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_request_echo_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsrequest_echo_2eproto}, {}};

static void InitDefaultsresponse_echo_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::example::_response_default_instance_;
    new (ptr) ::example::response();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::example::response::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_response_echo_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsresponse_echo_2eproto}, {}};

void InitDefaults_echo_2eproto() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_empty_echo_2eproto.base);
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_request_echo_2eproto.base);
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_response_echo_2eproto.base);
}

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_echo_2eproto[3];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_echo_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* file_level_service_descriptors_echo_2eproto[1];

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_echo_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::example::empty, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::example::request, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::example::request, msg_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::example::response, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::example::response, msg_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::example::empty)},
  { 5, -1, sizeof(::example::request)},
  { 11, -1, sizeof(::example::response)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::example::_empty_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::example::_request_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::example::_response_default_instance_),
};

static ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptorsTable assign_descriptors_table_echo_2eproto = {
  {}, AddDescriptors_echo_2eproto, "echo.proto", schemas,
  file_default_instances, TableStruct_echo_2eproto::offsets,
  file_level_metadata_echo_2eproto, 3, file_level_enum_descriptors_echo_2eproto, file_level_service_descriptors_echo_2eproto,
};

const char descriptor_table_protodef_echo_2eproto[] =
  "\n\necho.proto\022\007example\"\007\n\005empty\"\026\n\007reques"
  "t\022\013\n\003msg\030\001 \001(\t\"\027\n\010response\022\013\n\003msg\030\001 \001(\t2"
  "\242\001\n\013EchoService\022+\n\004Echo\022\020.example.reques"
  "t\032\021.example.response\0223\n\014EchoMulParam\022\020.e"
  "xample.request\032\021.example.response\0221\n\014Reg"
  "isterEcho\022\016.example.empty\032\021.example.resp"
  "onseB\003\200\001\001b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_echo_2eproto = {
  false, InitDefaults_echo_2eproto, 
  descriptor_table_protodef_echo_2eproto,
  "echo.proto", &assign_descriptors_table_echo_2eproto, 257,
};

void AddDescriptors_echo_2eproto() {
  static constexpr ::PROTOBUF_NAMESPACE_ID::internal::InitFunc deps[1] =
  {
  };
 ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_echo_2eproto, deps, 0);
}

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_echo_2eproto = []() { AddDescriptors_echo_2eproto(); return true; }();
namespace example {

// ===================================================================

void empty::InitAsDefaultInstance() {
}
class empty::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

empty::empty()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:example.empty)
}
empty::empty(const empty& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:example.empty)
}

void empty::SharedCtor() {
}

empty::~empty() {
  // @@protoc_insertion_point(destructor:example.empty)
  SharedDtor();
}

void empty::SharedDtor() {
}

void empty::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const empty& empty::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_empty_echo_2eproto.base);
  return *internal_default_instance();
}


void empty::Clear() {
// @@protoc_insertion_point(message_clear_start:example.empty)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* empty::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      default: {
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          return ptr;
        }
        ptr = UnknownFieldParse(tag,
          _internal_metadata_.mutable_unknown_fields(), ptr, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);
        break;
      }
    }  // switch
  }  // while
  return ptr;
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool empty::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:example.empty)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
  handle_unusual:
    if (tag == 0) {
      goto success;
    }
    DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SkipField(
          input, tag, _internal_metadata_.mutable_unknown_fields()));
  }
success:
  // @@protoc_insertion_point(parse_success:example.empty)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:example.empty)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void empty::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:example.empty)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:example.empty)
}

::PROTOBUF_NAMESPACE_ID::uint8* empty::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:example.empty)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:example.empty)
  return target;
}

size_t empty::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:example.empty)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void empty::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:example.empty)
  GOOGLE_DCHECK_NE(&from, this);
  const empty* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<empty>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:example.empty)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:example.empty)
    MergeFrom(*source);
  }
}

void empty::MergeFrom(const empty& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:example.empty)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

}

void empty::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:example.empty)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void empty::CopyFrom(const empty& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:example.empty)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool empty::IsInitialized() const {
  return true;
}

void empty::Swap(empty* other) {
  if (other == this) return;
  InternalSwap(other);
}
void empty::InternalSwap(empty* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::PROTOBUF_NAMESPACE_ID::Metadata empty::GetMetadata() const {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::assign_descriptors_table_echo_2eproto);
  return ::file_level_metadata_echo_2eproto[kIndexInFileMessages];
}


// ===================================================================

void request::InitAsDefaultInstance() {
}
class request::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int request::kMsgFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

request::request()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:example.request)
}
request::request(const request& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.msg().size() > 0) {
    msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.msg_);
  }
  // @@protoc_insertion_point(copy_constructor:example.request)
}

void request::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(
      &scc_info_request_echo_2eproto.base);
  msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

request::~request() {
  // @@protoc_insertion_point(destructor:example.request)
  SharedDtor();
}

void request::SharedDtor() {
  msg_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void request::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const request& request::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_request_echo_2eproto.base);
  return *internal_default_instance();
}


void request::Clear() {
// @@protoc_insertion_point(message_clear_start:example.request)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  msg_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* request::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // string msg = 1;
      case 1: {
        if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) != 10) goto handle_unusual;
        ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_msg(), ptr, ctx, "example.request.msg");
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        break;
      }
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          return ptr;
        }
        ptr = UnknownFieldParse(tag,
          _internal_metadata_.mutable_unknown_fields(), ptr, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);
        break;
      }
    }  // switch
  }  // while
  return ptr;
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool request::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:example.request)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string msg = 1;
      case 1: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (10 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_msg()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->msg().data(), static_cast<int>(this->msg().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "example.request.msg"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:example.request)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:example.request)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void request::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:example.request)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), static_cast<int>(this->msg().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "example.request.msg");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->msg(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:example.request)
}

::PROTOBUF_NAMESPACE_ID::uint8* request::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:example.request)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), static_cast<int>(this->msg().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "example.request.msg");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        1, this->msg(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:example.request)
  return target;
}

size_t request::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:example.request)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->msg());
  }

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void request::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:example.request)
  GOOGLE_DCHECK_NE(&from, this);
  const request* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<request>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:example.request)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:example.request)
    MergeFrom(*source);
  }
}

void request::MergeFrom(const request& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:example.request)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.msg().size() > 0) {

    msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.msg_);
  }
}

void request::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:example.request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void request::CopyFrom(const request& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:example.request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool request::IsInitialized() const {
  return true;
}

void request::Swap(request* other) {
  if (other == this) return;
  InternalSwap(other);
}
void request::InternalSwap(request* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  msg_.Swap(&other->msg_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
}

::PROTOBUF_NAMESPACE_ID::Metadata request::GetMetadata() const {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::assign_descriptors_table_echo_2eproto);
  return ::file_level_metadata_echo_2eproto[kIndexInFileMessages];
}


// ===================================================================

void response::InitAsDefaultInstance() {
}
class response::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int response::kMsgFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

response::response()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:example.response)
}
response::response(const response& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from.msg().size() > 0) {
    msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.msg_);
  }
  // @@protoc_insertion_point(copy_constructor:example.response)
}

void response::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(
      &scc_info_response_echo_2eproto.base);
  msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

response::~response() {
  // @@protoc_insertion_point(destructor:example.response)
  SharedDtor();
}

void response::SharedDtor() {
  msg_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void response::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const response& response::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_response_echo_2eproto.base);
  return *internal_default_instance();
}


void response::Clear() {
// @@protoc_insertion_point(message_clear_start:example.response)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  msg_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* response::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // string msg = 1;
      case 1: {
        if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) != 10) goto handle_unusual;
        ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_msg(), ptr, ctx, "example.response.msg");
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        break;
      }
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          return ptr;
        }
        ptr = UnknownFieldParse(tag,
          _internal_metadata_.mutable_unknown_fields(), ptr, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr != nullptr);
        break;
      }
    }  // switch
  }  // while
  return ptr;
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool response::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:example.response)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string msg = 1;
      case 1: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (10 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_msg()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->msg().data(), static_cast<int>(this->msg().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "example.response.msg"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:example.response)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:example.response)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void response::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:example.response)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), static_cast<int>(this->msg().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "example.response.msg");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->msg(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:example.response)
}

::PROTOBUF_NAMESPACE_ID::uint8* response::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:example.response)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->msg().data(), static_cast<int>(this->msg().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "example.response.msg");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        1, this->msg(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:example.response)
  return target;
}

size_t response::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:example.response)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string msg = 1;
  if (this->msg().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->msg());
  }

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void response::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:example.response)
  GOOGLE_DCHECK_NE(&from, this);
  const response* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<response>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:example.response)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:example.response)
    MergeFrom(*source);
  }
}

void response::MergeFrom(const response& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:example.response)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.msg().size() > 0) {

    msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.msg_);
  }
}

void response::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:example.response)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void response::CopyFrom(const response& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:example.response)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool response::IsInitialized() const {
  return true;
}

void response::Swap(response* other) {
  if (other == this) return;
  InternalSwap(other);
}
void response::InternalSwap(response* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  msg_.Swap(&other->msg_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
}

::PROTOBUF_NAMESPACE_ID::Metadata response::GetMetadata() const {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::assign_descriptors_table_echo_2eproto);
  return ::file_level_metadata_echo_2eproto[kIndexInFileMessages];
}


// ===================================================================

EchoService::~EchoService() {}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* EchoService::descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&assign_descriptors_table_echo_2eproto);
  return file_level_service_descriptors_echo_2eproto[0];
}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* EchoService::GetDescriptor() {
  return descriptor();
}

void EchoService::Echo(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                         const ::example::request*,
                         ::example::response*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method Echo() not implemented.");
  done->Run();
}

void EchoService::EchoMulParam(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                         const ::example::request*,
                         ::example::response*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method EchoMulParam() not implemented.");
  done->Run();
}

void EchoService::RegisterEcho(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                         const ::example::empty*,
                         ::example::response*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method RegisterEcho() not implemented.");
  done->Run();
}

void EchoService::CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                             ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                             const ::PROTOBUF_NAMESPACE_ID::Message* request,
                             ::PROTOBUF_NAMESPACE_ID::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), file_level_service_descriptors_echo_2eproto[0]);
  switch(method->index()) {
    case 0:
      Echo(controller,
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<const ::example::request*>(
                 request),
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<::example::response*>(
                 response),
             done);
      break;
    case 1:
      EchoMulParam(controller,
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<const ::example::request*>(
                 request),
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<::example::response*>(
                 response),
             done);
      break;
    case 2:
      RegisterEcho(controller,
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<const ::example::empty*>(
                 request),
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<::example::response*>(
                 response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& EchoService::GetRequestPrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::example::request::default_instance();
    case 1:
      return ::example::request::default_instance();
    case 2:
      return ::example::empty::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->input_type());
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& EchoService::GetResponsePrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::example::response::default_instance();
    case 1:
      return ::example::response::default_instance();
    case 2:
      return ::example::response::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->output_type());
  }
}

EchoService_Stub::EchoService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
EchoService_Stub::EchoService_Stub(
    ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
    ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::PROTOBUF_NAMESPACE_ID::Service::STUB_OWNS_CHANNEL) {}
EchoService_Stub::~EchoService_Stub() {
  if (owns_channel_) delete channel_;
}

void EchoService_Stub::Echo(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::example::request* request,
                              ::example::response* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}
void EchoService_Stub::EchoMulParam(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::example::request* request,
                              ::example::response* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(1),
                       controller, request, response, done);
}
void EchoService_Stub::RegisterEcho(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::example::empty* request,
                              ::example::response* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(2),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace example
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::example::empty* Arena::CreateMaybeMessage< ::example::empty >(Arena* arena) {
  return Arena::CreateInternal< ::example::empty >(arena);
}
template<> PROTOBUF_NOINLINE ::example::request* Arena::CreateMaybeMessage< ::example::request >(Arena* arena) {
  return Arena::CreateInternal< ::example::request >(arena);
}
template<> PROTOBUF_NOINLINE ::example::response* Arena::CreateMaybeMessage< ::example::response >(Arena* arena) {
  return Arena::CreateInternal< ::example::response >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
