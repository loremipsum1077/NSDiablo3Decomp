// Template checks for blz::rb_tree with blz::basic_string / embedded_string /
// vector keys and values.
#include "Contrib/Contrib/blz/include/blz/rb_tree.h"
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/map.h"
#include "Contrib/Contrib/blz/include/blz/string.h"
#include "Contrib/Contrib/blz/include/blz/embedded_string.h"
#include "Contrib/Contrib/blz/include/blz/vector.h"

namespace google { namespace protobuf {
class FieldDescriptor;
namespace TextFormat {
struct ParseLocation { int line; int column; };
class ParseInfoTree;
class FieldValuePrinter;
}
} }

typedef blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > BlzString;
typedef blz::basic_embedded_string<char, 32ul, blz::char_traits<char>, blz::allocator<char> > Embedded32;

template class blz::rb_tree<blz::set_traits<BlzString>, blz::less<BlzString>, blz::allocator<BlzString> >;
template class blz::rb_tree<blz::rb_map_traits<int, BlzString>, blz::less<int>, blz::allocator<blz::pair<const int, BlzString> > >;
template class blz::rb_tree<blz::rb_map_traits<BlzString, blz::pair<const void*, int> >, blz::less<BlzString>, blz::allocator<blz::pair<const BlzString, blz::pair<const void*, int> > > >;
template class blz::rb_tree<blz::rb_map_traits<blz::pair<BlzString, int>, blz::pair<const void*, int> >, blz::less<blz::pair<BlzString, int> >, blz::allocator<blz::pair<const blz::pair<BlzString, int>, blz::pair<const void*, int> > > >;
template class blz::rb_tree<blz::rb_map_traits<unsigned long, Embedded32>, blz::less<unsigned long>, blz::allocator<blz::pair<const unsigned long, Embedded32> > >;
template class blz::rb_tree<blz::rb_map_traits<const google::protobuf::FieldDescriptor*, blz::vector<google::protobuf::TextFormat::ParseLocation, blz::allocator<google::protobuf::TextFormat::ParseLocation> > >, blz::less<const google::protobuf::FieldDescriptor*>, blz::allocator<blz::pair<const google::protobuf::FieldDescriptor* const, blz::vector<google::protobuf::TextFormat::ParseLocation, blz::allocator<google::protobuf::TextFormat::ParseLocation> > > > >;
template class blz::rb_tree<blz::rb_map_traits<const google::protobuf::FieldDescriptor*, blz::vector<google::protobuf::TextFormat::ParseInfoTree*, blz::allocator<google::protobuf::TextFormat::ParseInfoTree*> > >, blz::less<const google::protobuf::FieldDescriptor*>, blz::allocator<blz::pair<const google::protobuf::FieldDescriptor* const, blz::vector<google::protobuf::TextFormat::ParseInfoTree*, blz::allocator<google::protobuf::TextFormat::ParseInfoTree*> > > > >;
template class blz::rb_tree<blz::rb_map_traits<const google::protobuf::FieldDescriptor*, const google::protobuf::TextFormat::FieldValuePrinter*>, blz::less<const google::protobuf::FieldDescriptor*>, blz::allocator<blz::pair<const google::protobuf::FieldDescriptor* const, const google::protobuf::TextFormat::FieldValuePrinter*> > >;
