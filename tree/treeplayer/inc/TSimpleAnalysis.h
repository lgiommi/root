// @(#)root/treeplayer:$Id$
// Author: Luca Giommi   22/08/16

/*************************************************************************
 * Copyright (C) 1995-2016, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TSimpleAnalysis
#define ROOT_TSimpleAnalysis

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSimpleAnalysis                                                      //
//                                                                      //
// A TSimpleAnalysis object allow to, given an input file, create a     //
// .root file in which are saved histograms that the user wants to      //
// create.                                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <string>
#include <fstream>
#include <vector>
#include <map>

class TSimpleAnalysis {

private:
   std::string              fInputName; ///< Name of the input file
   std::vector<std::string> fInputFiles; ///< .root input files
   std::string              fOutputFile; ///< Output file in which are stored the histograms
   std::string              fTreeName; ///< Name of the input tree
   std::ifstream            fIn; ///< Stream for the input file
   std::map<std::string, std::pair<std::string, std::string>> fHists; ///< contains
   //in the first part the names of the histograms written in the output file, in the
   //second part the pair of what is showed in the histograms and the cut applied on the variables

   //The elements of the enumeration refer to the different typologies of elements
   //that are in the input file
   enum EReadingWhat {
      kReadingOutput,
      kReadingTreeName,
      kReadingInput,
      kReadingExpressions,
      kEndOfFile
   };

   std::string HandleExpressionConfig(const std::string& line);
   std::string GetNextNonEmptyLine(int& numbLine);
   std::string ExtractTreeName();
   void GetLine(std::string& line);
   bool HandleInputConfig(const std::string& line);
   void GetLineForConfigure(std::string& line, int& numbLine);


public:
   TSimpleAnalysis(const std::string& file): fInputName (file) {}
   TSimpleAnalysis(const std::string& output, const std::vector<std::string>& inputFiles,
                   const std::vector<std::string>& expressions, const std::string& treeName);
   bool Analyze();
   bool Configure();

};

bool RunSimpleAnalysis(const char* configurationFile);

#endif
