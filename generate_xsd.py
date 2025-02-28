import sys
import xml.etree.ElementTree as ET

class NodeInfo:
    def __init__(self, name):
        self.name = name
        self.attributes = {}  # {attr_name: {"type": "xs:string", "required": bool}}
        self.text_type = None  # 有文本内容时的类型（统一为 xs:string）
        self.has_text = False  # 是否有文本
        self.children = {}  # {child_name: ChildInfo}
        self.node_count = 2  # 默认值 2（minOccurs=0, maxOccurs=unbounded）

class ChildInfo:
    def __init__(self, name, node_count):
        self.name = name
        self.node_count = node_count

def parse_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    nodes = {}

    def process_element(element):
        node_name = element.tag
        if node_name not in nodes:
            nodes[node_name] = NodeInfo(node_name)

        node_info = nodes[node_name]

        # 解析属性（忽略 nodeCount）
        for attr, value in element.attrib.items():
            if attr == "nodeCount":
                node_info.node_count = int(value)
            else:
                # 判断属性值是否为 "0"，决定是否 required
                is_required = not value.endswith("0")
                # 如果属性已存在，且当前属性为 0，则更新0
                if attr in node_info.attributes:
                    if not is_required:
                        node_info.attributes[attr]["required"] = False
                else:
                    node_info.attributes[attr] = {"type": "xs:string", "required": is_required}

        # 解析文本内容
        if element.text and element.text.strip():
            node_info.has_text = True
            node_info.text_type = "xs:string"

        # 解析子节点
        for child in element:
            child_name = child.tag
            child_node_count = int(child.attrib.get("nodeCount", 2))  # 默认 2
            if child_name not in node_info.children:
                node_info.children[child_name] = ChildInfo(child_name, child_node_count)
            process_element(child)

    process_element(root)
    return nodes, root.tag

def generate_xsd(nodes, root_name, output_file="output.xsd"):
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        f.write('<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">\n\n')

        processed = set()
        def write_complex_type(name):
            if name in processed or name == root_name:
                return
            node_info = nodes[name]
            
            f.write(f'<xs:complexType name="tn_{name}">\n')

            if node_info.has_text and node_info.attributes:
                # 既有文本又有属性，使用 simpleContent + extension
                f.write('    <xs:simpleContent>\n')
                f.write('        <xs:extension base="xs:string">\n')
                for attr, attr_info in node_info.attributes.items():
                    if attr_info["required"]:
                        f.write(f'            <xs:attribute name="{attr}" type="xs:string" use="required"/>\n')
                    else:
                        f.write(f'            <xs:attribute name="{attr}" type="xs:string" use="optional"/>\n')
                f.write('        </xs:extension>\n')
                f.write('    </xs:simpleContent>\n')
            elif node_info.has_text:
                # 仅有文本
                f.write('    <xs:simpleContent>\n')
                f.write('        <xs:extension base="xs:string"/>\n')
                f.write('    </xs:simpleContent>\n')
            else:
                # 仅有子节点或属性
                if node_info.children:
                    f.write('    <xs:sequence>\n')
                    for child_name, child_info in node_info.children.items():
                        min_occurs = '1' if child_info.node_count == 1 else '0'
                        max_occurs = '1' if child_info.node_count == 1 else 'unbounded'
                        f.write(f'        <xs:element name="{child_name}" type="tn_{child_info.name}" minOccurs="{min_occurs}" maxOccurs="{max_occurs}"/>\n')
                    f.write('    </xs:sequence>\n')
                for attr, attr_info in node_info.attributes.items():
                    if attr_info["required"]:
                        f.write(f'    <xs:attribute name="{attr}" type="xs:string" use="required"/>\n')
                    else:
                        f.write(f'    <xs:attribute name="{attr}" type="xs:string" use="optional"/>\n')

            f.write('</xs:complexType>\n\n')
            processed.add(name)

        for name in nodes:
            write_complex_type(name)

        # 根节点直接定义
        root_node = nodes[root_name]
        f.write(f'<xs:element name="{root_name}">\n')
        f.write('    <xs:complexType>\n')

        if root_node.has_text and root_node.attributes:
            # 既有文本又有属性
            f.write('        <xs:simpleContent>\n')
            f.write('            <xs:extension base="xs:string">\n')
            for attr, attr_info in root_node.attributes.items():
                if attr_info["required"]:
                    f.write(f'                <xs:attribute name="{attr}" type="xs:string" use="required"/>\n')
                else:
                    f.write(f'                <xs:attribute name="{attr}" type="xs:string" use="optional"/>\n')
            f.write('            </xs:extension>\n')
            f.write('        </xs:simpleContent>\n')
        elif root_node.has_text:
            # 仅有文本
            f.write('        <xs:simpleContent>\n')
            f.write('            <xs:extension base="xs:string"/>\n')
            f.write('        </xs:simpleContent>\n')
        else:
            # 仅有子节点或属性
            if root_node.children:
                f.write('        <xs:sequence>\n')
                for child_name, child_info in root_node.children.items():
                    min_occurs = '1' if child_info.node_count == 1 else '0'
                    max_occurs = '1' if child_info.node_count == 1 else 'unbounded'
                    f.write(f'            <xs:element name="{child_name}" type="tn_{child_info.name}" minOccurs="{min_occurs}" maxOccurs="{max_occurs}"/>\n')
                f.write('        </xs:sequence>\n')
            for attr, attr_info in root_node.attributes.items():
                if attr_info["required"]:
                    f.write(f'        <xs:attribute name="{attr}" type="xs:string" use="required"/>\n')
                else:
                    f.write(f'        <xs:attribute name="{attr}" type="xs:string" use="optional"/>\n')

        f.write('    </xs:complexType>\n')
        f.write('</xs:element>\n\n')
        f.write('</xs:schema>')

if __name__ == "__main__":
    input_file = "input.xml"
    output_xsd = "output.xsd"
    if len(sys.argv) == 3:
        input_file = sys.argv[1]
        output_xsd = sys.argv[2]
    elif len(sys.argv) != 1:
        print("Usage: python generate_xsd.py [input.xml output.xsd]")
        print(f"Using default values: {input_file} {output_xsd}")
        sys.exit(1)

    nodes, root_name = parse_xml(input_file)
    generate_xsd(nodes, root_name, output_xsd)
    print(f"Generated {output_xsd} successfully!")