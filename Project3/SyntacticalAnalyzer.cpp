/*******************************************************************************
* File name: SyntacticalAnalyzer.cpp                                                               *
* Project: CS 460 Project 2/3 Fall 2019                                          *
* Author(s): Andrew Langwell, Chris Tran, Lizzy Axworthy, Christopher DeGuzman                                                                 *
* Date: Dec 13, 2019                                                                 *
* Description: This file contains                                              *
*******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <set>
#include "SyntacticalAnalyzer.h"

using namespace std;

//NOTES: Have to check if token == EOF_T if calling multiple functions
//sequentially like in program for example. Checking for EOF should
//happen in each function so that the right error message is displayed.

SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
	string out_file = filename;
	out_file.erase(out_file.length()-2);
	out_file += "p2";
	p2_file.open(out_file);
	lex = new LexicalAnalyzer (filename);
	token_type t;

	cgen = new CodeGen (filename);

	int totalErrors = program();
}

SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
	delete lex;
	delete cgen;
}

int SyntacticalAnalyzer::program ()
{
	cout << "in program" << endl;

	int errors = 0;
	set<token_type> firsts = {LPAREN_T};
  	set<token_type>follows={};

	token = lex->GetToken();
	while(token_type(token) != LPAREN_T){
		if(token_type(token)== EOF_T){
				errors++;
				lex->ReportError("EOF reached before first LPAREN_T in program().\n");
				return errors;
		}
		errors++;
		lex->ReportError("LPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}

	p2_file << "Using rule 1.\n";
	token = lex->GetToken();
	while(token_type(token) != DEFINE_T){
		if(token_type(token) == EOF_T){
				errors++;
				lex->ReportError("EOF reached before first DEFINE_T in program().\n");
				return errors;
		}
		errors++;
		lex->ReportError("DEFINE_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}

  errors += define();
  token = lex->GetToken();
	while(token_type(token) != LPAREN_T){
		if(token_type(token)== EOF_T){
				errors++;
				lex->ReportError("EOF reached before second LPAREN_T in program().\n");
				return errors;
		}
		errors++;
		lex->ReportError("LPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}

  errors+=more_defines();

  if(token_type(token) != EOF_T){
    token = lex->GetToken();
		while(token_type(token) != EOF_T){
  		lex->ReportError("End of File Expected. "+lex->GetTokenName(token)+" found instead.\n");
			token = lex->GetToken();
			errors++;
		}
  }
	return errors;
}

//Token not advanced into next nonterminal after this function. Token will
//be RPAREN_T is everything went well, otherwise will be EOF_T.
int SyntacticalAnalyzer::more_defines()
{
	cout << "in more_defines" << endl;


	int errors = 0;
	set<token_type> firsts = {IDENT_T, DEFINE_T};
  	set<token_type>follows={EOF_T};

	token = lex->GetToken();
	while(firsts.find(token_type(token)) == firsts.end()){
	    errors++;
	    token = lex->GetToken();
		if(token_type(token) == EOF_T){
			lex->ReportError("EOF_T reached before IDENT_T or DEFINE_T found in more_defines().\n");
			return errors;
		}
  	}

  if(token_type(token)==DEFINE_T){
	    p2_file<<"Using rule 2.\n";
	    errors += define();
		token = lex->GetToken();
		while(token_type(token) != LPAREN_T){
			if(token_type(token)== EOF_T){
					errors++;
					lex->ReportError("EOF reached before LPAREN_T in more_defines().\n");
					return errors;
			}
			errors++;
			lex->ReportError("LPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
			token = lex->GetToken();
		}
	    errors += more_defines();
	}

  else if(token_type(token)==IDENT_T){
    p2_file<<"Using rule 3.\n";
		errors += stmt_list();

		while(token_type(token) != RPAREN_T){
			if(token_type(token)== EOF_T){
					errors++;
					lex->ReportError("EOF reached before RPAREN_T.\n");
					return errors;
			}
			errors++;
			lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
			token = lex->GetToken();
		}
  }

	return errors;
}

//Token not advanced into next nonterminal after this function. Token will
//be RPAREN_T is everything went well, otherwise will be EOF_T.
//Function only called if a DEFINE_T is found
int SyntacticalAnalyzer::define ()
{
	cout << "in define" << endl;

	p2_file<<"Using rule 4.\n";
	set<token_type> firsts = {DEFINE_T};
  	set<token_type>follows={LPAREN_T};
	int errors = 0;

	//Look for first LPAREN_T
	token = lex->GetToken();
	while(token_type(token) != LPAREN_T){
		if(token_type(token)== EOF_T){
				errors++;
				lex->ReportError("EOF reached before LPAREN_T in define().\n");
				return errors;
		}
		errors++;
		lex->ReportError("LPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}

	token = lex->GetToken();

	//Look for identifier
	while(token_type(token) != IDENT_T){
		if(token_type(token)== EOF_T){
				errors++;
				lex->ReportError("EOF reached before IDENT_T in define().\n");
				return errors;
		}
		errors++;
		lex->ReportError("IDENT_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}
	bool isMain = 0;
	if(lex->GetLexeme() == "main"){
		cgen->WriteCode(0, "int main (");
		isMain = 1;
	}else{	
		cgen->WriteCode(0,"Object ");
		cgen->WriteCode(0, lex->GetLexeme());
		cgen->WriteCode(0, " (");
	}
	//param_list needs to start with GetToken() so the same IDENT_T isn't counted
	//twice
	errors += param_list();

	//see if param_list already found eof. If not, start looking for RPAREN_T.
	if(token_type(token) != EOF_T){
		while(token_type(token) != RPAREN_T){
			if(token_type(token)== EOF_T){
				errors++;
				lex->ReportError("EOF reached before RPAREN_T in define().\n");
				return errors;
			}
			errors++;
			lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
			token = lex->GetToken();
		}
	}
	cgen->WriteCode(0, "\n{\nObject __RetVal;\n");


	errors += stmt();

	if(token_type(token) != EOF_T)
		errors += stmt_list();
	//Technically, since stmt_list can be empty, this is the actual error
	else{
		lex->ReportError("EOF reached before ending RPAREN_T in define().\n");
	}

	//in a good program, stmt_list will have return RPAREN_T
	while(token_type(token) != RPAREN_T){
		cout << token;
		if(token_type(token)== EOF_T){
			errors++;
			cout << "error in define EOF reached";
			lex->ReportError("EOF reached before ending RPAREN_T in define().\n");
			return errors;
		}
		errors++;
		lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
		token = lex->GetToken();
	}
	cout << isMain;
	if(isMain){
		cgen->WriteCode(0, "return 0;\n}\n");
	}else{
		cgen->WriteCode(0, "\n return __RetVal;\n}\n");
	}
	return errors;
}

//After returning from this function, token will have been advanced. This is
//necessary because of the inevitability of an empty list to end the chain
int SyntacticalAnalyzer::stmt_list ()
{
	cout << "in stmt_list" << endl;

	int errors = 0;
	set<token_type> firsts = {/*RPAREN_T,*/  IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};
	token=lex->GetToken();

	//if it's not the start of a statement, stmt_list is empty.
	if(firsts.find(token_type(token)) == firsts.end()){
		p2_file<<"Using rule 6.\n";
		return errors;
	}

	//because we already grabbed the token and can't put it back,
	//we have to do the work of stmt here. Otherwise calling GetToken in the
	//beginning of stmt will cause us to lose a token.
	else{
		p2_file<<"Using rule 5.\n";


		if(token_type(token) == IDENT_T){
			p2_file<<"Using rule 8.\n";
		}

		else if(token_type(token) == LPAREN_T){
			p2_file<<"Using rule 9.\n";
			errors += action();
			while(token_type(token) != RPAREN_T){
				if (token_type(token) == EOF_T){
						errors++;
						lex->ReportError("EOF reached before RPAREN_T found in stmt_list().\n");
						return errors;
				}
				errors++;
				lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
				token = lex->GetToken();
			}
		}

		else if(token_type(token) == NUMLIT_T || token_type(token) == STRLIT_T || token_type(token) == SQUOTE_T){
			cgen->WriteCode(0, "__RetVal = ");
			p2_file<<"Using rule 7.\n";
			bool isSQUOTE = false;
			if (token_type(token) == SQUOTE_T)
				isSQUOTE = true;
			//if (isSQUOTE)
				//cgen->WriteCode(0, "\"");
			errors += literal();
			//if (isSQUOTE)
				//cgen->WriteCode(0, "\"");
			cgen->WriteCode(0, ";\n");
			isSQUOTE = false;
		}
		errors+=stmt_list();
	}

	return errors;
}



