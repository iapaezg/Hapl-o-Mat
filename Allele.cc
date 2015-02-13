#include <iostream>
#include <algorithm>

#include "Allele.h"
#include "Utility.h"

FileAllelesTogOrG Allele::fileAllelesTog("data/H1g.txt");
FileAllelesTogOrG Allele::fileAllelesToG("data/H1.txt");

FilegOrGOr4dToAlleles Allele::fileGToAlleles("data/H1.txt");
FilegOrGOr4dToAlleles Allele::filegToAlleles("data/H1g.txt");
FilegOrGOr4dToAlleles Allele::file4dToAlleles("data/allAllelesExpanded.txt");

std::shared_ptr<Allele> Allele::createAllele(const std::string code, const Allele::codePrecision wantedPrecision, const double alleleFrequency){

  std::shared_ptr<Allele> pAllele;
  Allele::codePrecision precision = identifyCodePrecision(code);
  switch(precision){
  case Allele::codePrecision::g:
    {
      pAllele = std::make_shared<Alleleg> (code, precision, wantedPrecision, alleleFrequency);
      break;
    }
  case Allele::codePrecision::fourDigit:
    {
      pAllele = std::make_shared<Allele4d> (code, precision, wantedPrecision, alleleFrequency);
      break;
    }
  case Allele::codePrecision::G:
    {
      pAllele = std::make_shared<AlleleG> (code, precision, wantedPrecision, alleleFrequency);
      break;
    }
  case Allele::codePrecision::sixDigit:
    {
      pAllele = std::make_shared<Allele6d> (code, precision, wantedPrecision, alleleFrequency);
      break;
    }
  case Allele::codePrecision::eightDigit:
    {
      pAllele = std::make_shared<Allele8d> (code, precision, wantedPrecision, alleleFrequency);
      break;
    }
  case Allele::codePrecision::asItIs:
    {
      break;
    }
  }//switch

  return pAllele;
}

Allele::codePrecision Allele::identifyCodePrecision(const std::string code){

  Allele::codePrecision precision;
  if(checkLastLetter(code, 'g'))
    precision = Allele::codePrecision::g;
  else if(checkLastLetter(code, 'G'))
    precision = Allele::codePrecision::G;
  else{
    size_t numberColons = std::count(code.begin(), code.end(), ':');
    switch (numberColons){
    case 1:
      {
	precision = Allele::codePrecision::fourDigit;
	break;
      }
    case 2:
      {
	precision = Allele::codePrecision::sixDigit;
	break;
      }
    case 3:
      {
	precision = Allele::codePrecision::eightDigit;
	break;
      }
    default:
      std::cerr << "Code "
		<< code
		<<" can not be assigned to known precision."
		<< std::endl;
      exit (EXIT_FAILURE);

    }//switch
  }

  return precision;
}

std::string Allele::printCodePrecision(const codePrecision precision){

  std::string out;
  switch(precision){
  case codePrecision::g:
    {
      out = "g";
      break;
    }
  case codePrecision::fourDigit:
    {
      out = "4d";
      break;
    }
  case codePrecision::G:
    {
      out = "G";
      break;
    }
  case codePrecision::sixDigit:
    {
      out = "6d";
      break;
    }
  case codePrecision::eightDigit:
    {
      out = "8d";
      break;
    }
  case Allele::codePrecision::asItIs:
    {
      out = "asItIs";
      break;
    }
  }
  return out;
}

std::vector<std::shared_ptr<Allele>> Allele::translate(){

  switch(wantedPrecision){
  case Allele::codePrecision::g:
    {
      return this->translateTog();
      break;
    }
  case Allele::codePrecision::fourDigit:
    {
      return this->translateTo4d();
      break;
    }
  case Allele::codePrecision::G:
    {
      return this->translateToG();
      break;
    }
  case Allele::codePrecision::sixDigit:
    {
      return this->translateTo6d();
      break;
    }
  case Allele::codePrecision::eightDigit:
    {
      return this->translateTo8d();
      break;
    }
  case Allele::codePrecision::asItIs:
    {
      std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
      listOfPAlleleg.push_back(this->create(code, precision, Allele::codePrecision::asItIs, frequency));
      return listOfPAlleleg;
      break;
    }
  default:
    {
      std::cerr << "Can not assign wanted precision to known precision." << std::endl;
      exit (EXIT_FAILURE);
    }
  }//switch                                             
}

