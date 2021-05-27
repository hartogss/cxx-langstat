def write_header(analysis_name, analysis_shorthand):
    print("Creating header file")
    filename = analysis_name + '.h'
    header_guard = analysis_name.upper() + "_H"
    # f-strings aren't great to use here due to C/C++'s liking in curly braces
    with open(filename, "w") as f:
        f.write("""
#ifndef %(header_guard)s
#define %(header_guard)s

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class %(analysis_name)s : public Analysis {
public:
    %(analysis_name)s();
    // FIXME:
    // Use nontrivial destructor if necessary
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    static constexpr auto ShorthandName = "%(analysis_shorthand)s";
};

//-----------------------------------------------------------------------------

#endif // %(header_guard)s
""" % { "analysis_name" : analysis_name,
        "analysis_shorthand" : analysis_shorthand,
        "header_guard" : header_guard})

def write_implementation(analysis_name, analysis_shorthand):
    print("Creating implementation")
    filename = analysis_name + '.cpp'
    with open(filename, "w") as f:
        f.write("""
#include "cxx-langstat/Analyses/%(analysis_name)s.h"
#include "cxx-langstat/Utils.h"

//-----------------------------------------------------------------------------
// FIXME: implementation
void %(analysis_name)s::analyzeFeatures(){

}
// FIXME: implementation
void %(analysis_name)s::processFeatures(nlohmann::ordered_json j){

}

//-----------------------------------------------------------------------------

""" % { "analysis_name" : analysis_name,
        "analysis_shorthand" : analysis_shorthand})



def main():
    write_header("newtest", "nta")
    write_implementation("newtest", "nta")


if __name__ == "__main__":
    main()
