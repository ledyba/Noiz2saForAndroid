#include "bulletmlparser.h"
#include "bulletmlparser-tinyxml.h"
#include "bulletmlerror.h"
#include "bulletmltree.h"

#include <string>
#include <assert.h>

BulletMLParserTinyXML::BulletMLParserTinyXML(const std::string& filename)
    : xmlFile_(filename), curNode_(0)
{
	setName(filename);
}

BulletMLParserTinyXML::~BulletMLParserTinyXML() {}

void BulletMLParserTinyXML::getTree(const tinyxml2::XMLNode* node) {
	if (node->ToComment() != 0) return;
    translateNode(node);

    const tinyxml2::XMLNode* child;
    for (child = node->FirstChild(); child; child = child->NextSibling()) {
		const tinyxml2::XMLText* text;
		if ((text = child->ToText()) != 0) {
			curNode_->setValue(text->Value());
			break;
		}

		getTree(child);
    }

    curNode_ = curNode_->getParent();
}

void BulletMLParserTinyXML::translateNode(const tinyxml2::XMLNode* node) {
    const tinyxml2::XMLElement* elem = node->ToElement();
    assert(elem != 0);

    BulletMLNode* xmlNode = addContent(elem->Value());

	if (xmlNode->getName() == BulletMLNode::bulletml) {
		const tinyxml2::XMLAttribute* attr;
		for (attr = elem->FirstAttribute(); attr; attr = attr->Next()) {
			if (attr->Value() == "horizontal") setHorizontal();
		}
	}
	else {
		MyAttributes mattr;
		const tinyxml2::XMLAttribute* attr;
		for (attr = elem->FirstAttribute(); attr; attr = attr->Next()) {
			mattr.push_back(attr->Name());
			mattr.push_back(attr->Value());
		}
		addAttribute(mattr, xmlNode);
		if (curNode_ != 0) curNode_->addChild(xmlNode);
	}
	curNode_ = xmlNode;
}

void BulletMLParserTinyXML::parseImpl(tinyxml2::XMLDocument& doc) {
	if (doc.Error()) {
		throw BulletMLError(std::string(doc.GetErrorStr1()) + ": " + doc.GetErrorStr2());
	}

    const tinyxml2::XMLNode* node;
    for (node = doc.FirstChild(); node; node = node->NextSibling()) {
		if (node->ToElement() != 0) {
			getTree(node);
			break;
		}
    }
}

void BulletMLParserTinyXML::parse() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(xmlFile_.c_str());
	parseImpl(doc);
}