std::vector<std::shared_ptr<Allele>>::iterator Allele::ispAlleleInList (std::vector<std::shared_ptr<Allele>> & listOfpAlleles) const{
    
  std::string alleleCode = code;
  std::vector<std::shared_ptr<Allele>>::iterator pos = find_if(listOfpAlleles.begin(),
							       listOfpAlleles.end(),
							       [alleleCode](const std::shared_ptr<Allele> & allele)
							       {
								 return alleleCode == allele->getCode();
							       });
  return pos;
}

std::string Allele::allelesTog(){

  std::string codeInPrecision;
  std::string locus = getLocus(code);
  FileAllelesTogOrG::list_t::const_iterator pos;
  FileAllelesTogOrG::list_t::const_iterator lastPos;
  fileAllelesTog.findPositionLocus(locus, pos, lastPos);
  
  bool found = false;
  while(pos != lastPos && found==false){
    std::string gCodeWithoutg = leftOfFirstDelim(pos->first, 'g');
    if(code == gCodeWithoutg){
      codeInPrecision = pos->first;
      found = true;
    }
    else{
      for(auto entry = pos->second.cbegin();
	  entry != pos->second.cend() && found==false;
	  entry ++){
	if(code == *entry){
	  codeInPrecision = pos->first;
	  found = true;
	}
      }//for entries line
    }//else
    pos ++;
  }//while lines in fileAllelesTog

  if(found == false){
    std::string code4d = cutCodeKeepingLastLetter(code, 1);
    codeInPrecision = code4d;
  }

  return codeInPrecision;
}

strVec_t Allele::allelesToG(){

  strVec_t codesInPrecision;
  std::string locus = getLocus(code);
  FileAllelesTogOrG::list_t::const_iterator pos;
  FileAllelesTogOrG::list_t::const_iterator lastPos;
  fileAllelesToG.findPositionLocus(locus, pos, lastPos);
  
  while(pos != lastPos){
    bool found = false;
    for(auto entry = pos->second.cbegin();
	entry != pos->second.cend() && found == false;
	entry ++){
      if(code == *entry){
	codesInPrecision.push_back(pos->first);
	found = true;
      }
    }//for entries line
    pos ++;
  }//while lines in fileAllelesTog

  if(codesInPrecision.empty()){
    std::string code6d = cutCodeKeepingLastLetter(code, 2);
    codesInPrecision.push_back(code6d);
  }

  return codesInPrecision;
}

strVec_t Allele::GToAlleles(){

  strVec_t newCodes;
  auto itFileGToAlleles = fileGToAlleles.getList().find(code);
  if(itFileGToAlleles == fileGToAlleles.getList().cend()){
    std::cerr << "Could not find G-Code "
              << code
	      << " in file"
	      << fileGToAlleles.getFileName()
	      << "."
              << std::endl;
    exit (EXIT_FAILURE);
  }
  else{
    newCodes = itFileGToAlleles->second;
  }

  return newCodes;
}

strVec_t Allele::gToAlleles(){

  strVec_t codesInPrecision;
  auto itFilegToAlleles = filegToAlleles.getList().find(code);
  if(itFilegToAlleles == filegToAlleles.getList().cend()){
    std::cerr << "Could not find g-Code "
              << code
	      << " in file "
	      << filegToAlleles.getFileName()
	      << "."
              << std::endl;
    exit (EXIT_FAILURE);
  }
  else{
    codesInPrecision = itFilegToAlleles->second;
  }

  return codesInPrecision;
}


strVec_t Allele::expandPrecision(){

  strVec_t codesInPrecision;
  auto pos = file4dToAlleles.getList().find(code);
  if(pos != file4dToAlleles.getList().cend()){
    codesInPrecision = pos->second;
  }
  else{
    std::cerr << "Missing translation of "
	      << code 
	      << " to higher precisions in file "
	      << file4dToAlleles.getFileName()
	      << "."
	      << std::endl;
    exit (EXIT_FAILURE);
  }

  return codesInPrecision;
}


