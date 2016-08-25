// @(#)root/tree:$Id$
// Author: Luca Giommi   22/08/16

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
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
// A TSimpleAnalysis object permit to, given an input file or through   //
// command line, create a .root file in which are saved histograms      //
// that the user want to create.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TFile
#include "TFile.h"
#endif

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif

#ifndef ROOT_TChain
#include "TChain.h"
#endif

#ifndef ROOT_TH1
#include "TH1.h"
#endif

#ifndef ROOT_TError
#include "TError.h"
#endif

#include <fstream>

class TSimpleAnalysis {

private:
   std::string fInputName;   ///< Name of the input file
   std::vector<std::string> fInputFiles;   ///< .root input files
   std::vector<std::string> fExpressions;   ///< What is showed by the histograms inside the output file
   std::vector<std::string> fHNames;   ///< Names of the histograms
   std::vector<std::string> fCut;   ///< Cuts added to the hisograms
   std::string fOutputFile;   ///< Output file in which are stored the histograms
   std::string fTreeName;   ///< Name of the input tree
   std::ifstream in;   ///< Stream for the input file

   //The elements of the enumeration refer to the different typologies of elements
   //that are in the input file
   enum EReadingWhat {kReadingOutput,kReadingInput,kReadingTreeName,
                      kReadingExpressions,kEndOfFile};
   static const std::string kCutIntr;   ///< The string that represents the starter point of the cut expresson
   Int_t fCounter=0;   ///< Counter usefull for the reading of the file

public:
   TSimpleAnalysis(const std::string& kFile);
   TSimpleAnalysis(const std::string& output, std::vector<std::string> inputFiles,
                   const std::string& name, std::vector<std::string> expressions);
   void CheckHNames(std::vector<std::string> name);
   Bool_t Analysis();
   Bool_t HandleTreeNameConfig(const std::string& line);
   void HandlefExpressionConfig(std::string& line, Int_t& numbLine);
   Bool_t DeleteSpaces (std::string& line);
   std::string SkipSubsequentEmptyLines(Int_t& numbLine);
   Bool_t HandleLines(std::string& line, Int_t& readingSection, Int_t& numbLine);
   void Settings();

};

#endif
