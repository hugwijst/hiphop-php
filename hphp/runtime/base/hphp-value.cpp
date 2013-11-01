/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2013 Facebook, Inc. (http://www.facebook.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/runtime/base/complex-types.h"
#include "hphp/util/trace.h"
#include "hphp/util/json.h"

#include "hphp/runtime/base/variable-serializer.h"

namespace HPHP {

//////////////////////////////////////////////////////////////////////

std::string TypedValue::pretty() const {
  char buf[20];
  sprintf(buf, "0x%lx", long(m_data.num));
  return Trace::prettyNode(tname(m_type).c_str(), std::string(buf));
}

void TypedValue::serialize(JSON::DocTarget::OutputStream& out) const {
  JSON::DocTarget::MapStream obj(out);

  obj.add("type", tname(m_type));

  switch(m_type) {
    case KindOfStaticString: // FALL THROUGH
    case KindOfString:  obj.add("value", m_data.pstr->toCPPString()); break;
    case KindOfBoolean: obj.add("value", m_data.num == 0); break;
    case KindOfInt64:   obj.add("value", (int)m_data.num); break;
    //case KindOfDouble:  obj.add("value", m_data.dbl); break;
    case KindOfArray:
      {
        VariableSerializer vs(VariableSerializer::Type::JSON);
        String str = vs.serializeValue(m_data.parr, false);
        obj.add("value", JSON::JsonString(str->toCPPString()));
        break;
      }
    default: /* don't know how to represent the type in JSON */ break;
  }

  obj.done();
}

//////////////////////////////////////////////////////////////////////

}
