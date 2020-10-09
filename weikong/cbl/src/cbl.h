#if !defined(CBL_H__20181221)
#define CBL_H__20181221

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <hash_map>
#include <iostream>

#include "base/base.h"
#include "serial/SerialPort.h"
#include "tinyxml2/tinyxml2.h"
#include "excel/BasicExcel.h"
#include "excel/ExcelFormat.h"
#include "usbhid/CHid.h"
#include "encrypt/aes.h"
#include "encrypt/des.h"
#include "encrypt/rc4.h"
#include "jsoncpp/json.h"

using namespace std;
using namespace cbl;
using namespace ExcelFormat;

//为了在windows上避免和msxml2的冲突，定义一些别名
typedef tinyxml2::XMLDocument		tXMLDocument;
typedef tinyxml2::XMLElement		tXMLElement;
typedef tinyxml2::XMLAttribute		tXMLAttribute;
typedef tinyxml2::XMLComment		tXMLComment;
typedef tinyxml2::XMLText			tXMLText;
typedef tinyxml2::XMLDeclaration	tXMLDeclaration;
typedef tinyxml2::XMLUnknown		tXMLUnknown;
typedef tinyxml2::XMLPrinter		tXMLPrinter;

#endif //CBL_H__20181221
