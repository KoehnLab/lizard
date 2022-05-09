/*
 * This file is part of the lizard quantum chemistry software.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file at the root of the lizard source
 * tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.
 */

grammar GeCCoExport;

body:
	contraction* EOF
;

contraction:
	contr_num NL
	result NL
	factor NL
	num_vertices NL
	super_vertex NL
	num_arcs NL
	vertices NL
	arcs NL
	external_arcs NL
	contr_string NL
	result_string NL?
;

contr_num:
	'[CONTR] #' contraction_index=INT
;

result:
	'/RESULT/' NL tensor_spec
;

factor:
	'/FACTOR/' external_factor=DOUBLE sign=INT contraction_factor=DOUBLE
;

num_vertices:
	'/#VERTICES/' vertex_count=INT operatator_count=INT
;

super_vertex:
	'/SVERTEX/' (operator_indices+=INT)+
;

num_arcs:
	'/#ARCS/' arc_count=INT external_arc_count=INT
;

vertices:
	'/VERTICES/' (NL tensor_spec)+
;

arcs:
	'/ARCS/' arc_spec
;

external_arcs:
	'/XARCS/' arc_spec
;

contr_string:
	'/CONTR_STRING/' NL
	(vertex_indices+=INT)* NL
	(index_types+=INT)* NL
	(index_spaces+=INT)* NL
	(result_flags+=ID)* NL
	(arc_index+=INT)* NL
	(index_ids+=INT)*
;

result_string:
	'/RESULT_STRING/' NL
	(vertex_indices+=INT)* NL
	(index_types+=INT)* NL
	(index_spaces+=INT)* NL
	(arc_index+=INT)* NL
	(index_ids+=INT)*
;

tensor_spec:
	name=ID transpose=ID index_spec
;

index_spec:
	'[' ID? COMMA ID? ( ';' ID? COMMA ID? )* ']'
;

arc_spec:
	(NL INT INT index_spec)*
;



fragment LETTER: [a-zA-Z] ;
fragment DIGIT: [0-9] ;

INT: ('+'|'-')? DIGIT+ ;
DOUBLE: ('+'|'-')? DIGIT* '.' DIGIT+ ;
COMMA: ',' ;
ID: (LETTER | '_' | DIGIT)+ ;
NL: '\n' ;
WHITESPACE: [ \t\r]+ -> skip ;
