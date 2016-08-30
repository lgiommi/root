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

#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TError.h"

/** \class TSympleAnalysis
A TSimpleAnalysis object permit to, given an input file,
create an .root file in which are saved histograms
that the user wants to create.
*/

const std::string TSimpleAnalysis::kCutIntr="if";


////////////////////////////////////////////////////////////////////////////////
/// Constructor for the case with the input file
///
/// \param[in] kFile name of the input file that has to be read

TSimpleAnalysis::TSimpleAnalysis(const std::string& file): fInputName(file)
{}

////////////////////////////////////////////////////////////////////////////////
/// Constructor for the case of command line parsing arguments
///
/// \param[in] output name of the output file
/// \param[in] inputFiles name of the input .root files
/// \param[in] name name of the tree
/// \param[in] expressions what is showed in the histograms

TSimpleAnalysis::TSimpleAnalysis(const std::string& output, const std::vector<std::string>& inputFiles,
                                 const std::string& name, std::vector<std::string> expressions):
   fInputFiles(inputFiles), fOutputFile(output), fTreeName(name)
{
   std::string histExpressions;
   std::string histNames;
   std::string histCut;
   for (std::string expr: expressions) {
      std::size_t equal=expr.find("=");
      if (equal == std::string::npos) {
         ::Error("TSimpleAnalysis",
                 "Missing '=' in %s",expr.c_str());
         throw std::runtime_error("Error");
      }
      std::size_t cutPos=expr.find(kCutIntr, equal);
      std::string substring=expr.substr(0,equal);
      if (substring.empty()) {
         ::Error("TSimpleAnalysis",
                 "No hname found in %s",expr.c_str());
         throw std::runtime_error("Error");
      }
      histNames.append(substring);
      substring=expr.substr(equal+1,cutPos-equal-1);
      if (substring.empty()) {
         ::Error("TSimpleAnalysis",
                 "No expression found in %s",expr.c_str());
         throw std::runtime_error("Error");
      }
      histExpressions.append(substring);
      if (cutPos == std::string::npos) {
         histCut.append("");
      } else {
         histCut.append(expr.substr(cutPos+kCutIntr.size()));
      }
   }
   auto check =
      fHists.insert(std::make_pair((const std::string&)histNames,
                                   std::make_pair(histExpressions,histCut)));
   if (check.second==false) {
      ::Error("TSimpleAnalysis","The histogram name '%s' is already existing", histNames.c_str());
      throw std::runtime_error("Name of the histogram already existing");
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Return true if the input arguments create the output file correctly

bool TSimpleAnalysis::Analysis()
{
   TChain chain(fTreeName.c_str());
   for (const std::string& inputfile: fInputFiles)
      chain.Add(inputfile.c_str());
   TFile ofile(fOutputFile.c_str(),"RECREATE");

   for (const auto &histo : fHists) {
      chain.Draw((histo.second.first + ">>" + histo.first).c_str(), histo.second.second.c_str(), "goff");
      TH1F *ptrHisto = (TH1F*)gDirectory->Get(histo.first.c_str());
      ptrHisto->Write();
      if (ptrHisto == nullptr)
         return false;
   }
   return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Return false if not a tree name
///
/// param[in] line line read from the input file

static bool HandleTreeNameConfig(const std::string& line)
{
   return line.find("=") == std::string::npos;
}

////////////////////////////////////////////////////////////////////////////////
/// Delete white spaces in a string
///
/// param[in] line line read from the input file

static bool DeleteSpaces(std::string& line)
{
   std::size_t firstNotSpace = line.find_first_not_of(" ");
   if (firstNotSpace != std::string::npos)
      line = line.substr(firstNotSpace,line.size()-firstNotSpace);
   std::size_t lastNotSpace = line.find_last_not_of(" ");
   if (lastNotSpace != std::string::npos)
      line = line.substr(0,lastNotSpace+1);
   return 1;
}

////////////////////////////////////////////////////////////////////////////////
/// It handles the expression lines of the input file in order to pass the
/// elements to the members of the object
///
/// param[in] line line read from the input file
/// param[in] numbLine number of the input file line

void TSimpleAnalysis::HandleExpressionConfig(std::string& line, int& numbLine)
{
   std::string histExpressions;
   std::string histNames;
   std::string histCut;
   std::size_t equal = line.find("=");
   if (equal == std::string::npos) {
      ::Error("TSimpleAnalysis::HandlefExpressionConfig",
              "Missing '=' in %s:%d",fInputName.c_str(), numbLine);
      throw std::runtime_error("Error");
   }
   std::size_t cutPos = line.find(kCutIntr, equal);
   std::string substring=line.substr(0,equal);
   if (substring.empty()) {
      ::Error("TSimpleAnalysis::HandlefExpressionConfig",
              "No hname found in %s:%d",fInputName.c_str(), numbLine);
      throw std::runtime_error("Error");
   }
   DeleteSpaces(substring);
   histNames.append(substring);
   substring=line.substr(equal+1,cutPos-equal-1);
   if (substring.empty()) {
      ::Error("TSimpleAnalysis::HandlefExpressionConfig",
              "No expression found in %s:%d",fInputName.c_str(), numbLine);
      throw std::runtime_error("Error");
   }
   DeleteSpaces(substring);
   histExpressions.append(substring);
   if (cutPos == std::string::npos) {
      histCut.append("");
   } else {
      histCut.append(line.substr(cutPos+kCutIntr.size()));
   }

    auto check =
      fHists.insert(std::make_pair((const std::string&)histNames,
                                   std::make_pair(histExpressions,histCut)));
   if (check.second==false) {
      ::Error("TSimpleAnalysis::HandleExpressionConfig",
              "The histogram name '%s' is already existing", histNames.c_str());
      throw std::runtime_error("Name of the histogram already existing");
   }
}


////////////////////////////////////////////////////////////////////////////////
/// Skip subsequent empty lines in a string and returns the number of the
/// current line of the input file
///
/// param[in] numbLine number of the input file line

std::string TSimpleAnalysis::SkipSubsequentEmptyLines(int& numbLine)
{
   std::string notEmptyLine;

   while (getline(fIn,notEmptyLine) && DeleteSpaces(notEmptyLine)
          && (notEmptyLine.empty() || notEmptyLine.find("#") == 0 ||
              notEmptyLine.find_first_not_of(" ") == std::string::npos))

   numbLine++;
   return notEmptyLine;
}

////////////////////////////////////////////////////////////////////////////////
/// After the eventual skip of subsequent empty lines, returns true if the
/// line is a comment
///
/// param[in] line line read from the input file
/// param[in] readingSection current section of the read file
/// param[in] numbLine number of the input file line

bool TSimpleAnalysis::HandleLines(std::string& line, int& readingSection, int& numbLine)
{
   if (line.empty() || line.find_first_not_of(" ") == std::string::npos) {
      if (readingSection == 0 && fCounter == 0)
         return 1;
      readingSection++;
      line = SkipSubsequentEmptyLines(numbLine);
   }

   DeleteSpaces(line);
   std::size_t comment = line.find("#");
   if (comment==0)
      return 1;
   if (((comment != 0) || (comment != std::string::npos)) && readingSection == 0)
      fCounter++;
   line = line.substr(0,comment);
   DeleteSpaces(line);
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// This function has the aim of setting the arguments read from the input file

void TSimpleAnalysis::Configure()
{
   int readingSection = kReadingOutput;
   std::string line;
   int numbLine = 0;

   fIn.open(fInputName);
   if(!fIn) {
      ::Error("TSimpleAnalysis","File %s not found",fInputName.c_str());
      throw std::runtime_error("Error");
   }

   while(!fIn.eof()) {

      getline (fIn,line);
      numbLine++;
      if (HandleLines(line,readingSection,numbLine)==1)
         continue;

      switch (readingSection) {
      case kReadingOutput:
         fOutputFile = line;
         break;

      case kReadingInput:
         fInputFiles.push_back(line);
         break;

      case kReadingTreeName:

         if (HandleTreeNameConfig(line) == true) {
            fTreeName = line;
            readingSection = kReadingExpressions;
         }
         else {
            HandleExpressionConfig(line,numbLine);
         }
         break;

      case kReadingExpressions:
         HandleExpressionConfig(line,numbLine);
         break;

      case kEndOfFile: break;
      }
   }
}
