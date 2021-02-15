// Basic structure for factory supporting self-registering classes:
// https://www.codeproject.com/articles/567242/aplusc-2b-2bplusobjectplusfactory
// https://www.bfilipek.com/2018/02/factory-selfregister.html#self-registration
// Why static s_registered variable is a problem when linking static libraries:
// https://www.bfilipek.com/2018/02/static-vars-static-lib.html
// How to use -Wl, and why I use -force_load instead
// https://github.com/premake/premake-core/issues/1170


#ifndef FACTORY_H
#define FACTORY_H

#include <map>
#include <memory>

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class AnalysisFactory{
public:
    using TCreateMethod = std::unique_ptr<Analysis>(*)();
    static bool RegisterFactoryFunction(std::string name,
        TCreateMethod classFactoryFunction);
    static std::unique_ptr<Analysis> Create(std::string name);
    static std::map<std::string, std::unique_ptr<Analysis>(*)()> AnalyzerFactoryRegistry;
};

#endif // FACTORY_H

//-----------------------------------------------------------------------------
