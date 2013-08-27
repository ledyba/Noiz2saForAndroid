/// BulletML のパーサ・ツリー by tinyxml
/**
 * tinyxmlによるBulletMLのパーサ
 */

#ifndef BULLETMLPARSER_TINYXML_H_
#define BULLETMLPARSER_TINYXML_H_

#include <string>

#include "bulletmlcommon.h"

#include "tinyxml/tinyxml2.h"

class BulletMLNode;

class BulletMLParserTinyXML : public BulletMLParser {
public:
    DECLSPEC BulletMLParserTinyXML(const std::string& filename);
    DECLSPEC virtual ~BulletMLParserTinyXML();

    DECLSPEC virtual void parse();

protected:
	void parseImpl(tinyxml2::XMLDocument& doc);

protected:
    void getTree(const tinyxml2::XMLNode* node);
    void translateNode(const tinyxml2::XMLNode* node);

private:
    std::string xmlFile_;
    BulletMLNode* curNode_;
};

#endif // ! BULLETMLPARSER_TINYXML_H_
