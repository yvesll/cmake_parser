
#include "cmParser.h"

cmListFileParser::cmListFileParser() : Lexer(cmListFileLexer_New())
{
}

cmListFileParser::~cmListFileParser()
{
    cmListFileLexer_Delete(this->Lexer);
}

bool cmListFileParser::ParseFile(const char* filename)
{
    this->FileName = filename;

    // Open the file.
    cmListFileLexer_BOM bom;
    if (!cmListFileLexer_SetFileName(this->Lexer, filename, &bom)) {
        std::cerr << "cmListFileCache: error can not open file." << std::endl;
        return false;
    }

    if (bom == cmListFileLexer_BOM_Broken) {
        cmListFileLexer_SetFileName(this->Lexer, nullptr, nullptr);
        std::cerr << "Error while reading Byte-Order-Mark. "
                                 "File not seekable?" << std::endl;
        return false;
    }

    // Verify the Byte-Order-Mark, if any.
    if (bom != cmListFileLexer_BOM_None && bom != cmListFileLexer_BOM_UTF8) {
        cmListFileLexer_SetFileName(this->Lexer, nullptr, nullptr);
        std::cerr <<
                  "File starts with a Byte-Order-Mark that is not UTF-8." << std::endl;
        return false;
    }

    return this->Parse();
}

bool cmListFileParser::Parse()
{
    // Use a simple recursive-descent parser to process the token
    // stream.
    bool haveNewline = true;
    while (cmListFileLexer_Token* token = cmListFileLexer_Scan(this->Lexer)) {
        if (token->type == cmListFileLexer_Token_Space) {
        } else if (token->type == cmListFileLexer_Token_Newline) {
            haveNewline = true;
        } else if (token->type == cmListFileLexer_Token_CommentBracket) {
            haveNewline = false;
        } else if (token->type == cmListFileLexer_Token_Identifier) {
            if (haveNewline) {
                haveNewline = false;
                if (this->ParseFunction(token->text, token->line)) {
                    this->Functions.emplace_back(
                            std::move(this->FunctionName), this->FunctionLine,
                            this->FunctionLineEnd, std::move(this->FunctionArguments));
                } else {
                    return false;
                }
            } else {
                std::ostringstream error;
                error << "Parse error.  Expected a newline, got "
                      << cmListFileLexer_GetTypeAsString(this->Lexer, token->type)
                      << " with text \"" << token->text << "\".";
                std::cerr <<  error.str() << std::endl;
                return false;
            }
        } else {
            std::ostringstream error;
            error << "Parse error.  Expected a command name, got "
                  << cmListFileLexer_GetTypeAsString(this->Lexer, token->type)
                  << " with text \"" << token->text << "\".";
            std::cerr <<  error.str() << std::endl;
            return false;
        }
    }

    return true;
}

bool cmListFileParser::ParseFunction(const char* name, long line)
{
    // Ininitialize a new function call.
    this->FunctionName = name;
    this->FunctionLine = line;

    // Command name has already been parsed.  Read the left paren.
    cmListFileLexer_Token* token;
    while ((token = cmListFileLexer_Scan(this->Lexer)) &&
           token->type == cmListFileLexer_Token_Space) {
    }
    if (!token) {
        std::ostringstream error;
        /* clang-format off */
        error << "Unexpected end of file.\n"
              << "Parse error.  Function missing opening \"(\".";
        /* clang-format on */
        std::cerr << error.str() << std::endl;
        return false;
    }
    if (token->type != cmListFileLexer_Token_ParenLeft) {
        std::ostringstream error;
        error << "Parse error.  Expected \"(\", got "
              << cmListFileLexer_GetTypeAsString(this->Lexer, token->type)
              << " with text \"" << token->text << "\".";
        std::cerr << error.str() << std::endl;
        return false;
    }

    // Arguments.
    unsigned long parenDepth = 0;
    this->Separation = SeparationOkay;
    while ((token = cmListFileLexer_Scan(this->Lexer))) {
        if (token->type == cmListFileLexer_Token_Space ||
            token->type == cmListFileLexer_Token_Newline) {
            this->Separation = SeparationOkay;
            continue;
        }
        if (token->type == cmListFileLexer_Token_ParenLeft) {
            parenDepth++;
            this->Separation = SeparationOkay;
            if (!this->AddArgument(token, cmListFileArgument::Unquoted)) {
                return false;
            }
        } else if (token->type == cmListFileLexer_Token_ParenRight) {
            if (parenDepth == 0) {
                this->FunctionLineEnd = token->line;
                return true;
            }
            parenDepth--;
            this->Separation = SeparationOkay;
            if (!this->AddArgument(token, cmListFileArgument::Unquoted)) {
                return false;
            }
            this->Separation = SeparationWarning;
        } else if (token->type == cmListFileLexer_Token_Identifier ||
                   token->type == cmListFileLexer_Token_ArgumentUnquoted) {
            if (!this->AddArgument(token, cmListFileArgument::Unquoted)) {
                return false;
            }
            this->Separation = SeparationWarning;
        } else if (token->type == cmListFileLexer_Token_ArgumentQuoted) {
            if (!this->AddArgument(token, cmListFileArgument::Quoted)) {
                return false;
            }
            this->Separation = SeparationWarning;
        } else if (token->type == cmListFileLexer_Token_ArgumentBracket) {
            if (!this->AddArgument(token, cmListFileArgument::Bracket)) {
                return false;
            }
            this->Separation = SeparationError;
        } else if (token->type == cmListFileLexer_Token_CommentBracket) {
            this->Separation = SeparationError;
        } else {
            // Error.
            std::ostringstream error;
            error << "Parse error.  Function missing ending \")\".  "
                  << "Instead found "
                  << cmListFileLexer_GetTypeAsString(this->Lexer, token->type)
                  << " with text \"" << token->text << "\".";
            std::cerr << error.str() << std::endl;
            return false;
        }
    }

    std::ostringstream error;
    error << "Parse error.  Function missing ending \")\".  "
          << "End of file reached.";
    std::cerr << error.str() << std::endl;
    return false;
}

bool cmListFileParser::AddArgument(cmListFileLexer_Token* token,
                                   cmListFileArgument::Delimiter delim)
{
    this->FunctionArguments.emplace_back(token->text, delim, token->line);
    if (this->Separation == SeparationOkay) {
        return true;
    }
    bool isError = (this->Separation == SeparationError ||
                    delim == cmListFileArgument::Bracket);
    std::ostringstream m;

    m << "Syntax " << (isError ? "Error" : "Warning") << " in cmake code at "
      << "column " << token->column << "\n"
      << "Argument not separated from preceding token by whitespace.";
    /* clang-format on */
    if (isError) {
        std::cerr << m.str() << std::endl;
        return false;
    }
    return true;
}