//Token not advanced into next nonterminal after this function
int SyntacticalAnalyzer::stmt ()
{
	cout << "in stmt" << endl;

	int errors = 0;
	token = lex->GetToken();
	set<token_type> firsts = {IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};
  	set<token_type>follows={LPAREN_T};
	while(firsts.find(token_type(token)) == firsts.end()){
		if(token_type(token) == EOF_T){
			errors++;
			lex->ReportError("EOF reached before IDENT_T, LPAREN_T, or literal found in stmt().\n");
			return errors;
		}
		errors++;
		token = lex->GetToken();
	}
  if(token_type(token) == IDENT_T){
  		cgen->WriteCode(0, lex->GetLexeme());
		p2_file<<"Using rule 8.\n";
	}

	else if(token_type(token) == LPAREN_T){
		p2_file<<"Using rule 9.\n";
		errors += action();
		while(token_type(token) != RPAREN_T){
			if (token_type(token) == EOF_T){
					errors++;
					lex->ReportError("EOF reached before RPAREN_T found in stmt().\n");
					return errors;
			}
			else{
				errors++;
				lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
				token = lex->GetToken();
			}
		}
	}
	else if(token_type(token) == NUMLIT_T || token_type(token) == STRLIT_T || token_type(token) == SQUOTE_T){
		//cgen->WriteCode(0, "__RetVal = ");
		p2_file<<"Using rule 7.\n";
		bool isSQUOTE = false;
		if (token_type(token) == SQUOTE_T)
			isSQUOTE = true;
		//if (isSQUOTE)
			//cgen->WriteCode(0, "\"");
		errors += literal();
		//if (isSQUOTE)
			//cgen->WriteCode(0, "\"");
		cgen->WriteCode(0, ";\n");
		isSQUOTE = false;
	}

	return errors;
}

