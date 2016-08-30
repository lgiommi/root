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
// A TSimpleAnalysis object permit to, given an input file, create a    //
// .root file in which are saved histograms that the user wants to      //
// create. The configuration file needs to set the parameters that allow//
// to create the histograms. This file has a fixed sintax, in which are //
// allowed comments (marked with the '#' sign) at the beginning of the  //
// line (less than whitespaces), or at the end of the configuration     //
// line. Subsequently empty and/or comment lines or both are skipped    //
// from the configuration procedure.                                    //
// Here an example of configuration file:                               //
//                                                                      //
// #This is an example of configuration file                            //
// file_output.root   #the output file in which are stored histograms   //
// #the next line has to be an empty line                               //
//                                                                      //
// #The lines of the next block correspond to .root input files in which//
// #are stored the Ntuple                                               //
// hsimple1.root   #first .root input file                              //
// hsimple2.root   #second .root input file                             //
// #the next line has to be an empty line                               //
//                                                                      //
// #The next line has the name of the tree of the input data            //
// ntuple   #name of the input tree                                     //
// #The next block is composed by lines that allow to configure the     //
// #histograms. They have to be the follow sintax:                      //
// #hName=hExpr if hCut   #where:                                       //
// #'hName' is the name of the histogram written in the output file     //
// #'hExpr' is the expression reproduced in the histogram               //
// #' if '  is the expression that tell us the begin of the optional    //
// #        cut expression                                              //
// #'hCut'  is the optional cut expression in the histogram             //
// hpx=px if px<-3   #first histogram                                   //
// hpxpy=px:py    #second histograms                                    //
//                                                                      //
// #End of the configuration file                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

#include <string>
#include <fstream>
#include <vector>

class TSimpleAnalysis {

private:
   std::string              fInputName; ///< Name of the input file
   std::vector<std::string> fInputFiles; ///< .root input files
   std::vector<std::string> fExpressions; ///< What is showed by the histograms inside the output file
   std::vector<std::string> fHistNames; ///< Names of the histograms
   std::vector<std::string> fCut; ///< Cuts added to the hisograms
   std::string              fOutputFile; ///< Output file in which are stored the histograms
   std::string              fTreeName; ///< Name of the input tree
   std::ifstream            fIn; ///< Stream for the input file

   //The elements of the enumeration refer to the different typologies of elements
   //that are in the input file
   enum EReadingWhat {
      kReadingOutput,
      kReadingInput,
      kReadingTreeName,
      kReadingExpressions,
      kEndOfFile
   };
   static const std::string kCutIntr; ///< The string that represents the starter point of the cut expresson
   Int_t fCounter=0; ///< Counter usefull for the reading of the file
   void HandleExpressionConfig(std::string& line, Int_t& numbLine);
   std::string SkipSubsequentEmptyLines(Int_t& numbLine);
   bool HandleLines(std::string& line, Int_t& readingSection, Int_t& numbLine);


public:
   TSimpleAnalysis(const std::string& file);
   TSimpleAnalysis(const std::string& output, const std::vector<std::string>& inputFiles,
                   const std::string& name, std::vector<std::string> expressions);
   bool Analysis();
   void Configure();

};

#endif
