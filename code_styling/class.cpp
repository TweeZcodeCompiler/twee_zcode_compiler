/*
 *  multiline comments: _*__COMMENT
 *  
 *  Short informative file description
 */

/*
 *  "structural spacer" \/
 *  ==================================================#include==========================================================
 *
 *  includes, grouped in standard libraries and own headers
 *  
 *  seperate the included "main" header from the rest 
 */
#include <something>

#include "header.hpp"

#include "somethingOwn"

/*
 *  forward declarations
 */
class B;

/*
 *  ==================================================Constructor=======================================================
 *
 *  allways add the default constructor
 */
    SomeExampleClass::SomeExampleClass(int value) {}
    SomeExampleClass::SomeExampleClass(int value) 
    {
        this->value = value;
    }
/*
 *  order of declaration: constructor, deconstructor, getter, setter, util (to_string() e.g.)
 *
 *  ==================================================Setter============================================================
 *
 *  beyond this comment are setter functions for private / protected variables, 
 *  same for getter, (de-)construtor and other (utility) functions
 *  
 *  functions and variables have an indent of 4 blanks, do not use tabs as the tab width sometime varies
 *
 *  function / variable name in camel case, the name should reflect the behaviour / content of a function / variable
 */