//Token not advanced into next nonterminal after this function (unless quoted_lit does it)
//This function is only called if we have the right token, so we don't need to
//check it's value or get new tokens.
int SyntacticalAnalyzer::literal ()
{
	cout << "in literal" << endl;

	//special case where we don't grab a new token at the beginning.
	int errors = 0;
	set<token_type> firsts = {NUMLIT_T, STRLIT_T,  SQUOTE_T};
	set<token_type>follows={RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T, LAMBDA};
	if(token_type(token) == EOF_T){
		errors++;
		return errors;
	}

	if(token_type(token) == NUMLIT_T){
		cgen->WriteCode(0,"Object(" + lex->GetLexeme() +")");
		p2_file<<"Using rule 10.\n";
	}
	else if(token_type(token) == STRLIT_T){
		cgen->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		p2_file<<"Using rule 11.\n";
	}
	else{
		cgen->WriteCode(0, "Object(\"");
		p2_file<<"Using rule 12.\n";
		errors += quoted_lit();
		cgen->WriteCode(0, "\")");
	}

	return errors;
}

int SyntacticalAnalyzer::quoted_lit ()
{
	cout << "in quoted_lit" << endl;

	int errors = 0;

	set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T,
				  LISTOP2_T, IF_T, DISPLAY_T, NEWLINE_T,
				  LISTOP1_T, AND_T, OR_T, NOT_T,
				  DEFINE_T, NUMBERP_T, LISTP_T,
				  ZEROP_T, NULLP_T, STRINGP_T, PLUS_T,
				  MINUS_T, DIV_T, MULT_T, MODULO_T,
				  ROUND_T, EQUALTO_T, GT_T, LT_T,
				  GTE_T, LTE_T, SQUOTE_T, COND_T,
				  ELSE_T};
	set<token_type> follows = {RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};

	while (firsts.find(token) == firsts.end()){
	    errors++;
	    token = lex->GetToken();
	}
	p2_file<<"Using rule 13.\n";
	errors += any_other_token();

	return errors;
}