std::vector<std::shared_ptr<Allele>> Allele4d::translateTog(){

  strVec_t codesInPrecision;
  strVec_t codesIn8d = expandPrecision();
  for(auto codeIn8d : codesIn8d){
    code = codeIn8d;
    std::string codeIng = allelesTog();
    codesInPrecision.push_back(codeIng);
  }

  std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleg = std::make_shared<Alleleg> (codeInPrecision, frequency);
    auto pos = pAlleleg->ispAlleleInList(listOfPAlleleg);
    if(pos == listOfPAlleleg.cend())
      listOfPAlleleg.push_back(pAlleleg);
    else
      (*pos)->addFrequency(pAlleleg->getFrequency());
  }

  return listOfPAlleleg;
}

std::vector<std::shared_ptr<Allele>> Alleleg::translateTog(){

  std::shared_ptr<Allele> pAlleleg = std::make_shared<Alleleg> (code, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
  listOfPAlleleg.push_back(pAlleleg);
  return listOfPAlleleg;
}

std::vector<std::shared_ptr<Allele>> AlleleG::translateTog(){

  strVec_t codesIn8d = GToAlleles();
 
  strVec_t codesInPrecision;
  for(auto codeIn8d : codesIn8d){
    code = codeIn8d;
    std::string codeIng = allelesTog();
    codesInPrecision.push_back(codeIng);
  }

  std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleg = std::make_shared<Alleleg> (codeInPrecision, frequency);
    auto pos = pAlleleg->ispAlleleInList(listOfPAlleleg);
    if(pos == listOfPAlleleg.cend())
      listOfPAlleleg.push_back(pAlleleg);
    else
      (*pos)->addFrequency(pAlleleg->getFrequency());
  }

  return listOfPAlleleg;
}

std::vector<std::shared_ptr<Allele>> Allele6d::translateTog(){

  strVec_t codesInPrecision;
  strVec_t codesIn8d = expandPrecision();
  for(auto codeIn8d : codesIn8d){
    code = codeIn8d;
    std::string codeIng = allelesTog();
    codesInPrecision.push_back(codeIng);
  }

  std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleg = std::make_shared<Alleleg> (codeInPrecision, frequency);
    auto pos = pAlleleg->ispAlleleInList(listOfPAlleleg);
    if(pos == listOfPAlleleg.cend())
      listOfPAlleleg.push_back(pAlleleg);
    else
      (*pos)->addFrequency(pAlleleg->getFrequency());
  }

  return listOfPAlleleg;
}

std::vector<std::shared_ptr<Allele>> Allele8d::translateTog(){

  std::string codeInPrecision =  allelesTog();
  std::vector<std::shared_ptr<Allele>> listOfPAlleleg;
  std::shared_ptr<Allele> pAlleleg = std::make_shared<Alleleg> (codeInPrecision, frequency);
  listOfPAlleleg.push_back(pAlleleg);
  
  return listOfPAlleleg;
}

std::vector<std::shared_ptr<Allele>> Allele4d::translateToG(){
  
  strVec_t codesInPrecision;
  strVec_t codesIn8d = expandPrecision();
  for(auto codeIn8d : codesIn8d){
    code = codeIn8d;
    strVec_t newCodesInPrecision = allelesToG();
    codesInPrecision.insert(codesInPrecision.end(),
			    newCodesInPrecision.cbegin(),
			    newCodesInPrecision.cend());
  }
  std::vector<std::shared_ptr<Allele>> listOfPAlleleG;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleG = std::make_shared<AlleleG> (codeInPrecision, frequency);
    auto pos = pAlleleG->ispAlleleInList(listOfPAlleleG);
    if(pos == listOfPAlleleG.cend())
      listOfPAlleleG.push_back(pAlleleG);
    else
      (*pos)->addFrequency(pAlleleG->getFrequency());
  }

  return listOfPAlleleG;
}

