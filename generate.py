
import sys
import xml.etree.ElementTree as ET
from textwrap import dedent

class NodeInfo:
    def __init__(self, name):
        self.name = name
        self.attributes = []
        self.has_text = False
        self.children = {}  # {child_name: ChildInfo}
        self.node_count = 1

class ChildInfo:
    def __init__(self, name, node_count):
        self.name = name
        self.node_count = node_count

def parse_xml(xml_file):
    try:
        with open(xml_file, 'r', encoding='utf-8') as f:
            print("File content:")
            print(f.read())
    except Exception as e:
        print(f"Error reading file: {e}")
        raise

    tree = ET.parse(xml_file)
    root = tree.getroot()
    nodes = {}

    def process_element(element, parent_name):
        node_name = element.tag
        if node_name not in nodes:
            nodes[node_name] = NodeInfo(node_name)
        
        node_info = nodes[node_name]
        attribs = element.attrib
        node_info.attributes = [a for a in attribs if a != "nodeCount"]
        node_info.node_count = int(attribs.get("nodeCount", 1))
        node_info.has_text = element.text and element.text.strip() != ""

        for child in element:
            child_name = child.tag
            child_node_count = int(child.attrib.get("nodeCount", 1))
            if child_name not in node_info.children:
                node_info.children[child_name] = ChildInfo(child_name, child_node_count)
            process_element(child, node_name)

    process_element(root, None)
    return nodes, root.tag

def generate_class_code(node_info):
    class_name = node_info.name
    code = f"class {class_name} {{\npublic:\n"
    
    # Member variables
    for attr in node_info.attributes:
        code += f"    std::string {attr};\n"
    if node_info.has_text:
        code += "    std::string text;\n"
    for child_name, child_info in node_info.children.items():
        if child_info.node_count == 1:
            code += f"    {child_info.name} {child_name};\n"
        else:
            code += f"    std::vector<{child_info.name}*> {child_name}_vec;\n"

    # Constructor
    code += f"\n    {class_name}() {{\n"
    code += "    }\n"

    # Destructor
    code += f"    ~{class_name}() {{\n"
    for child_name, child_info in node_info.children.items():
        if child_info.node_count != 1:
            code += f"        for (auto p : {child_name}_vec) delete p;\n"
    code += "    }\n"

    # Copy constructor
    code += f"    {class_name}(const {class_name}& other) {{\n"
    for attr in node_info.attributes:
        code += f"        {attr} = other.{attr};\n"
    if node_info.has_text:
        code += "        text = other.text;\n"
    for child_name, child_info in node_info.children.items():
        if child_info.node_count == 1:
            code += f"        {child_name} = other.{child_name};\n"
        else:
            code += f"        for (auto p : other.{child_name}_vec) {{\n"
            code += f"            auto obj = new {child_info.name}(*p);\n"
            code += "            if (!obj) throw std::bad_alloc();\n"
            code += f"            {child_name}_vec.push_back(obj);\n"
            code += "        }\n"
    code += "    }\n"

    # Assignment operator
    code += f"    {class_name}& operator=(const {class_name}& other) {{\n"
    code += "        if (this != &other) {\n"
    for attr in node_info.attributes:
        code += f"            {attr} = other.{attr};\n"
    if node_info.has_text:
        code += "            text = other.text;\n"
    for child_name, child_info in node_info.children.items():
        if child_info.node_count == 1:
            code += f"            {child_name} = other.{child_name};\n"
        else:
            code += f"            for (auto p : {child_name}_vec) delete p;\n"
            code += f"            {child_name}_vec.clear();\n"
            code += f"            for (auto p : other.{child_name}_vec) {{\n"
            code += f"                auto obj = new {child_info.name}(*p);\n"
            code += "                if (!obj) throw std::bad_alloc();\n"
            code += f"                {child_name}_vec.push_back(obj);\n"
            code += "            }\n"
    code += "        }\n        return *this;\n    }\n"

    # Read function
    code += f"    void read(const pugi::xml_node& node) {{\n"
    for attr in node_info.attributes:
        code += f'        {attr} = node.attribute("{attr}").as_string();\n'
    if node_info.has_text:
        code += '        text = node.text().as_string();\n'
    for child_name, child_info in node_info.children.items():
        if child_info.node_count == 1:
            code += f'        {child_name}.read(node.child("{child_name}"));\n'
        else:
            code += f'        for (auto child : node.children("{child_name}")) {{\n'
            code += f'            auto obj = new {child_info.name}();\n'
            code += "            if (!obj) throw std::bad_alloc();\n"
            code += '            obj->read(child);\n'
            code += f'            {child_name}_vec.push_back(obj);\n'
            code += '        }\n'
    code += "    }\n"

    # Write function
    code += f"    void write(pugi::xml_node& node) const {{\n"
    for attr in node_info.attributes:
        code += f'        node.append_attribute("{attr}") = {attr}.c_str();\n'
    if node_info.has_text:
        code += '        node.text().set(text.c_str());\n'
    for child_name, child_info in node_info.children.items():
        if child_info.node_count == 1:
            code += f'        pugi::xml_node child_node = node.append_child("{child_name}");\n'
            code += f'        {child_name}.write(child_node);\n'
        else:
            code += f'        for (auto p : {child_name}_vec) {{\n'
            code += f'            pugi::xml_node child_node = node.append_child("{child_name}");\n'
            code += f'            p->write(child_node);\n'
            code += '        }\n'
    code += "    }\n};\n\n"
    return code

def generate_file_class(root_name):
    return dedent(f"""\
    class {root_name}File {{
    public:
        {root_name} root;
        
        bool load(const std::string& filename) {{
            pugi::xml_document doc;
            if (!doc.load_file(filename.c_str())) return false;
            root.read(doc.child("{root_name}"));
            return true;
        }}
        
        bool save(const std::string& filename) {{
            pugi::xml_document doc;
            pugi::xml_node decl_node = doc.append_child(pugi::node_declaration);
            decl_node.append_attribute("version") = "1.0";
            decl_node.append_attribute("encoding") = "UTF-8";
            pugi::xml_node root_node = doc.append_child("{root_name}");
            root.write(root_node);
            return doc.save_file(filename.c_str());
        }}
    }};""")

def generate_header(nodes, root_name, output_file):
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(dedent(f"""\
        #pragma once
        #include <string>
        #include <vector>
        #include "pugixml.hpp"
        
        namespace {root_name}NS {{
        """))
        
        # Generate forward declarations
        for name in nodes:
            f.write(f"class {name};\n")
        f.write("\n")
        
        # Generate class implementations
        processed = set()
        def write_class(name):
            if name in processed: return
            for child in nodes[name].children.values():
                write_class(child.name)
            f.write(generate_class_code(nodes[name]))
            processed.add(name)
        write_class(root_name)
        
        # Generate file class
        f.write(generate_file_class(root_name))
        
        f.write(dedent(f"""\
        }}  // namespace {root_name}NS
        """))

if __name__ == "__main__":
    input_file = "input.xml"
    output_file = "output.hpp"
    if len(sys.argv) == 3:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
    elif len(sys.argv) != 1:
        print("Usage: python generate.py [input.xml output.hpp]")
        print(f"Using default values: {input_file} {output_file}")
        sys.exit(1)
        
    nodes, root_name = parse_xml(input_file)
    generate_header(nodes, root_name, output_file)
    print(f"Generated {output_file} successfully!")