int SyntacticalAnalyzer::more_tokens ()
{
	cout << "in more_tokens" << endl;

	int errors = 0;

	set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, LISTOP2_T, IF_T, DISPLAY_T, NEWLINE_T, LISTOP1_T, AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, SQUOTE_T, COND_T, ELSE_T, RPAREN_T, LPAREN_T};
	set<token_type> follows = {RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T,
				   STRLIT_T, SQUOTE_T};

	while (firsts.find(token_type(token)) == firsts.end()){
        errors++;
        if (token_type(token) == EOF_T)
        	return errors;
	cout << "t_t(): " << token_type(token) << "token: " << token << endl;

        token = lex->GetToken();
    }
	if (token_type(token) == RPAREN_T)
	{
	    p2_file<<"Using rule 15.\n";
	    //token = lex->GetToken();
	}else{
	    p2_file<<"Using rule 14.\n";
		cgen->WriteCode(0, " ");
	    errors += any_other_token();

	    errors += more_tokens();
	}
	return errors;
}

int SyntacticalAnalyzer::param_list ()
{
	cout << "in param_list" << endl;

	int errors = 0;
	set<token_type> firsts = {IDENT_T, RPAREN_T};
  	set<token_type> follows = {RPAREN_T};
	token = lex->GetToken();
	if(token_type(token) == EOF_T){
		errors++;
		return errors;
	}

	if (token_type(token) == IDENT_T)
	{
	    p2_file<<"Using rule 16.\n";
	    cgen->WriteCode(0, lex->GetLexeme());
			errors += param_list();
	}
	else
	{
		cgen->WriteCode(0, ")");
	    p2_file<<"Using rule 17.\n";
	}

	return errors;
}

int SyntacticalAnalyzer::else_part ()
{
	cout << "in else_part" << endl;

	int errors = 0;
	token = lex->GetToken();
	set<token_type> firsts = {IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T, RPAREN_T};
	set<token_type> follows = {RPAREN_T};
	if(token_type(token) == EOF_T){
		errors++;
		return errors;
	}

	if(token_type(token) == IDENT_T || token_type(token) == LPAREN_T || token_type(token) == NUMLIT_T || token_type(token) == STRLIT_T || token_type(token) == SQUOTE_T){
		p2_file << "Using rule 18.\n";
		// don't want to get next token because token is already at the first of stmt token = lex->GetToken();
		if(token_type(token) == IDENT_T){
			// this cgen messes up test case 3
			//cgen->WriteCode(0, "__RetVal = ");
			cgen->WriteCode(0, lex->GetLexeme());
			cgen->WriteCode(0, ";");
			p2_file<<"Using rule 8.\n";
		}
		else if(token_type(token) == LPAREN_T){
			p2_file<<"Using rule 9.\n";
			errors += action();
			while(token_type(token) != RPAREN_T){
				if(token_type(token) == EOF_T){
					errors++;
					lex->ReportError("EOF found before RPAREN_T in else_part().\n");
					return errors;
				}
				errors++;
				lex->ReportError("RPAREN_T Expected. "+lex->GetTokenName(token)+" found instead.\n");
				token = lex->GetToken();
			}
		}
		else if(token_type(token) == NUMLIT_T || token_type(token) == STRLIT_T || token_type(token) == SQUOTE_T){
			cgen->WriteCode(0, "__RetVal = ");
			p2_file<<"Using rule 7.\n";
			bool isSQUOTE = false;
			if (token_type(token) == SQUOTE_T)
				isSQUOTE = true;
			//if (isSQUOTE)
				//cgen->WriteCode(0, "\"");
			errors += literal();
			//if (isSQUOTE)
				//cgen->WriteCode(0, "\"");
			cgen->WriteCode(0, ";\n");
			isSQUOTE = false;
		}
		errors+=stmt_list();

	}

	else{
		p2_file << "Using rule 19.\n";
	}

	return errors;
}