std::vector<std::shared_ptr<Allele>> Alleleg::translateToG(){

  strVec_t codesInPrecision;
  strVec_t codesIn4d = gToAlleles();
  for(auto codeIn4d : codesIn4d){
    code = codeIn4d;
    strVec_t codesIn8d = expandPrecision();
    for(auto codeIn8d : codesIn8d){
      strVec_t newCodesInPrecision = allelesToG();
      codesInPrecision.insert(codesInPrecision.end(),
			      newCodesInPrecision.cbegin(),
			      newCodesInPrecision.cend()); 
    }
  }  

  std::vector<std::shared_ptr<Allele>> listOfPAlleleG;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleG = std::make_shared<AlleleG> (codeInPrecision, frequency);
    auto pos = pAlleleG->ispAlleleInList(listOfPAlleleG);
    if(pos == listOfPAlleleG.cend())
      listOfPAlleleG.push_back(pAlleleG);
    else
      (*pos)->addFrequency(pAlleleG->getFrequency());
  }

  return listOfPAlleleG;
}

std::vector<std::shared_ptr<Allele>> AlleleG::translateToG(){

  std::shared_ptr<Allele> pAlleleG = std::make_shared<AlleleG> (code, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAlleleG;
  listOfPAlleleG.push_back(pAlleleG);
  return listOfPAlleleG;
}

std::vector<std::shared_ptr<Allele>> Allele6d::translateToG(){

  strVec_t codesInPrecision;
  strVec_t codesIn8d = expandPrecision();
  for(auto codeIn8d : codesIn8d){
    code = codeIn8d;
    strVec_t codesInG = allelesToG();
    codesInPrecision.insert(codesInPrecision.end(),
			    codesInG.cbegin(),
			    codesInG.cend()); 
  }

  std::vector<std::shared_ptr<Allele>> listOfPAlleleG;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleG = std::make_shared<AlleleG> (codeInPrecision, frequency);
    auto pos = pAlleleG->ispAlleleInList(listOfPAlleleG);
    if(pos == listOfPAlleleG.cend())
      listOfPAlleleG.push_back(pAlleleG);
    else
      (*pos)->addFrequency(pAlleleG->getFrequency());
  }

  return listOfPAlleleG;
}

std::vector<std::shared_ptr<Allele>> Allele8d::translateToG(){

  strVec_t codesInPrecision =  allelesToG();
  std::vector<std::shared_ptr<Allele>> listOfPAlleleG;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAlleleG = std::make_shared<AlleleG> (codeInPrecision, frequency);
    auto pos = pAlleleG->ispAlleleInList(listOfPAlleleG);
    if(pos == listOfPAlleleG.cend())
      listOfPAlleleG.push_back(pAlleleG);
    else
      (*pos)->addFrequency(pAlleleG->getFrequency());
  }

  return listOfPAlleleG;
}

std::vector<std::shared_ptr<Allele>> Allele4d::translateTo4d(){
  
  std::shared_ptr<Allele> pAllele4d = std::make_shared<Allele4d> (code, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele4d;
  listOfPAllele4d.push_back(pAllele4d);
  return listOfPAllele4d;
}

std::vector<std::shared_ptr<Allele>> Alleleg::translateTo4d(){

  strVec_t codesInPrecision = gToAlleles();
  std::vector<std::shared_ptr<Allele>> listOfPAllele4d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::string shorterNewCode = cutCodeKeepingLastLetter(codeInPrecision, 1);
    std::shared_ptr<Allele> pAllele4d = std::make_shared<Allele4d> (shorterNewCode, frequency);
    auto pos = pAllele4d->ispAlleleInList(listOfPAllele4d);
    if(pos == listOfPAllele4d.cend())
      listOfPAllele4d.push_back(pAllele4d);
    else
      (*pos)->addFrequency(pAllele4d->getFrequency());
  }

  return listOfPAllele4d;
}

