# This code and its methods are adapted from
# https://github.com/llvm/llvm-project/blob/release/12.x/clang-tools-extra/clang-tidy/add_new_check.py

# Command line Python script to create and register new analyses
# Running this script will create a header (.h) and implementation (.cpp)
# file for your analysis in the cxx-langstat project directory and register
# it in CMakeLists.txt and AnalysisRegistry.cpp

# Steps of creating a new analysis:
# 1. Create YourAnalysis.cpp and YourAnalysis.h
# 2. In YourAnalysis.h, create header guards: YOURANALYSIS_H etc.
# 3. Include YourAnalysis.h in YourAnalysis.cpp
# 4. Add YouAnalysis to CMakeLists.txt
# 5. Add YourAnalysis to AnalysisRegistry.cpp
# 6. Implement analyzeFeatures and processFeatures in YourAnalysis.cpp
# 7. Advice: in YourAnalysis.h, create datastructure that holds features in neat
# way and implement functions to convert that type to/from JSON
#
# FIXME:
# analyzeFeatures and processFeatures should be renamed s.t. they reflect what
# they do
# remove this notice only when done


import os
import argparse


# Write header file for your new analysis.
def write_header(path, analysis_name, analysis_shorthand):
    filename = os.path.join(path, analysis_name) + '.h'
    print("Creating %s..." % filename)
    header_guard = analysis_name.upper() + '_H'
    # f-strings aren't great to use here due to C++'s liking in curly braces
    with open(filename, "w") as f:
        f.write("""
#ifndef %(header_guard)s
#define %(header_guard)s

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class %(analysis_name)s : public Analysis {
public:
    %(analysis_name)s();
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


# Write implementation for your new analysis.
def write_implementation(path, analysis_name, analysis_shorthand):
    filename = os.path.join(path, analysis_name) + '.cpp'
    print("Creating %s..." % filename)
    with open(filename, "w") as f:
        f.write("""
#include "cxx-langstat/Analyses/%(analysis_name)s.h"
#include "cxx-langstat/Utils.h"

// FIXME:
// Use nontrivial destructor if necessary
%(analysis_name)s::%(analysis_name)s(){
    std::cout << "This message means that %(analysis_name)s was successfully registered!\\n";
}

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


# Adapts the CMake file that creates the static libAnalysis library
# (see lib/Analyses/CMakeLists.txt)
# Copied from clang-tidy's add_new_check.py
def adapt_cmake(path, analysis):
    filename = os.path.join(path, 'CMakeLists.txt')
    with open(filename, 'r') as f:
        lines = f.readlines()

    cpp_file = analysis + '.cpp'

    # Figure out whether this analysis already exists.
    for line in lines:
        if line.strip() == cpp_file:
            print("An analysis with this name already exists. Exiting.")
            return False

    print('Updating %s...' % filename)
    with open(filename, 'w') as f:
        cpp_found = False
        file_added = False
        for line in lines:
            cpp_line = line.strip().endswith('.cpp')
            if (not file_added) and (cpp_line or cpp_found):
                cpp_found = True
                if (line.strip() > cpp_file) or (not cpp_line):
                    f.write('    ' + cpp_file + '\n')
                    file_added = True
            f.write(line)

    return True


# Register the analysis in AnalysisRegistry.cpp
# Conceptually similar to adapt_cmake, but reads lines with a step size of 2
# when necessary
# Adapted from adapt_cmake
def register_analysis(registry_path, headers_path, analysis, analysis_shorthand):
    filename = os.path.join(registry_path, 'AnalysisRegistry.cpp')
    with open(filename, 'r') as f:
        lines = f.readlines()

    # Register analysis s.t. cxx-langstat can create instances of it when run
    # Admittedly, suboptimal, but that's how cxx-langstat works as of now
    # TODO: add TODO to change this if cxx-langstat changes the way that it
    # handles the registration of analyses

    if_stmt = f"if(Options.EnabledAnalyses.contains(\"{analysis_shorthand}\"))"
    registration = f"Analyses.emplace_back(std::make_unique<{analysis}>());"

    # If analysis with this shorthand is already registered, we should reject
    # the new analysis here
    for line in lines:
        if line.strip() == if_stmt:
            print("An analysis with this shorthand has already been registered. Exiting.")
            return False

    print('Updating %s...' % filename)
    with open(filename, 'w') as f:
        registration_found = False
        analysis_registered = False
        index = 0
        while index < len(lines)-1:
            line = lines[index]
            nextline = lines[index+1]

            is_registration = line.strip().startswith('if(Options.EnabledAnalyses.contains')
            is_registration = is_registration and nextline.strip().startswith('Analyses.emplace_back(std::make_unique')
            if (not analysis_registered) and (is_registration or registration_found):
                registration_found = True
                if (line.strip() > if_stmt.strip()) or (not is_registration):
                    f.write('    ' + if_stmt + '\n')
                    f.write('    ' + '    ' + registration + '\n')
                    analysis_registered = True
            f.write(line)
            # the current and the next line are a registration, thus we write both
            # and skip one ahead
            if is_registration:
                f.write(nextline)
                index += 1
            index += 1
        # Because we're using a step-size of 2, we have to not forget to write
        # the last line explicitly
        f.write(lines[-1])

    # Add header file of analysis to includes
    header_file = os.path.join(headers_path, analysis) + '.h'
    include_line = '#include \"' + header_file + '\"'

    with open(filename, 'r') as f:
        lines = f.readlines()
    # Figure out whether this include already exists.
    for line in lines:
        if line.strip() == include_line:
            return False

    with open(filename, 'w') as f:
        include_found = False
        file_included = False
        for line in lines:
            is_analysis_header_include = line.strip().startswith('#include \"' + headers_path)
            if (not file_included) and (is_analysis_header_include or include_found):
                include_found = True
                if (line.strip() > include_line) or (not is_analysis_header_include):
                    f.write(include_line + '\n')
                    file_included = True
            f.write(line)

    return True


# Ensure analysis name ends with 'Analysis'
def full_name(analysis):
    return analysis.split('Analysis')[0] + 'Analysis'


#
def main():
    # Parse analysis name and shorthand from terminal
    parser = argparse.ArgumentParser(description="Python script for adding new analyses")
    parser.add_argument('analysis', help='Full name of the anlysis, e.g. ContainerLibAnalysis')
    parser.add_argument('analysis_shorthand', help='Shorthand used to enable the anlysis when running cxx-langstat, e.g. CLA')
    args = parser.parse_args()
    analysis = full_name(args.analysis)
    analysis_shorthand = args.analysis_shorthand

    if (analysis == '' or analysis == 'Analysis'):
        print('Analysis name may not be empty or \'Analysis\'')
        return False

    if (analysis_shorthand == ''):
        print('Analysis shorthand may not be empty')
        return False

    # Standard directories to put analyses
    header_path = 'cxx-langstat/Analyses'
    implementation_path = 'lib/Analyses'

    # FIXME: revert CMakelists.txt if registering analysis fails
    if (not adapt_cmake(implementation_path, analysis)):
        return
    if (not register_analysis('lib', header_path, analysis, analysis_shorthand)):
        return
    # Try to add to CMake and register analysis first before writing the actual
    # files for the analysis to avoid cluttering up the project directories if
    # analysis can't be successfully registered
    write_header(os.path.join('include', header_path), analysis, analysis_shorthand)
    write_implementation(implementation_path, analysis, analysis_shorthand)



if __name__ == "__main__":
    main()