int SyntacticalAnalyzer::stmt_pair ()
{
	cout << "stmt_pair" << endl;

	int errors = 0;
	set<token_type> firsts = {LPAREN_T, RPAREN_T};
	set<token_type> follows = {RPAREN_T};

	while(firsts.find(token_type(token)) == firsts.end()){
	    errors++;
	    token = lex->GetToken();
		if(token_type(token) == EOF_T){
			lex->ReportError("EOF_T reached before LPAREN_T or RPAREN_T found in stmt_pair().\n");
			return errors;
		}
	}

	if(token_type(token) == LPAREN_T){
		p2_file << "Using rule 20.\n";
		token = lex->GetToken();
		errors += stmt_pair_body();
	}else if(token_type(token) == RPAREN_T){
		p2_file << "Using rule 21.\n";
		token = lex->GetToken();
	}else{
		errors++;
		lex->ReportError("Unexpected token found");
	}

	return errors;
}

int SyntacticalAnalyzer::stmt_pair_body ()
{
	cout << "in stmt_pair_body" << endl;

	int errors = 0;
	set<token_type> firsts = {ELSE_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};
	set<token_type> follows = {RPAREN_T};

	while(firsts.find(token_type(token)) == firsts.end()){
    errors++;
    token = lex->GetToken();
		if(token_type(token) == EOF_T){
			lex->ReportError("EOF_T reached before a valid first in stmt_pair_body().\n");
			return errors;
		}
	}

	if(token_type(token) == ELSE_T){
		p2_file << "Using rule 23.\n";
		token = lex->GetToken();
		errors += stmt();
		if(token_type(token) == RPAREN_T){
			token = lex->GetToken();
		}else{
			errors++;
			lex->ReportError("Expected RPAREN_T");
		}
	}else if (token_type(token) == IDENT_T || token_type(token) == LPAREN_T || token_type(token) == NUMLIT_T || token_type(token) == STRLIT_T || token_type(token) == SQUOTE_T){
		p2_file << "Using rule 22.\n";
		// dont get next token token is already first of stmt token = lex->GetToken();
		errors += stmt();
		errors += stmt();
		if(token_type(token) == RPAREN_T){
			token = lex->GetToken();
		}else{
			errors++;
			lex->ReportError("Expected RPAREN_T");
		}
		errors += stmt_pair();
	}
	return errors;
}

