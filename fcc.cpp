#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "fcc.h"

//create global variables
int counter = 0;
int wordChecker = 0;
int corrections = 0;
std::string fixedSentence = "";

/*
 * more_to_read
 *
 * checks a file to make sure there is still more data to read from
 *
 * input: a string named file
 * output: a boolean based off of if there is still morre data to read
 */
bool more_to_read(std::ifstream &file) {
  file >> std::noskipws;
  while (file.peek() == ' ') {
    char discard;
    file >> discard;
  }
  return !(file.eof() || std::iscntrl(file.peek()));
}

/*
 * compare_files
 *
 * compares the fixme and typo vectors and counts the words that match
 * adds the correct words to the fixedSentence variable
 *
 * input: 3 vectors (fixMeVector, typoVector, fixoVector) and a string variable (fixedSentence)
 * output: an int that counts the number of incorrect words
 */
int compare_files (std::vector<std::string> fixMeVector, std::vector<std::string> typoVector, std::vector<std::string> fixoVector, std::string &fixedSentence) {
  //reset the variables to 0 everytime the method is used
  counter = 0;
  wordChecker = 0;
  fixedSentence = "";
  
  //loop through the fix me vector
  for (auto fixMeWord : fixMeVector) {
    //loop through the typo vector
    for (int i = 0; i < typoVector.size(); i++) {
      //if the fix me word and the typo word are the same then execute the following statements
      if (fixMeWord == typoVector.at(i)) {
        //increase the autocorrect counter by 1
        counter++;
        //increase the word checker by 1 (used to know that a word was that point in the vector was incorrect)
        wordChecker++;
        //append the fixo word to the end of the fixed sentence string
        fixedSentence.append(fixoVector.at(i) + " ");
      }
    }
    //if the word was correct (wordchecker is not one) then execute the following statements
    if(wordChecker != 1) {
      //append the fixed word to the end of the fixed sentence
      fixedSentence.append(fixMeWord + " ");
    }
    //reset the word checker to zero
    wordChecker = 0;
  }
  //return the number of autocorrects
  return counter;
}

/*
 * loop_through_files
 *
 * loops through two files and adds the strings in each file to the vector arguments
 *
 * input: two input files (fixoFile and typoFile) and two vectors (fixoVector and typoVector)
 * output: doesn't return anything just adds the strings from the files to the given vectors
 */
void loop_through_files (std::ifstream &fixoFile, std::ifstream &typoFile, std::ifstream &fixMeFile, std::vector<std::string> fixoVector, std::vector<std::string> typoVector, std::vector<std::string> fixMeVector, int &corrections) {
  //while loop that runs until there aren't anymore words left in either fixo or typo file
  while (more_to_read(fixoFile) && more_to_read(typoFile))
  {
    //initialize the fixo and typo string variables
    std::string fixoString;
    std::string typoString;
    
    //take the next word in the files and store it into the string variables
    fixoFile >> fixoString;
    typoFile >> typoString;

    //add the strings to the vectors
    fixoVector.push_back(fixoString);
    typoVector.push_back(typoString);
  }
  //while loop that runs until there aren't anymore words left in the fixme file
  while (more_to_read(fixMeFile))
  {
    //initialize the fixme string variables
    std::string fixMeString;
    
    //take the next word in the file and store it into the string variable
    fixMeFile >> fixMeString;

    //add the string to the vector
    fixMeVector.push_back(fixMeString);
  }
  //save the number of autofixes to the corrections variable (done by using the compare files method)
  corrections = compare_files(fixMeVector, typoVector, fixoVector, fixedSentence);
}

/*
 * check_files
 *
 * checks both files to see if there was any text that was left over
 * and not read then appends the correct words to a string variable
 *
 * input: two input files (fixoFIle and typoFile)
 * output: if there was text left over then it returns true. If not then it returns false
 */
bool check_files (std::ifstream &fixoFile, std::ifstream &typoFile) {
  //if there was more text in one of the files then return -1
  if (more_to_read(fixoFile) || more_to_read(typoFile))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
 * remove_end_space
 *
 * takes in a string variable and removes the last character then returns the new string
 *
 * input: a string variable
 * output: the same string variable but with the last character removed
 */
std::string remove_end_space(std::string &string)
{
  //set the last character to the string length
  int lastCharacter = string.size() - 1;
  //while the last character is greater than or equal to zero and the last
  //character in the string is blank execute the following statement
  while (lastCharacter >= 0 && string[lastCharacter] == ' ')
    //subtract the last character by 1
    --lastCharacter;
  return string.substr(0, lastCharacter + 1);
}

/*
 * fcc
 *
 * takes in 4 files and constructs an improved sentence by fixing typos throughout
 * the old sentence. Also counts the number of typos throughout the old sentence
 *
 * input: 4 string variables (fixeme_filename, typo_filename, fixo_filename, and fixed_sentence)
 * output: and int based on how many typos there were
 */
int fcc(const std::string &fixme_filename, const std::string &typo_filename,
        const std::string &fixo_filename, std::string &fixed_sentence) {
  //initialize the fixo, typo, and fixme files
  std::ifstream fixo{};
  std::ifstream typo{};
  std::ifstream fixMe{};
  
  //open the three files with the filename arguments
  fixo.open(fixo_filename);
  typo.open(typo_filename);
  fixMe.open(fixme_filename);

  //initialize the fixo, typo, and fixme vectors
  std::vector<std::string> fixoWords;
  std::vector<std::string> typoWords;
  std::vector<std::string> fixMeWords;

  //loop through the files and add the words to the vectors
  loop_through_files(fixo, typo, fixMe, fixoWords, typoWords, fixMeWords, corrections);

  //remove the spaces at the end and add a period
  fixed_sentence = remove_end_space(fixedSentence) + ".";

  //check to see if there were unchecked words leftover in the files
  if (check_files(fixo, typo)) {
    //if there were more words to read then return -1
    return -1;
  }
  else {
    //if there were not any more words to read then return the vector size (the amount of words fixed)
    return corrections;
  }
}