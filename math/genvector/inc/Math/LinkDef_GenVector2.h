// dictionary for std::vector of genvector objects
// problem on Windows: CINT cannot deal with  too long class name
// generated by an std::vector<ROOT::Math::Vector>

#ifndef _WIN32
#pragma extra_include "vector";
#include <vector>

// conflict on solaris between template class T from std::vector and T().
#ifndef __sun
#pragma link C++ class std::vector<ROOT::Math::XYZTVector >+;
#pragma link C++ class std::vector<ROOT::Math::PtEtaPhiEVector >+;
#endif

#pragma link C++ class std::vector<ROOT::Math::XYZVector >+;
#pragma link C++ class std::vector<ROOT::Math::XYZPoint >+;

#pragma link C++ class std::vector<ROOT::Math::RhoEtaPhiVector >+;
#pragma link C++ class std::vector<ROOT::Math::RhoEtaPhiPoint >+;

#endif       // endif Win32