std::vector<std::shared_ptr<Allele>> AlleleG::translateTo4d(){

  strVec_t codesInPrecision = GToAlleles();
  std::vector<std::shared_ptr<Allele>> listOfPAllele4d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::string shorterNewCode = cutCodeKeepingLastLetter(codeInPrecision, 1);
    std::shared_ptr<Allele> pAllele4d = std::make_shared<Allele4d> (shorterNewCode, frequency);
    auto pos = pAllele4d->ispAlleleInList(listOfPAllele4d);

    if(pos == listOfPAllele4d.cend())
      listOfPAllele4d.push_back(pAllele4d);
    else
      (*pos)->addFrequency(pAllele4d->getFrequency());
  }

  return listOfPAllele4d;
}

std::vector<std::shared_ptr<Allele>> Allele6d::translateTo4d(){

  std::string code4d = cutCodeKeepingLastLetter(code, 1);
  std::shared_ptr<Allele> pAllele4d = std::make_shared<Allele4d> (code4d, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele4d;
  listOfPAllele4d.push_back(pAllele4d);
  return listOfPAllele4d;
}

std::vector<std::shared_ptr<Allele>> Allele8d::translateTo4d(){

  std::string code4d = cutCodeKeepingLastLetter(code, 1);
  std::shared_ptr<Allele> pAllele4d = std::make_shared<Allele4d> (code4d, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele4d;
  listOfPAllele4d.push_back(pAllele4d);
  return listOfPAllele4d;
}

std::vector<std::shared_ptr<Allele>> Allele4d::translateTo6d(){

  strVec_t codesInPrecision = expandPrecision();
  std::vector<std::shared_ptr<Allele>> listOfPAllele6d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::string shorterNewCode = cutCodeKeepingLastLetter(codeInPrecision, 2);
    std::shared_ptr<Allele> pAllele6d = std::make_shared<Allele6d> (shorterNewCode, frequency);
    auto pos = pAllele6d->ispAlleleInList(listOfPAllele6d);

    if(pos == listOfPAllele6d.cend())
      listOfPAllele6d.push_back(pAllele6d);
    else
      (*pos)->addFrequency(pAllele6d->getFrequency());
  }

  return listOfPAllele6d;
}

std::vector<std::shared_ptr<Allele>> Alleleg::translateTo6d(){

  strVec_t codesInPrecision;
  strVec_t codesIn4d = gToAlleles();
  for(auto codeIn4d : codesIn4d){
    code = codeIn4d;
    strVec_t codesIn8d = expandPrecision();
    codesInPrecision.insert(codesInPrecision.end(),
			    codesIn8d.cbegin(),
			    codesIn8d.cend());
  }

  std::vector<std::shared_ptr<Allele>> listOfPAllele6d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::string shorterCode = cutCodeKeepingLastLetter(codeInPrecision, 2);
    std::shared_ptr<Allele> pAllele6d = std::make_shared<Allele6d> (shorterCode, frequency);
    auto pos = pAllele6d->ispAlleleInList(listOfPAllele6d);
    if(pos == listOfPAllele6d.cend())
      listOfPAllele6d.push_back(pAllele6d);
    else
      (*pos)->addFrequency(pAllele6d->getFrequency());
  }
  
  return listOfPAllele6d;
}
  
std::vector<std::shared_ptr<Allele>> AlleleG::translateTo6d(){

  strVec_t codesInPrecision = GToAlleles();
  std::vector<std::shared_ptr<Allele>> listOfPAllele6d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::string shorterNewCode = cutCodeKeepingLastLetter(codeInPrecision, 2);
    std::shared_ptr<Allele> pAllele6d = std::make_shared<Allele6d> (shorterNewCode, frequency);
    auto pos = pAllele6d->ispAlleleInList(listOfPAllele6d);
    if(pos == listOfPAllele6d.cend())
      listOfPAllele6d.push_back(pAllele6d);
    else
      (*pos)->addFrequency(pAllele6d->getFrequency());
  }

  return listOfPAllele6d;
}

