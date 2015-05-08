/*
 *  multiline comments: _*__COMMENT
 *  
 *  Short informative file description
 */

/*
 *  import guards, never forget those!
 */
#ifndef _HEADER_HPP_ 
#define _HEADER_HPP_

/*
 *  "structural spacer" \/
 *  ==================================================#include==========================================================
 *
 *  includes, grouped in standard libraries and own headers
 */

#include <something>

#include "somethingOwn"

/*
 *  forward declarations
 */
class B;

/*  
 *  we agreed on not to use the using namespace std command, 
 *  since every file using this header would also use std as namespace
 *
 *  using namespace std; 
 */


/*
 *  camel casing everything from classes to variable names to functions
 */
class SomeExampleClass 
{

/*
 *  declarations in increasing visibilty order, starting with the private variables / functions up to the public ones
 */
private:

protected: 

public: 

/*
 *  ==================================================Setter============================================================
 *
 *  beyond this comment are setter functions for private / protected variables, 
 *  same for getter, (de-)construtor and other (utility) functions
 *  
 *  functions and variables have an indent of 4 spaces, do not use tabs as the tab width sometime varies
 */

/*
 * setter / getter should be a combination of the variable and the prefixes set / get
 */
    void setName(std::string);
    std::string getName();

};

#endif 
/*
 *  endif _HEADER_HPP_
 */