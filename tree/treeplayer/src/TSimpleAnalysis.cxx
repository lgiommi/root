// @(#)root/treeplayer:$Id$
// Author: Luca Giommi   22/08/16

/*************************************************************************
 * Copyright (C) 1995-2016, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TSimpleAnalysis.h"

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TError.h"
#include "TKey.h"

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>

/** \class TSimpleAnalysis
A TSimpleAnalysis object permit to, given an input file, create a
.root file in which are saved histograms that the user wants to
create. The configuration file needs to set the parameters that allow
to create the histograms. This file has a fixed sintax, in which are
allowed comments (marked with the '#' sign) at the beginning of the
line (less than whitespaces), or at the end of the configuration
line. Empty or comment lines are skipped from the configuration procedure.
Here an example of configuration file:
```
#This is an example of configuration file
file_output.root   #the output file in which are stored histograms

#The next line has the name of the tree of the input data. It is
#optional if there is exactly one tree in the first input file
ntuple   #name of the input tree

#The lines of the next block correspond to .root input files that
#contain the tree
hsimple1.root   #first .root input file
hsimple2.root   #second .root input file

#The next block is composed by lines that allow to configure the
#histograms. They have to be the follow syntax:
#name = expression if cut
#which corresponds to chain->Draw("expression >> name", "cut")
#i.e. it will create a histogram called name and store it in
#file_output.root.
#"if cut" is optional
hpx=px if px<-3   #first histogram
hpxpy=px:py    #second histograms

#End of the configuration file
```
*/

////////////////////////////////////////////////////////////////////////////////
/// Deletes leading and trailing white spaces in a string
///
/// param[in] line line read from the input file

static void DeleteSpaces(std::string& line)
{
   std::size_t firstNotSpace = line.find_first_not_of(" \t");
   if (firstNotSpace != std::string::npos)
      line = line.substr(firstNotSpace, line.size() - firstNotSpace);
   else
      line.clear();
   std::size_t lastNotSpace = line.find_last_not_of(" \t");
   if (lastNotSpace != std::string::npos)
      line = line.substr(0, lastNotSpace + 1);
}

////////////////////////////////////////////////////////////////////////////////
/// It handles the expression lines of the input file in order to pass the
/// elements to the members of the object
///
/// param[in] line line or read from the input file either the expression passed to the constructor

std::string TSimpleAnalysis::HandleExpressionConfig(const std::string& line)
{
   static const std::string kCutIntr = " if ";

   std::size_t equal = line.find("=");
   if (equal == std::string::npos)
      return "Error: missing '='";
   std::size_t cutPos = line.find(kCutIntr, equal);
   std::string histName = line.substr(0, equal);
   DeleteSpaces(histName);
   if (histName.empty())
      return "Error: no histName found";

   std::string histExpression = line.substr(equal + 1, cutPos - equal - 1);
   DeleteSpaces(histExpression);
   if (histExpression.empty())
      return "Error: no expression found";

   std::string histCut;
   if (cutPos != std::string::npos) {
      histCut = line.substr(cutPos + kCutIntr.size());
      DeleteSpaces(histCut);
      if (histCut.empty())
         return "Error: invalid syntax";
   }
   else
      histCut = "";

   auto check = fHists.insert(std::make_pair((const std::string&)histName,
                                             std::make_pair(histExpression, histCut)));
   if (!check.second)
      return "Duplicate histogram name";
   return "";
}

////////////////////////////////////////////////////////////////////////////////
/// Constructor for the case of command line parsing arguments
///
/// \param[in] output name of the output file
/// \param[in] inputFiles name of the input .root files
/// \param[in] expressions what is showed in the histograms
/// \param[in] treeName name of the tree
/// \throws std::runtime_error in case of ill-formed expressions

