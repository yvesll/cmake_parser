#pragma once
#ifndef CMPARSER_CMPARSER_H
#define CMPARSER_CMPARSER_H

#endif //CMPARSER_CMPARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>

#include "cmListFileLexer.h"

struct cmListFileArgument
{
    enum Delimiter
    {
        Unquoted,
        Quoted,
        Bracket
    };
    cmListFileArgument() = default;
    cmListFileArgument(std::string v, Delimiter d, long line)
            : Value(std::move(v))
            , Delim(d)
            , Line(line)
    {
    }
    bool operator==(const cmListFileArgument& r) const
    {
        return (this->Value == r.Value) && (this->Delim == r.Delim);
    }
    bool operator!=(const cmListFileArgument& r) const { return !(*this == r); }
    std::string Value;
    Delimiter Delim = Unquoted;
    long Line = 0;
};

class cmListFileFunction
{
public:
    cmListFileFunction(std::string name, long line, long lineEnd,
                       std::vector<cmListFileArgument> args)
            : Impl{ std::make_shared<Implementation>(std::move(name), line, lineEnd,
                                                     std::move(args)) }
    {
    }

    std::string const& OriginalName() const noexcept
    {
        return this->Impl->OriginalName;
    }

    long Line() const noexcept { return this->Impl->Line; }
    long LineEnd() const noexcept { return this->Impl->LineEnd; }

    std::vector<cmListFileArgument> const& Arguments() const noexcept
    {
        return this->Impl->Arguments;
    }

private:
    struct Implementation
    {
        Implementation(std::string name, long line, long lineEnd,
                       std::vector<cmListFileArgument> args)
                : OriginalName{ std::move(name) }
                , Line{ line }
                , LineEnd{ lineEnd }
                , Arguments{ std::move(args) }
        {
        }

        std::string OriginalName;
        long Line = 0;
        long LineEnd = 0;
        std::vector<cmListFileArgument> Arguments;
    };

    std::shared_ptr<Implementation const> Impl;
};

struct cmListFileParser
{
    cmListFileParser();
    ~cmListFileParser();
    bool ParseFile(const char* filename);
    bool Parse();
    bool ParseFunction(const char* name, long line);
    bool AddArgument(cmListFileLexer_Token* token,
                     cmListFileArgument::Delimiter delim);
    std::vector<cmListFileFunction> Functions;
    const char* FileName = nullptr;
    cmListFileLexer* Lexer;
    std::string FunctionName;
    long FunctionLine;
    long FunctionLineEnd;
    std::vector<cmListFileArgument> FunctionArguments;
    enum
    {
        SeparationOkay,
        SeparationWarning,
        SeparationError
    } Separation;
};