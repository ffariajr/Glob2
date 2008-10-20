from add_stuff_base import *

backup("YOGClientEvent.h")
backup("YOGClientEvent.cpp")

print "Name? "
name = raw_input()
tname = "YE"+name.replace("YOG", "").replace("Event", "")

variables = assemble_variables(False, False)
vn = len(variables)

constructor=assemble_constructor_define(variables)

declare_functions=assemble_declare_get_functions(variables)
declare_variables=assemble_declare_variables(variables)


initialize_variables=""
if vn:
	initialize_variables="	: "
	initialize_variables+=assemble_initialize_variables(variables)
	initialize_variables+="\n"

format_variables = assemble_format_variables(variables)
compare_variables = assemble_compare_variables(variables)
get_function_defines = assemble_get_function_definitions(variables)

hcode = """
///mname
class mname : public YOGClientEvent
{
public:
	///Creates a mname event
	""" + constructor + """;

	///Returns tname
	Uint8 getEventType() const;

	///Returns a formatted version of the event
	std::string format() const;
	
	///Compares two YOGEvent
	bool operator==(const YOGClientEvent& rhs) const;
"""
hcode+=declare_functions
hcode+=declare_variables
hcode+="""};



"""

scode=""

scode+="mname::%s\n" % constructor
scode+=initialize_variables
scode+="{\n}\n\n\n\n"
scode+="""Uint8 mname::getEventType() const
{
	return tname;
}



std::string mname::format() const
{
""" + format_variables + """
	return s.str();
}



bool mname::operator==(const YOGClientEvent& rhs) const
{
	if(typeid(rhs)==typeid(mname))
	{
""" + compare_variables + """
	}
	return false;
}


"""

scode += get_function_defines



lines = readLines("YOGClientEvent.h")
i = findMarker(lines,"type_append_marker")
lines.insert(i, "	%s,\n" % tname)


i = findMarker(lines,"event_append_marker")
lines.insert(i, hcode.replace("mname", name).replace("tname", tname))
writeLines("YOGClientEvent.h", lines)

lines = readLines("YOGClientEvent.cpp")
i = findMarker(lines, "code_append_marker")
lines.insert(i, scode.replace("mname", name).replace("tname", tname))
writeLines("YOGClientEvent.cpp", lines)

