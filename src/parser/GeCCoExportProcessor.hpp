// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_PARSER_GECCOEXPORTPROCESSOR_HPP_
#define LIZARD_PARSER_GECCOEXPORTPROCESSOR_HPP_

#include "lizard/parser/autogen/GeCCoExportBaseListener.h"
#include "lizard/parser/autogen/GeCCoExportParser.h"

namespace lizard::parser {

class GeCCoExportProcessor : public autogen::GeCCoExportBaseListener {
public:
	GeCCoExportProcessor() = default;
	~GeCCoExportProcessor() = default;

	virtual void exitContraction(autogen::GeCCoExportParser::ContractionContext *ctx) override;
	virtual void enterResult(autogen::GeCCoExportParser::ResultContext *ctx) override;
	virtual void enterFactor(autogen::GeCCoExportParser::FactorContext *ctx) override;
	virtual void enterSuper_vertex(autogen::GeCCoExportParser::Super_vertexContext *ctx) override;
	virtual void enterVertices(autogen::GeCCoExportParser::VerticesContext *ctx) override;

	virtual void exitResult_string(autogen::GeCCoExportParser::Result_stringContext *ctx) override;
	virtual void exitContr_string(autogen::GeCCoExportParser::Contr_stringContext *ctx) override;
};

} // namespace lizard::parser

#endif // LIZARD_PARSER_GECCOEXPORTPROCESSOR_HPP_
