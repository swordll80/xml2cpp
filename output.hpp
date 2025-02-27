#pragma once
#include <string>
#include <vector>
#include "pugixml.hpp"

namespace ExampleNS {
class Example;
class node1;
class node2;
class node3;

class node1 {
public:
    std::string att1;
    std::string att2;

    node1() {
    }
    ~node1() {
    }
    node1(const node1& other) {
        att1 = other.att1;
        att2 = other.att2;
    }
    node1& operator=(const node1& other) {
        if (this != &other) {
            att1 = other.att1;
            att2 = other.att2;
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_string();
        att2 = node.attribute("att2").as_string();
    }
    void write(pugi::xml_node& node) const {
        node.append_attribute("att1") = att1.c_str();
        node.append_attribute("att2") = att2.c_str();
    }
};

class node3 {
public:
    std::string att1;
    std::string att2;
    std::string text;

    node3() {
    }
    ~node3() {
    }
    node3(const node3& other) {
        att1 = other.att1;
        att2 = other.att2;
        text = other.text;
    }
    node3& operator=(const node3& other) {
        if (this != &other) {
            att1 = other.att1;
            att2 = other.att2;
            text = other.text;
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_string();
        att2 = node.attribute("att2").as_string();
        text = node.text().as_string();
    }
    void write(pugi::xml_node& node) const {
        node.append_attribute("att1") = att1.c_str();
        node.append_attribute("att2") = att2.c_str();
        node.text().set(text.c_str());
    }
};

class node2 {
public:
    std::string att1;
    std::vector<node3*> node3_vec;

    node2() {
    }
    ~node2() {
        for (auto p : node3_vec) delete p;
    }
    node2(const node2& other) {
        att1 = other.att1;
        for (auto p : other.node3_vec) {
            auto obj = new node3(*p);
            if (!obj) throw std::bad_alloc();
            node3_vec.push_back(obj);
        }
    }
    node2& operator=(const node2& other) {
        if (this != &other) {
            att1 = other.att1;
            for (auto p : node3_vec) delete p;
            node3_vec.clear();
            for (auto p : other.node3_vec) {
                auto obj = new node3(*p);
                if (!obj) throw std::bad_alloc();
                node3_vec.push_back(obj);
            }
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_string();
        for (auto child : node.children("node3")) {
            auto obj = new node3();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node3_vec.push_back(obj);
        }
    }
    void write(pugi::xml_node& node) const {
        node.append_attribute("att1") = att1.c_str();
        for (auto p : node3_vec) {
            pugi::xml_node child_node = node.append_child("node3");
            p->write(child_node);
        }
    }
};

class Example {
public:
    node1 node1;
    std::vector<node2*> node2_vec;

    Example() {
    }
    ~Example() {
        for (auto p : node2_vec) delete p;
    }
    Example(const Example& other) {
        node1 = other.node1;
        for (auto p : other.node2_vec) {
            auto obj = new node2(*p);
            if (!obj) throw std::bad_alloc();
            node2_vec.push_back(obj);
        }
    }
    Example& operator=(const Example& other) {
        if (this != &other) {
            node1 = other.node1;
            for (auto p : node2_vec) delete p;
            node2_vec.clear();
            for (auto p : other.node2_vec) {
                auto obj = new node2(*p);
                if (!obj) throw std::bad_alloc();
                node2_vec.push_back(obj);
            }
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        node1.read(node.child("node1"));
        for (auto child : node.children("node2")) {
            auto obj = new node2();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node2_vec.push_back(obj);
        }
    }
    void write(pugi::xml_node& node) const {
        pugi::xml_node child_node = node.append_child("node1");
        node1.write(child_node);
        for (auto p : node2_vec) {
            pugi::xml_node child_node = node.append_child("node2");
            p->write(child_node);
        }
    }
};

class ExampleFile {
public:
    Example root;

    bool load(const std::string& filename) {
        pugi::xml_document doc;
        if (!doc.load_file(filename.c_str())) return false;
        root.read(doc.child("Example"));
        return true;
    }

    bool save(const std::string& filename) {
        pugi::xml_document doc;
        pugi::xml_node decl_node = doc.append_child(pugi::node_declaration);
        decl_node.append_attribute("version") = "1.0";
        decl_node.append_attribute("encoding") = "UTF-8";
        pugi::xml_node root_node = doc.append_child("Example");
        root.write(root_node);
        return doc.save_file(filename.c_str());
    }
};}  // namespace ExampleNS