std::vector<std::shared_ptr<Allele>> Allele6d::translateTo6d(){
  
  std::shared_ptr<Allele> pAllele6d = std::make_shared<Allele6d> (code, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele6d;
  listOfPAllele6d.push_back(pAllele6d);
  return listOfPAllele6d;
}

std::vector<std::shared_ptr<Allele>> Allele8d::translateTo6d(){

  std::string code6d = cutCodeKeepingLastLetter(code, 2);
  std::shared_ptr<Allele> pAllele6d = std::make_shared<Allele6d> (code6d, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele6d;
  listOfPAllele6d.push_back(pAllele6d);
  return listOfPAllele6d;
}

//


std::vector<std::shared_ptr<Allele>> Allele4d::translateTo8d(){

  strVec_t codesInPrecision = expandPrecision();
  std::vector<std::shared_ptr<Allele>> listOfPAllele8d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAllele8d = std::make_shared<Allele8d> (codeInPrecision, frequency);
    auto pos = pAllele8d->ispAlleleInList(listOfPAllele8d);
    if(pos == listOfPAllele8d.cend())
      listOfPAllele8d.push_back(pAllele8d);
    else
      (*pos)->addFrequency(pAllele8d->getFrequency());
  }

  return listOfPAllele8d;
}

std::vector<std::shared_ptr<Allele>> Alleleg::translateTo8d(){

  strVec_t codesInPrecision;
  strVec_t codesIn4d = gToAlleles();
  for(auto codeIn4d : codesIn4d){
    code = codeIn4d;
    strVec_t codesIn8d = expandPrecision();
    codesInPrecision.insert(codesInPrecision.end(),
			    codesIn8d.cbegin(),
			    codesIn8d.cend());
  }

  std::vector<std::shared_ptr<Allele>> listOfPAllele8d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAllele8d = std::make_shared<Allele8d> (codeInPrecision, frequency);
    auto pos = pAllele8d->ispAlleleInList(listOfPAllele8d);
    if(pos == listOfPAllele8d.cend())
      listOfPAllele8d.push_back(pAllele8d);
    else
      (*pos)->addFrequency(pAllele8d->getFrequency());
  }
  
  return listOfPAllele8d;
}
  
std::vector<std::shared_ptr<Allele>> AlleleG::translateTo8d(){

  strVec_t codesInPrecision;
  strVec_t codesIn6d = GToAlleles();
  for(auto codeIn6d : codesIn6d){
    code = codeIn6d;
    strVec_t codesIn8d = expandPrecision();
    codesInPrecision.insert(codesInPrecision.end(),
			    codesIn8d.cbegin(),
			    codesIn8d.cend());
  }

  std::vector<std::shared_ptr<Allele>> listOfPAllele8d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAllele8d = std::make_shared<Allele8d> (codeInPrecision, frequency);
    auto pos = pAllele8d->ispAlleleInList(listOfPAllele8d);
    if(pos == listOfPAllele8d.cend())
      listOfPAllele8d.push_back(pAllele8d);
    else
      (*pos)->addFrequency(pAllele8d->getFrequency());
  }
  
  return listOfPAllele8d;
}

std::vector<std::shared_ptr<Allele>> Allele6d::translateTo8d(){

  strVec_t codesInPrecision = expandPrecision();
  std::vector<std::shared_ptr<Allele>> listOfPAllele8d;
  frequency /= static_cast<double>(codesInPrecision.size());
  for(auto codeInPrecision : codesInPrecision){
    std::shared_ptr<Allele> pAllele8d = std::make_shared<Allele8d> (codeInPrecision, frequency);
    auto pos = pAllele8d->ispAlleleInList(listOfPAllele8d);
    if(pos == listOfPAllele8d.cend())
      listOfPAllele8d.push_back(pAllele8d);
    else
      (*pos)->addFrequency(pAllele8d->getFrequency());
  }
  
  return listOfPAllele8d;
}

std::vector<std::shared_ptr<Allele>> Allele8d::translateTo8d(){

  std::shared_ptr<Allele> pAllele8d = std::make_shared<Allele8d> (code, frequency);
  std::vector<std::shared_ptr<Allele>> listOfPAllele8d;
  listOfPAllele8d.push_back(pAllele8d);
  return listOfPAllele8d;
}
