#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "pugixml.hpp"

namespace ExampleNS {
class Example;
class node1;
class node2;
class node3;
class node4;
class node5;
class node6;
class node7;

class node1 {
public:
    int att1;
    float att2;
    std::string att3;

    node1() {
    }
    ~node1() {
    }
    node1(const node1& other) {
        att1 = other.att1;
        att2 = other.att2;
        att3 = other.att3;
    }
    node1& operator=(const node1& other) {
        if (this != &other) {
            att1 = other.att1;
            att2 = other.att2;
            att3 = other.att3;
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_int();
        att2 = node.attribute("att2").as_float();
        att3 = node.attribute("att3").as_string();
    }
    void write(pugi::xml_node& node) const {
        node.append_attribute("att1") = att1;
        node.append_attribute("att2") = att2;
        node.append_attribute("att3") = att3.c_str();
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
        if (!att2.empty()) {
            node.append_attribute("att2") = att2.c_str();
        }
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

class node5 {
public:
    int text;

    node5() {
    }
    ~node5() {
    }
    node5(const node5& other) {
        text = other.text;
    }
    node5& operator=(const node5& other) {
        if (this != &other) {
            text = other.text;
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        text = node.text().as_int();
    }
    void write(pugi::xml_node& node) const {
        node.text().set(text);
    }
};

class node4 {
public:
    std::string att1;
    std::vector<node5*> node5_vec;

    node4() {
    }
    ~node4() {
        for (auto p : node5_vec) delete p;
    }
    node4(const node4& other) {
        att1 = other.att1;
        for (auto p : other.node5_vec) {
            auto obj = new node5(*p);
            if (!obj) throw std::bad_alloc();
            node5_vec.push_back(obj);
        }
    }
    node4& operator=(const node4& other) {
        if (this != &other) {
            att1 = other.att1;
            for (auto p : node5_vec) delete p;
            node5_vec.clear();
            for (auto p : other.node5_vec) {
                auto obj = new node5(*p);
                if (!obj) throw std::bad_alloc();
                node5_vec.push_back(obj);
            }
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_string();
        for (auto child : node.children("node5")) {
            auto obj = new node5();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node5_vec.push_back(obj);
        }
    }
    void write(pugi::xml_node& node) const {
        if (!att1.empty()) {
            node.append_attribute("att1") = att1.c_str();
        }
        for (auto p : node5_vec) {
            pugi::xml_node child_node = node.append_child("node5");
            p->write(child_node);
        }
    }
};

class node7 {
public:
    float text;

    node7() {
    }
    ~node7() {
    }
    node7(const node7& other) {
        text = other.text;
    }
    node7& operator=(const node7& other) {
        if (this != &other) {
            text = other.text;
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        text = node.text().as_float();
    }
    void write(pugi::xml_node& node) const {
        node.text().set(text);
    }
};

class node6 {
public:
    std::string att1;
    std::vector<node7*> node7_vec;

    node6() {
    }
    ~node6() {
        for (auto p : node7_vec) delete p;
    }
    node6(const node6& other) {
        att1 = other.att1;
        for (auto p : other.node7_vec) {
            auto obj = new node7(*p);
            if (!obj) throw std::bad_alloc();
            node7_vec.push_back(obj);
        }
    }
    node6& operator=(const node6& other) {
        if (this != &other) {
            att1 = other.att1;
            for (auto p : node7_vec) delete p;
            node7_vec.clear();
            for (auto p : other.node7_vec) {
                auto obj = new node7(*p);
                if (!obj) throw std::bad_alloc();
                node7_vec.push_back(obj);
            }
        }
        return *this;
    }
    void read(const pugi::xml_node& node) {
        att1 = node.attribute("att1").as_string();
        for (auto child : node.children("node7")) {
            auto obj = new node7();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node7_vec.push_back(obj);
        }
    }
    void write(pugi::xml_node& node) const {
        node.append_attribute("att1") = att1.c_str();
        for (auto p : node7_vec) {
            pugi::xml_node child_node = node.append_child("node7");
            p->write(child_node);
        }
    }
};

class Example {
public:
    node1 node1;
    std::vector<node2*> node2_vec;
    std::vector<node4*> node4_vec;
    std::vector<node6*> node6_vec;

    Example() {
    }
    ~Example() {
        for (auto p : node2_vec) delete p;
        for (auto p : node4_vec) delete p;
        for (auto p : node6_vec) delete p;
    }
    Example(const Example& other) {
        node1 = other.node1;
        for (auto p : other.node2_vec) {
            auto obj = new node2(*p);
            if (!obj) throw std::bad_alloc();
            node2_vec.push_back(obj);
        }
        for (auto p : other.node4_vec) {
            auto obj = new node4(*p);
            if (!obj) throw std::bad_alloc();
            node4_vec.push_back(obj);
        }
        for (auto p : other.node6_vec) {
            auto obj = new node6(*p);
            if (!obj) throw std::bad_alloc();
            node6_vec.push_back(obj);
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
            for (auto p : node4_vec) delete p;
            node4_vec.clear();
            for (auto p : other.node4_vec) {
                auto obj = new node4(*p);
                if (!obj) throw std::bad_alloc();
                node4_vec.push_back(obj);
            }
            for (auto p : node6_vec) delete p;
            node6_vec.clear();
            for (auto p : other.node6_vec) {
                auto obj = new node6(*p);
                if (!obj) throw std::bad_alloc();
                node6_vec.push_back(obj);
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
        for (auto child : node.children("node4")) {
            auto obj = new node4();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node4_vec.push_back(obj);
        }
        for (auto child : node.children("node6")) {
            auto obj = new node6();
            if (!obj) throw std::bad_alloc();
            obj->read(child);
            node6_vec.push_back(obj);
        }
    }
    void write(pugi::xml_node& node) const {
        pugi::xml_node child_node = node.append_child("node1");
        node1.write(child_node);
        for (auto p : node2_vec) {
            pugi::xml_node child_node = node.append_child("node2");
            p->write(child_node);
        }
        for (auto p : node4_vec) {
            pugi::xml_node child_node = node.append_child("node4");
            p->write(child_node);
        }
        for (auto p : node6_vec) {
            pugi::xml_node child_node = node.append_child("node6");
            p->write(child_node);
        }
    }
};

class ExampleFile {
public:
    Example root;

    bool load(const std::string& filename) {
        pugi::xml_document doc;
        pugi::xml_parse_result ret = doc.load_file(filename.c_str());
        if (!ret) {
            std::cerr << "Error: Failed to load file '" << filename << "': " 
                      << ret.description() << std::endl;
            return false;
        }
        const pugi::xml_node root_node = doc.child("Example");
        root.read(root_node);
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