TSimpleAnalysis::TSimpleAnalysis(const std::string& output,
                                 const std::vector<std::string>& inputFiles,
                                 const std::vector<std::string>& expressions,
                                 const std::string& treeName = ""):
   fInputFiles(inputFiles), fOutputFile(output), fTreeName(treeName)
{
   for (const std::string& expr: expressions) {
      std::string errMessage = HandleExpressionConfig(expr);
      if (!errMessage.empty())
         throw std::runtime_error(errMessage + " in " +  expr);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Needs to extract the name of the tree from the first input file when the tree
/// name isn't in the configuration file. Returns the name of the tree.

std::string TSimpleAnalysis::ExtractTreeName()
{
   std::string treeName = "";
   TFile inputFile (fInputFiles[0].c_str());
   for (TObject* keyAsObj : *inputFile.GetListOfKeys()) {
      TKey* key = dynamic_cast<TKey*>(keyAsObj);
      TClass* clObj = TClass::GetClass(key->GetClassName());
      if (!clObj)
         continue;
      if (clObj->InheritsFrom(TTree::Class())) {
         if (treeName.empty())
            treeName = key->GetName();
         else {
            ::Error("TSimpleAnalysis::Analyze", "Multiple trees inside %s", fInputFiles[0].c_str());
            return "";
         }
      }
   }
   if (treeName.empty()) {
      ::Error("TSimpleAnalysis::Analyze", "No tree inside %s", fInputFiles[0].c_str());
      return "";
   }
   return treeName;
}

////////////////////////////////////////////////////////////////////////////////
/// Executes all the TChain::Draw() as configured and stores the output histograms.
/// Returns true if the input arguments create correctly the output file

bool TSimpleAnalysis::Analyze()
{
   // Disambiguate tree name from first input file:
   // just try to open it, if that works it's an input file.
   if (TFile::Open(fTreeName.c_str())) {
      fInputFiles.insert(fInputFiles.begin(), fTreeName);
      fTreeName.clear();
      fTreeName = ExtractTreeName();
   }
   // Do the chain of the fInputFiles
   TChain chain(fTreeName.c_str());
   for (const std::string& inputfile: fInputFiles)
      chain.Add(inputfile.c_str());
   if (fInputFiles[0].find("=") != std::string::npos) {
      ::Error("TSimpleAnalysis::Analyze", "%s is an expression, not an input file",
              fInputFiles[0].c_str());
      return false;
   }
   int errValue = chain.LoadTree(0);
   if (errValue < 0) {
      ::Error("TSimpleAnalysis::Analyze",
              "The chain is not correctly set up, chain.LoadTree(0) returns %d", errValue);
      return false;
   }
   TFile ofile(fOutputFile.c_str(), "RECREATE");

   for (const auto &histo : fHists) {
      chain.Draw((histo.second.first + ">>" + histo.first).c_str(), histo.second.second.c_str(), "goff");
      TH1F *ptrHisto = (TH1F*)gDirectory->Get(histo.first.c_str());
      if (!ptrHisto)
         return false;
      ptrHisto->Write();
   }
   return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Returns false if not a tree name, otherwise sets the name of the tree.
///
/// param[in] line line read from the input file

bool TSimpleAnalysis::HandleInputConfig(const std::string& line)
{
   if (line.find("=") == std::string::npos) {
      fInputFiles.push_back(line);
      return true;
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
/// Removes everything after '#' and then removes leading and trailing spaces
///
/// param[in] line line read from the input file

static void RemoveComment(std::string& line)
{
   std::size_t comment = line.find('#');
   line = line.substr(0, comment);
   DeleteSpaces(line);
}


////////////////////////////////////////////////////////////////////////////////
/// Gets the next line from the configuration file and deletes leading and trailing spaces
///
/// param[in] line line read from the input file

void TSimpleAnalysis::GetLine(std::string& line)
{
   getline(fIn, line);
   DeleteSpaces(line);
}

////////////////////////////////////////////////////////////////////////////////
/// Skips subsequent empty lines in a string and returns the next no empty line
///
/// param[in] numbLine number of the input file line

std::string TSimpleAnalysis::GetNextNonEmptyLine(int& numbLine)
{
   std::string notEmptyLine;

   do {
      GetLine(notEmptyLine);
      numbLine++;
   } while (fIn && (notEmptyLine.empty() || notEmptyLine[0] == '#'));

   DeleteSpaces(notEmptyLine);
   return notEmptyLine;
}

////////////////////////////////////////////////////////////////////////////////
/// Gets lines from the configuration files up to the first no empty line and then
/// it removes comments
///
/// param[in] line line read from the input file
/// param[in] numbLine number of the input file line

void TSimpleAnalysis::GetLineForConfigure(std::string& line, int& numbLine)
{
   GetLine(line);
   numbLine++;
   if (line.empty())
      line = GetNextNonEmptyLine(numbLine);
   RemoveComment(line);
}

////////////////////////////////////////////////////////////////////////////////
/// This function has the aim of setting the arguments read from the input file

bool TSimpleAnalysis::Configure()
{
   int readingSection = kReadingOutput;
   std::string line;
   int numbLine = 0;

   fIn.open(fInputName);
   if (!fIn) {
      ::Error("TSimpleAnalysis", "File %s not found", fInputName.c_str());
      return 0;
   }

   while (!fIn.eof()) {
      std::string errMessage;

      GetLineForConfigure(line, numbLine);

      if (line.empty())
         continue;

         switch (readingSection) {
         case kReadingOutput:
            fOutputFile = line;
            readingSection++;
            break;

         case kReadingTreeName:
            fTreeName = line;
            readingSection++;
            break;

         case kReadingInput:
            if (!HandleInputConfig(line)) {
               errMessage = HandleExpressionConfig(line);
               readingSection = kReadingExpressions;
            }
            break;

         case kReadingExpressions:
            errMessage = HandleExpressionConfig(line);
            break;

         case kEndOfFile: break;
         }

         if (!errMessage.empty()) {
            ::Error("TSimpleAnalysis::Configure", "%s in %s:%d", errMessage.c_str(),
                    fInputName.c_str(), numbLine);
            return false;
         }
   }
   return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Function that allows to create the TSimpleAnalysis object and execute its
/// Configure and Analyze functions.
///
/// param[in] input name of the input file used to create the TSimpleAnalysis object

bool RunSimpleAnalysis (const char* configurationFile) {
   TSimpleAnalysis obj(configurationFile);
   obj.Configure();
   obj.Analyze();
   return true;
}
