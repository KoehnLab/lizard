/*
 * This file is part of the lizard quantum chemistry software.
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file at the root of the lizard source
 * tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.
 */

grammar TensorSymmetry;

body:
	element* EOF
;

element:
	tensor_symmetry SEMICOLON
	| index_space_definition SEMICOLON
;

tensor_symmetry:
	tensor_name=ID index_spec ':' index_exchanges
;

index_spec:
	'[' (creator_spaces+=ID)* SEMICOLON (annihilator_spaces+=ID)* ']'
;

index_exchanges:
	INT COMMA INT ('&' INT COMMA INT)* '->' INT
;

index_space_definition:
	space_id=ID '=' choices+=ID ('|' choices+=ID)*
;



fragment LETTER: [a-zA-Z] ;
fragment DIGIT: [0-9] ;

ID: LETTER (LETTER | '_' | DIGIT)* ;
INT: ('+' | '-')? DIGIT+ ;
SEMICOLON: ';' ;
COMMA: ',' ;
WHITESPACE: [ \n\r\t]+ -> skip;
COMMENT: '#' ~[\n]* -> skip;
