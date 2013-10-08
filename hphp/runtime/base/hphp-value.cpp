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

namespace HPHP {

//////////////////////////////////////////////////////////////////////

std::string TypedValue::pretty() const {
  char buf[20];
  sprintf(buf, "0x%lx", long(m_data.num));
  return Trace::prettyNode(tname(m_type).c_str(), std::string(buf));
}

void TypedValue::toJson(JSON::DocTarget::OutputStream& out) const {
  JSON::DocTarget::MapStream obj(out);

  obj.add("type", tname(m_type));

  std::ostringstream ss;
  ss << m_data.num;
  obj.add("value", ss.str());

  obj.done();
}

//////////////////////////////////////////////////////////////////////

}