int SyntacticalAnalyzer::action ()
{
	cout << "in action" << endl;

	int errors = 0;
	token = lex->GetToken();
	set<token_type> firsts = {IF_T, COND_T, LISTOP1_T, LISTOP2_T,
		AND_T, OR_T, NOT_T, NUMBERP_T,
		LISTP_T, ZEROP_T, NULLP_T,
		STRINGP_T, PLUS_T, MINUS_T, DIV_T,
		MULT_T, MODULO_T, ROUND_T,
		EQUALTO_T, GT_T, LT_T, GTE_T,
		LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T};
		set<token_type> follows = {RPAREN_T};

		while (firsts.find(token) == firsts.end()){
			if(token_type(token) == EOF_T){
				errors++;
				lex->ReportError("EOF reached before a valid action was found.\n");
			}
			errors++;
			token = lex->GetToken();
		}

		if (token_type(token) == IF_T)
		{
			p2_file<<"Using rule 24.\n";
			errors += stmt();
			errors += stmt();
			errors += else_part();
		}

		else if (token_type(token) == COND_T)
		{
			p2_file<<"Using rule 25.\n";
			token = lex->GetToken();
			while(token_type(token) != LPAREN_T){
				if(token_type(token) == EOF_T){
					errors++;
					lex->ReportError("EOF reached before LPAREN_T in cond statement.\n");
				}
				errors++;
				lex->ReportError("LPAREN_T expected. " + lex->GetTokenName(token) + " found instead.\n");
				token=lex->GetToken();
			}
			errors += stmt_pair_body();
		}

		else if (token_type(token) == LISTOP1_T)
		{
			cout << lex->GetLexeme();
			p2_file<<"Using rule 26.\n";
			cgen->WriteCode(0, "listop (");
			cgen->WriteCode(0, "\"");
			cgen->WriteCode(0, lex->GetLexeme());
			cgen->WriteCode(0, "\", ");
			errors+= stmt();
			cgen->WriteCode(0, ")");
		}

		else if (token_type(token) == LISTOP2_T)
		{
			cgen->WriteCode(0, "__RetVal = cons (");
			p2_file<<"Using rule 27.\n";
			errors+= stmt();
			cgen->WriteCode(0, ",\n");
			cgen->WriteCode(1, "");
			errors+= stmt();
			cgen->WriteCode(0, ")");
		}

		else if (token_type(token) == AND_T)
		{
			p2_file<<"Using rule 28.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == OR_T)
		{
			p2_file<<"Using rule 29.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == NOT_T)
		{
			p2_file<<"Using rule 30.\n";
			errors+= stmt();
		}

		else if (token_type(token) == NUMBERP_T)
		{
			p2_file<<"Using rule 31.\n";
			errors+= stmt();
		}

		else if (token_type(token) == LISTP_T)
		{
			p2_file<<"Using rule 32.\n";
			errors+= stmt();
		}

		else if (token_type(token) == ZEROP_T)
		{
			p2_file<<"Using rule 33.\n";
			errors+= stmt();
		}

		else if (token_type(token) == NULLP_T)
		{
			p2_file<<"Using rule 34.\n";
			errors+= stmt();
		}

		else if (token_type(token) == STRINGP_T)
		{
			p2_file<<"Using rule 35.\n";
			errors+= stmt();
		}

		else if (token_type(token) == PLUS_T)
		{
			p2_file<<"Using rule 36.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == MINUS_T)
		{
			p2_file<<"Using rule 37.\n";
			errors+= stmt();
			errors+= stmt_list();
		}

		else if (token_type(token) == DIV_T)
		{
			p2_file<<"Using rule 38.\n";
			errors+= stmt();
			errors+= stmt_list();
		}

		else if (token_type(token) == MULT_T)
		{
			p2_file<<"Using rule 39.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == MODULO_T)
		{
			p2_file<<"Using rule 40.\n";
			errors+= stmt();
			errors+= stmt();
		}

		else if (token_type(token) == ROUND_T)
		{
			p2_file<<"Using rule 41.\n";
			errors+= stmt();
		}
		else if (token_type(token) == EQUALTO_T)
		{
			p2_file<<"Using rule 42.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == GT_T)
		{
			p2_file<<"Using rule 43.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == LT_T)
		{
			p2_file<<"Using rule 44.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == GTE_T){
			p2_file<<"Using rule 45.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == LTE_T){
			p2_file<<"Using rule 46.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == IDENT_T){
			p2_file<<"Using rule 47.\n";
			errors+= stmt_list();
		}

		else if (token_type(token) == DISPLAY_T){
			p2_file<<"Using rule 48.\n";
			cgen->WriteCode(0, "cout << ");
			errors+= stmt();
		}

		else if (token_type(token) == NEWLINE_T){
			cgen->WriteCode(0, "cout << endl;\n");
			p2_file<<"Using rule 49.\n";
		}

		return errors;
}

int SyntacticalAnalyzer::any_other_token ()
{
	cout << "in any_other_token" << endl;

	int errors = 0;
	set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, LISTOP2_T, IF_T, DISPLAY_T, NEWLINE_T, LISTOP1_T, AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, SQUOTE_T, COND_T, ELSE_T};
	set<token_type> follows = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, LISTOP2_T, IF_T, DISPLAY_T, NEWLINE_T, LISTOP1_T, AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, SQUOTE_T, COND_T,
	       	ELSE_T, RPAREN_T, LPAREN_T};

	while(firsts.find(token_type(token)) == firsts.end()){
		errors++;
		token = lex->GetToken();
		if(token_type(token) == EOF_T){
			lex->ReportError("EOF_T reached before valid start in any_other_token().\n");
			return errors;
		}
	}

	if(token_type(token) == LPAREN_T){
		cgen->WriteCode(0, "(");
		p2_file << "Using rule 50.\n";
		token = lex->GetToken();
		//go to more_tokens check
		errors += more_tokens();
		if(token_type(token) == RPAREN_T){
			cgen->WriteCode(0, ")");
			token = lex->GetToken();
		}else{
			errors++;
			lex->ReportError("Expected RPAREN_T");
		}

	}else if (token_type(token) == IDENT_T){
		p2_file << "Using rule 51.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();
	}else if (token_type(token) == NUMLIT_T){
		p2_file << "Using rule 52.\n";
		cout << token << "is a number";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();
	}else if (token_type(token) == STRLIT_T){
		p2_file << "Using rule 53.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == LISTOP2_T){
		p2_file << "Using rule 54.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == IF_T){
		p2_file << "Using rule 55.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == DISPLAY_T){
		p2_file << "Using rule 56.\n";
		cgen->WriteCode(0, "cout << ");
		token = lex->GetToken();

	}else if (token_type(token) == NEWLINE_T){
		p2_file << "Using rule 57.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == LISTOP1_T){
		cout << lex->GetLexeme();
		cgen->WriteCode(0, lex->GetLexeme());
		p2_file << "Using rule 58.\n";
		token = lex->GetToken();

	}else if (token_type(token) == AND_T){
		p2_file << "Using rule 59.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == OR_T){
		p2_file << "Using rule 60.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == NOT_T){
		p2_file << "Using rule 61.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == DEFINE_T){
		p2_file << "Using rule 62.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == NUMBERP_T){
		p2_file << "Using rule 63.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == LISTP_T){
		p2_file << "Using rule 64.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();
	}else if (token_type(token) == ZEROP_T){
		p2_file << "Using rule 65.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == NULLP_T){
		p2_file << "Using rule 66.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == STRINGP_T){
		p2_file << "Using rule 67.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == PLUS_T){
		p2_file << "Using rule 68.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == MINUS_T){
		p2_file << "Using rule 69.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == DIV_T){
		p2_file << "Using rule 70.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == MULT_T){
		p2_file << "Using rule 71.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == MODULO_T){
		p2_file << "Using rule 72.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == ROUND_T){
		p2_file << "Using rule 73.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == EQUALTO_T){
		p2_file << "Using rule 74.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == GT_T){
		p2_file << "Using rule 75.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == LT_T){
		p2_file << "Using rule 76.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == GTE_T){
		p2_file << "Using rule 77.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == LTE_T){
		p2_file << "Using rule 78.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();

	}else if (token_type(token) == SQUOTE_T){
		p2_file << "Using rule 79.\n";
		token = lex->GetToken();
		errors += any_other_token();
	}else if (token_type(token) == COND_T){
		p2_file << "Using rule 80.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();
	}else if (token_type(token) == ELSE_T){
		p2_file << "Using rule 81.\n";
		cgen->WriteCode(0, lex->GetLexeme());
		token = lex->GetToken();
	}else{
		errors++;
		lex->ReportError("Unexpected token found");
	}
	return errors;
}
