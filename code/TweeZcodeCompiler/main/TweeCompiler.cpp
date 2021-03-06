//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include "exceptions.h"
#include "Utils.h"

#include <plog/Log.h>
#include <sstream>
#include <iostream>
#include <set>
#include <array>
#include <algorithm>

#include <Passage/Body/IBodyPartsVisitor.h>
#include <Passage/Body/Text.h>
#include <Passage/Body/Formatting.h>
#include <Passage/Body/Link.h>
#include <Passage/Body/Newline.h>

#include <Passage/Body/Expressions/BinaryOperation.h>
#include <Passage/Body/Expressions/UnaryOperation.h>

#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>

#include "Passage/Body/Expressions/Visited.h"
#include "Passage/Body/Expressions/Turns.h"
#include "Passage/Body/Expressions/Random.h"
#include "Passage/Body/Expressions/Previous.h"

#include <Passage/Body/Macros/SetMacro.h>
#include <Passage/Body/Macros/PrintMacro.h>
#include <Passage/Body/Macros/DisplayMacro.h>

#include <Passage/Body/Macros/IfMacro.h>
#include <Passage/Body/Macros/ElseIfMacro.h>
#include <Passage/Body/Macros/ElseMacro.h>
#include <Passage/Body/Macros/EndIfMacro.h>


using namespace std;
using namespace std::experimental;

static const string GLOB_PASSAGE = "PASSAGE_PTR",
        GLOB_PASSAGES_COUNT = "PASSAGES_COUNT",
        GLOB_TURNS_COUNT = "TURNS",
        LABEL_MAIN_LOOP = "MAIN_LOOP",
        LABEL_ROUTINE_FAIL_HANDLER = "failure",
        ROUTINE_MAIN = "main",
        ROUTINE_PASSAGE_FOR_STRING = "passage_id_for_string_link",
        GLOBAL_USER_INPUT = "USER_INPUT",
        TABLE_LINKED_PASSAGES = "LINKED_PASSAGES",
        TABLE_USERINPUT_LOOKUP = "USERINPUT_LOOKUP",
        TABLE_VISITED_PASSAGE_COUNT = "VISITED_PASSAGE_COUNT",
        ROUTINE_PASSAGE_BY_ID = "passage_by_id",
        ROUTINE_NAME_FOR_PASSAGE = "print_name_for_passage",
        ROUTINE_DISPLAY_LINKS = "display_links",
        ELSE_LABEL_PREFIX = "ELSE",
        ENDIF_LABEL_PREFIX = "ENDIF",
        ROUTINE_CLEAR_TABLES = "reset_tables",
        TEXT_FORMAT_ROUTINE = "toggleTextFormat",
        TEXT_FORMAT_ITALIC = "ITALIC",
        TEXT_FORMAT_BOLD = "BOLD",
        TEXT_FORMAT_REVERSE_VIDEO = "REVERSED_VIDEO",
        TEXT_FORMAT_FIXED_PITCH = "FIXED_PITCH",
        START_PASSAGE_NAME = "Start",
        GLOB_PREVIOUS_PASSAGE_ID = "PREVIOUS_PASSAGE_ID",
        GLOB_CURRENT_PASSAGE_ID = "CURRENT_PASSAGE_ID",
        STACK_POINTER = "sp",
        LINK_PREFIX = "LINK_", STRING_PREVIOUS = "PREVIOUS",

// mouse vars, routines & tables
// mouse link table: one link needs 4 entries: lineNumber, charStart, charEnd, passageNumber
// mouse arrow table: bounds are only entered if arrow is displayed.
//                    One arrow needs 4 entries: lineTop, charLeftTop, lineBottom, charLeftBottom

        MOUSE_CLICK_ROUTINE = "mouseClick",
        TABLE_CURSOR = "TABLE_CURSOR",
        TABLE_MOUSE_CLICK = "TABLE_MOUSE_CLICK",
        GLOB_INTERPRETER_SUPPORTS_MOUSE = "INTERPRETER_SUPPORTS_MOUSE",
        SUPPORTS_MOUSE_ROUTINE = "SUPPORTS_MOUSE_ROUTINE",
        UPDATE_MOUSE_SCREEN_ROUTINE = "UPDATE_MOUSE_SCREEN_ROUTINE",
        PRINT_SPACES_ROUTINE = "PRINT_SPACES_ROUTINE",
        TABLE_MOUSE_LINKS = "MOUSE_LINKS",
        GLOB_TABLE_MOUSE_LINKS_NEXT = "MOUSE_LINKS_NEXT",
        UPDATE_MOUSE_TABLE_BEFORE_ROUTINE = "UPDATE_MOUSE_TABLE_BEFORE_ROUTINE",
        UPDATE_MOUSE_TABLE_AFTER_ROUTINE = "UPDATE_MOUSE_TABLE_AFTER_ROUTINE",
        CLEAR_MOUSE_TABLE_ROUTINE = "CLEAR_MOUSE_TABLE_ROUTINE",
        GLOB_LINES_PRINTED = "LINES_PRINTED",
        PAUSE_PRINTING_ROUTINE = "PAUSE_PRINTING_ROUTINE",
        GLOB_NEXT_PASSAGE_ID = "NEXT_PASSAGE_ID",
        ROUTINE_PRINT_ARROW_UP = "PRINT_ARROW_UP",
        ROUTINE_PRINT_ARROW_DOWN = "PRINT_ARROW_DOWN",
        GLOB_PRINT_ARROW_UP_AT_END = "PRINT_ARROW_UP_AT_END",
        TABLE_MOUSE_CLICK_ARROWS = "MOUSE_CLICK_ARROWS",
        INIT_ROUTINE = "INIT_ROUTINE";

static const unsigned int ZSCII_NUM_OFFSET = 49;
static const unsigned int MAX_MOUSE_LINKS = 20;
static const unsigned int MARGIN_LEFT = 20;
static const unsigned int MARGIN_RIGHT = 10;
static const unsigned int MARGIN_TOP = 4;      // at least 4 lines to leave place for up arrow
static const unsigned int MARGIN_BOTTOM = 4;   // at least 4 lines to leave place for down arrow
static const string INTRO_TITLE = "C++ Compiler, built at FU Berlin";
static const string INTRO_MOUSE = "This interpreter supports mouse control. Click on links to go to the next passage and use the arrow keys on the right side of the screen to navigate in one passage.";
static const string INTRO_NO_MOUSE = "Unfortunately this interpreter does not support mouse control. Links are displayed at the end of a passage and can be chosen via your keyboard.";
static const string INTRO_NEXT_MOUSE = "Click somewhere to start the story. Have fun and good luck.";
static const string INTRO_NEXT_NO_MOUSE = "Press a key to start the story. Have fun and good luck.";

static const unsigned int LINKED_PASSAGES_SIZE = 512;

//#define ZAS_DEBUG

#ifndef ASSGEN
#define ASSGEN (*_assgen)
#endif


bool isPreviousMacro(string link) {
    // TODO: catch invalid link

    string previousFunc = "previous()";

    while (link.size() > previousFunc.size() && link.at(0) == ' ') {
        link = link.substr(1);
    }
    while (link.size() > previousFunc.size() && link.at(link.size() - 1) == ' ') {
        link = link.substr(0, link.size() - 1);
    }

    return link == previousFunc;
}

void maskString(std::string &string) {
    std::replace(string.begin(), string.end(), ' ', '_');
}

string routineNameForPassageName(std::string passageName) {
    stringstream ss;
    maskString(passageName);
    ss << "R_" << passageName;
    return ss.str();
}


string routineNameForPassage(const Passage &passage) {
    return routineNameForPassageName(passage.getHead().getName());
}

string labelForPassageName(string passageName) {
    stringstream ss;
    maskString(passageName);
    ss << "L_" << passageName;
    return ss.str();
}

string labelForPassage(Passage &passage) {
    return labelForPassageName(passage.getHead().getName());
}

string makeIfCaseLabel(const IfContext &context) {
    return ELSE_LABEL_PREFIX + to_string(context.number) + "_" + to_string(context.caseCount);
}

string makeIfEndLabel(const IfContext &context) {
    return ENDIF_LABEL_PREFIX + to_string(context.number);
}


string makeUserInputRoutine() {
    stringstream ss;

    ss << ".FUNCT display_links i, selectcount, linkedcount\n"
            "   call_vs __stack_count LINKED_PASSAGES " << to_string(LINKED_PASSAGES_SIZE) << " -> linkedcount\n"
            "   store i 0\n"
            "   jz linkedcount ?~loop_start\n"
            "   ret -1\n"
            "loop_start:\n"
            "   add selectcount 1 -> sp\n"
            "   add selectcount 49 -> sp\n"
            "   je INTERPRETER_SUPPORTS_MOUSE 1 ?mouse_is_supported\n"
            "   print_char sp\n"
            "   print \": \"\n"
            "   call_vs __stack_at LINKED_PASSAGES " << to_string(LINKED_PASSAGES_SIZE) << " i -> sp\n"
            "   print_addr sp\n"
            "   new_line\n"
            "   mouse_is_supported:\n"
            "   add selectcount 1 -> selectcount\n"
            "   add i 1 -> i\n"
            "   jl i linkedcount ?loop_start\n"
            "\n"
            "    je INTERPRETER_SUPPORTS_MOUSE 1 ?mouse_supported\n"
            "read_loop:"
            "    read_char 1 -> USER_INPUT\n"
            "    sub USER_INPUT 48 -> USER_INPUT\n"
            "    sub USER_INPUT 1 -> USER_INPUT\n"
            "   jg USER_INPUT linkedcount ?read_loop\n"
            "   jl USER_INPUT 0 ?read_loop\n"
            "   call_vs __stack_at LINKED_PASSAGES " << to_string(LINKED_PASSAGES_SIZE) << " USER_INPUT -> sp\n"
            "   call_vs passage_id_for_string_link sp -> sp\n"
            "   pop_stack linkedcount LINKED_PASSAGES\n"
            "    jump ?fetch_userinput_lookup\n"
            "mouse_supported:\n"
            "    push PRINT_ARROW_UP_AT_END\n"
            "    call_vs mouseClick sp -> sp\n"
            "fetch_userinput_lookup:\n"
            "\n"
            "   add TURNS 1 -> TURNS\n"
            "\n"
            "\n"
            "   ret sp";

    return ss.str();
}

string makeClearTablesRoutine() {
    return ".FUNCT reset_tables i\n"
            "   loop_start:\n"
            "   storeb USERINPUT_LOOKUP i 0\n"
            "   add i 1 -> i\n"
            "   jl i PASSAGES_COUNT ?loop_start\n"
            "   ret 0"
            "\n";
}


void TweeCompiler::compile(TweeFile &tweeFile, std::ostream &out) {
    _assgen = unique_ptr<ZAssemblyGenerator>(new ZAssemblyGenerator(out));
    vector<Passage> passages = tweeFile.getPassages();
    globalVariables = std::set<std::string>();
    labelCount = 0;
    linkCount = 0;
    foundLinks = vector<Link>();
    ifContexts = stack<IfContext>();
    ifCount = 0;

    {
        //check if all passage names are unique and there is a start passage "Start"
        set<string> passageNames;
        bool hasStart = false;

        for (auto passage =  passages.begin(); passage!=passages.end(); ++passage) {
            if (!hasStart) {
                hasStart = (passage->getHead().getName() == "Start");
            }
            if(passageNames.find(passage->getHead().getName()) == passageNames.end()) {
                passageNames.insert(passage->getHead().getName());
            } else {
                throw TweeDocumentException("Duplicate passage name: " + passage->getHead().getName());
            }
        }

        if (!hasStart) {
            throw TweeDocumentException("Twee document doesn't contain a start passage: ::Start");
        }

    }

    {
        int i = 0;
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            passageName2id[passage->getHead().getName()] = (i++);
        }
    }
    // tables needed for routine linking
    ASSGEN.addWordArray(TABLE_LINKED_PASSAGES, LINKED_PASSAGES_SIZE);
    ASSGEN.addByteArray(TABLE_USERINPUT_LOOKUP, (unsigned) passages.size()); // todo: check if this is still needed

    ASSGEN.addWordArray(TABLE_VISITED_PASSAGE_COUNT, (unsigned) passages.size());
    ASSGEN.addWordArray(TABLE_CURSOR, 8);
    ASSGEN.addWordArray(TABLE_MOUSE_CLICK, 20);


    ASSGEN.addWordArray(TABLE_MOUSE_LINKS, MAX_MOUSE_LINKS * 4 * 5); // maximum of MAX_MOUSE_LINKS links per page of a passage possible
    ASSGEN.addWordArray(TABLE_MOUSE_CLICK_ARROWS, 2 * 4);


    // special string for previous functionality
    ASSGEN.addString(STRING_PREVIOUS, "previous");



    // globals
    ASSGEN.addGlobal(GLOB_PASSAGE)
            .addGlobal(GLOBAL_USER_INPUT)
            .addGlobal(GLOB_PASSAGES_COUNT)
            .addGlobal(GLOB_PREVIOUS_PASSAGE_ID)
            .addGlobal(GLOB_CURRENT_PASSAGE_ID)
            .addGlobal(GLOB_TURNS_COUNT)
            .addGlobal(GLOB_INTERPRETER_SUPPORTS_MOUSE)
            .addGlobal(GLOB_TABLE_MOUSE_LINKS_NEXT)
            .addGlobal(GLOB_LINES_PRINTED)
            .addGlobal(GLOB_NEXT_PASSAGE_ID)
            .addGlobal(GLOB_PRINT_ARROW_UP_AT_END);


    // main routine
    {
        static const string LOCAL_NEXT_PASSAGE = "next_passage";

        // call start routine first
        ASSGEN.addRoutine(ROUTINE_MAIN, vector<ZRoutineArgument>({ZRoutineArgument(LOCAL_NEXT_PASSAGE)}))
                .storew(TABLE_LINKED_PASSAGES, "0", to_string(LINKED_PASSAGES_SIZE-1))
                .markStart()
                .store(GLOB_PASSAGES_COUNT, to_string(passages.size()))
                .store(GLOB_TURNS_COUNT, "1")
                .store(GLOB_PREVIOUS_PASSAGE_ID, "0")
                .store(GLOB_CURRENT_PASSAGE_ID, "0")
                .call_1n(SUPPORTS_MOUSE_ROUTINE)
                .call_1n(ROUTINE_CLEAR_TABLES)
                .store(GLOB_NEXT_PASSAGE_ID, "-1")
                .setCursor("-1", "0", "0")           // hide cursor
                .call_1n(INIT_ROUTINE)
                .call_vn(ROUTINE_PASSAGE_BY_ID, to_string(passageName2id.at(string(START_PASSAGE_NAME))));

        ASSGEN.addLabel(LABEL_MAIN_LOOP)
                .jumpEquals(GLOB_NEXT_PASSAGE_ID + " -1", "displayLinks")
                .store(LOCAL_NEXT_PASSAGE, GLOB_NEXT_PASSAGE_ID)
                .store(GLOB_NEXT_PASSAGE_ID, "-1")
                .jump("callRoutines")
                .addLabel("displayLinks")
                .call_vs(ROUTINE_DISPLAY_LINKS, nullopt, LOCAL_NEXT_PASSAGE)
                .addLabel("callRoutines")
                .jumpLower(ASSGEN.makeArgs({LOCAL_NEXT_PASSAGE, "0"}), "end_program")
                .call_1n(ROUTINE_CLEAR_TABLES)
                .call_vn(ROUTINE_PASSAGE_BY_ID, LOCAL_NEXT_PASSAGE)
                .jump(LABEL_MAIN_LOOP);

        ASSGEN.addLabel("end_program");
        ASSGEN.quit();
    }

    out << makeUserInputRoutine() << endl << makeClearTablesRoutine() << endl;


    {
        string varI = "i", varKey = "key";

        ASSGEN.addRoutine(INIT_ROUTINE, {ZRoutineArgument(varI), ZRoutineArgument(varKey)})
                .setMargins(to_string(MARGIN_LEFT), to_string(MARGIN_RIGHT), "0")

                        // add top margin
                .addLabel("loop")
                .newline()
                .add(varI, "1", varI)
                .jumpLess(varI + " " + to_string(MARGIN_TOP), "loop")
                .windowStyle("0", "15", "1")

                .setTextStyle("6")
                .setTextStyle("2")
                .print(INTRO_TITLE)
                .setTextStyle("0")
                .newline()
                .newline()

                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 1", "mouseSupported")
                .print(INTRO_NO_MOUSE)
                .newline()
                .newline()
                .newline()
                .print(INTRO_NEXT_NO_MOUSE)
                .read_char(varKey)
                .setMargins("0", "0", "0")
                .eraseWindow("0")
                .ret("0");

        ASSGEN.addLabel("mouseSupported")
                .print(INTRO_MOUSE)
                .newline()
                .newline()
                .newline()
                .print(INTRO_NEXT_MOUSE)
                .mouseWindow("-1")
                .read_char(varKey)
                .mouseWindow("1")
                .ret("0");
    }

    // call passage by id routine
    {
        const string idLocal = "id";
        const string passageCount = "passage_count";
        ASSGEN.addRoutine(ROUTINE_PASSAGE_BY_ID, vector<ZRoutineArgument>({ZRoutineArgument(idLocal), ZRoutineArgument(passageCount)}))
                .store(GLOB_PREVIOUS_PASSAGE_ID, GLOB_CURRENT_PASSAGE_ID)
                .store(GLOB_CURRENT_PASSAGE_ID, idLocal);

        // update visited array
        ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, GLOB_CURRENT_PASSAGE_ID, passageCount)
                .add(passageCount, "1", passageCount)
                .storew(TABLE_VISITED_PASSAGE_COUNT, GLOB_CURRENT_PASSAGE_ID, passageCount);

        for (auto it = passages.begin(); it != passages.end(); ++it) {
            string passageName = it->getHead().getName();
            unsigned id = passageName2id.at(passageName);
            ASSGEN.jumpEquals(ASSGEN.makeArgs({idLocal, to_string(id)}), labelForPassage(*it));
        }

        // error case
        ASSGEN.print("invalid id for passage: ")
                .print_num(idLocal)
                .quit();

        for (auto it = passages.begin(); it != passages.end(); ++it) {
            ASSGEN.addLabel(labelForPassage(*it)).call_vs(routineNameForPassage(*it), nullopt, STACK_POINTER).ret("0");
        }
    }


    // print passage name by id routine
    {
        const string idLocal = "id";
        ASSGEN.addRoutine(ROUTINE_NAME_FOR_PASSAGE, vector<ZRoutineArgument>({ZRoutineArgument(idLocal)}));
        for (auto it = passages.begin(); it != passages.end(); ++it) {
            string passageName = it->getHead().getName();
            unsigned id = passageName2id.at(passageName);
            ASSGEN.jumpEquals(ASSGEN.makeArgs({idLocal, to_string(id)}), labelForPassage(*it));
        }

        // error case
        ASSGEN.print("invalid id for passage: ")
                .print_num(idLocal)
                .quit();

        for (auto it = passages.begin(); it != passages.end(); ++it) {
            ASSGEN.addLabel(labelForPassage(*it)).print(it->getHead().getName()).ret("0");
        }
    }

    // mouse control routines
    {
        // mouse click routine

        string varKey = "key", varXMouse = "xMouse", varYLineClick = "yMouseLineClick", varXMouseClick = "xMouseClick", varYMouse = "yMouse", varFontSize = "fontSize";
        string varClickArrows = "clickArrows", varFirstLinkLine = "firstLinkLine", varMouseTableIndex = "mouseTableIndex", varI = "i", varCharWidth = "charWidth";

        vector<ZRoutineArgument> args;
        args.push_back(ZRoutineArgument(varClickArrows));   // needs to be set by call command
        args.push_back(ZRoutineArgument(varKey));
        args.push_back(ZRoutineArgument(varXMouse));
        args.push_back(ZRoutineArgument(varYMouse));
        args.push_back(ZRoutineArgument(varYLineClick));
        args.push_back(ZRoutineArgument(varXMouseClick));
        args.push_back(ZRoutineArgument(varFontSize));
        args.push_back(ZRoutineArgument(varCharWidth));
        args.push_back(ZRoutineArgument(varFirstLinkLine));
        args.push_back(ZRoutineArgument(varMouseTableIndex));
        args.push_back(ZRoutineArgument(varI));
        ASSGEN.addRoutine(MOUSE_CLICK_ROUTINE, args);

        ASSGEN.getWindowProperty("0", "13", varFontSize)
                .div(varFontSize, "256", varFontSize);

        ASSGEN.loadb("33", 0, STACK_POINTER)                                                // max chars per line
                .getWindowProperty("0", "3", STACK_POINTER)                                 // window width in pixels
                .sub(STACK_POINTER, to_string(MARGIN_LEFT), STACK_POINTER)
                .sub(STACK_POINTER, to_string(MARGIN_RIGHT), STACK_POINTER)
                .div(STACK_POINTER, STACK_POINTER, varCharWidth)
                .add(varCharWidth, "1", varCharWidth);

        ASSGEN.addLabel("MOUSE_CLICK_LOOP")
                .mouseWindow("-1")
                .read_char(varKey)
                .mouseWindow("1");

        ASSGEN.readMouse(TABLE_MOUSE_CLICK)
                .loadw(TABLE_MOUSE_CLICK, "0", varYMouse)
                .loadw(TABLE_MOUSE_CLICK, "1", varXMouse);

        ASSGEN.div(varYMouse, varFontSize, varYLineClick)
                .add(varYLineClick, "1", varYLineClick)
                .div(varXMouse, varCharWidth, varXMouseClick);

        ASSGEN.store(varMouseTableIndex, "1");
        ASSGEN.addLabel("LINKS_ENTRIES")
                .loadw(TABLE_MOUSE_LINKS, varMouseTableIndex, varI)
                .jumpGreaterEquals(varMouseTableIndex + " " + GLOB_TABLE_MOUSE_LINKS_NEXT, "ALL_ENTRIES_CHECKED")    // there are no more links saved in table
                .jumpGreater(varMouseTableIndex + " " + to_string(MAX_MOUSE_LINKS * 4), "ALL_ENTRIES_CHECKED")       // all 20 links are checked
                .jumpEquals(varYLineClick + " " + varI, "CORRECT_LINE")
                .add(varMouseTableIndex, "4", varMouseTableIndex)
                .jump("LINKS_ENTRIES");

        ASSGEN.addLabel("CORRECT_LINE")
                .add(varMouseTableIndex, "1", varMouseTableIndex)
                .loadw(TABLE_MOUSE_LINKS, varMouseTableIndex, varI)
                .jumpGreaterEquals(varXMouseClick + " " + varI, "CORRECT_X_START")
                .add(varMouseTableIndex, "3", varMouseTableIndex)
                .jump("LINKS_ENTRIES");

        ASSGEN.addLabel("CORRECT_X_START")
                .add(varMouseTableIndex, "1", varMouseTableIndex)
                .loadw(TABLE_MOUSE_LINKS, varMouseTableIndex, varI)
                .jumpLess(varXMouseClick + " " + varI, "CORRECT_X_ENDING")
                .add(varMouseTableIndex, "2", varMouseTableIndex)
                .jump("LINKS_ENTRIES");

        ASSGEN.addLabel("CORRECT_X_ENDING")
                .add(varMouseTableIndex, "1", varMouseTableIndex)
                .loadw(TABLE_MOUSE_LINKS, varMouseTableIndex, varI)
                .ret(varI);

        ASSGEN.addLabel("ALL_ENTRIES_CHECKED")
                .jumpEquals(varClickArrows + " 0", "MOUSE_CLICK_LOOP")   // are arrows active?
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "1", STACK_POINTER)              // test here with x value because arrows are always drawn on right sight
                .jumpEquals("sp 0", "ARROW_DOWN")

                .loadw(TABLE_MOUSE_CLICK_ARROWS, "0", STACK_POINTER)       // lineTop
                .jumpLess(varYLineClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "1", STACK_POINTER)       // width left
                .jumpLess(varXMouseClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "2", STACK_POINTER)       // line bottom
                .jumpGreater(varYLineClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "3", STACK_POINTER)       // width right
                .jumpGreater(varXMouseClick + " sp", "ARROW_DOWN")
                .restoreUndo(STACK_POINTER);                               // restores game to start this passage again

        ASSGEN.addLabel("ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "5", STACK_POINTER)
                .jumpEquals("sp 0", "MOUSE_CLICK_LOOP")

                .loadw(TABLE_MOUSE_CLICK_ARROWS, "4", STACK_POINTER)              // lineTop
                .jumpLess(varYLineClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "5", STACK_POINTER)              // width left
                .jumpLess(varXMouseClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "6", STACK_POINTER)              // line bottom
                .jumpGreater(varYLineClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "7", STACK_POINTER)              // width right
                .jumpGreater(varXMouseClick + " sp", "MOUSE_CLICK_LOOP")
                .ret("-1");                                              // signals that no link was clicked


        // print arrow up routine

        string varLineCount = "line_count";

        ASSGEN.addRoutine(ROUTINE_PRINT_ARROW_UP, {ZRoutineArgument(varCharWidth), ZRoutineArgument(varLineCount)});

        ASSGEN.loadb("33", "0", varCharWidth)
                .sub(varCharWidth, "4", varCharWidth)
                .loadb("32", "0", varLineCount)
                .sub(varLineCount, "4", varLineCount);

        ASSGEN.setWindow("1")
                .newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", STACK_POINTER) // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "0", STACK_POINTER)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "1", STACK_POINTER)
                .print("/ \\ ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", STACK_POINTER) // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "2", STACK_POINTER)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "3", STACK_POINTER);

        ASSGEN.setWindow("0")
                .ret("0");


        // print arrow down routine

        string varLinesPrinted = "lines_printed";

        ASSGEN.addRoutine(ROUTINE_PRINT_ARROW_DOWN, {ZRoutineArgument(varCharWidth), ZRoutineArgument(varLinesPrinted),
                                                     ZRoutineArgument(varLineCount)});

        ASSGEN.loadb("33", "0", varCharWidth)
                .sub(varCharWidth, "4", varCharWidth)
                .loadb("32", "0", varLineCount)
                .sub(varLineCount, "4", varLineCount);

        ASSGEN.setWindow("1")
                .addLabel("loop")
                .newline()
                .add(varLinesPrinted, "1", varLinesPrinted)
                .jumpLess(varLinesPrinted + " " + varLineCount, "loop");

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", STACK_POINTER) // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "4", STACK_POINTER)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "5", STACK_POINTER)
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" V  ")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", STACK_POINTER) // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "6", STACK_POINTER)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "7", STACK_POINTER);

        ASSGEN.setWindow("0")
                .ret("0");


        // support mouse routine

        ASSGEN.addRoutine(SUPPORTS_MOUSE_ROUTINE);

        ASSGEN.loadb("23", "0", STACK_POINTER)  // flags in header, bit 5 needs to be true
                .lor(STACK_POINTER, "11011111", STACK_POINTER)
                .jumpZero(STACK_POINTER, "NOT_SUPPORTED");


        ASSGEN.loadb("50", "0", STACK_POINTER)  // revision number first part, needs to be 1
                .land(STACK_POINTER, "00000001", STACK_POINTER)
                .jumpZero(STACK_POINTER, "NOT_SUPPORTED");


        ASSGEN.loadb("50", "1", STACK_POINTER)  // revision number second part, needs to be 1
                .land(STACK_POINTER, "00000001", STACK_POINTER)
                .jumpZero(STACK_POINTER, "NOT_SUPPORTED");

        ASSGEN.store(GLOB_INTERPRETER_SUPPORTS_MOUSE, "1")
                .ret("0");

        ASSGEN.addLabel("NOT_SUPPORTED")
                .store(GLOB_INTERPRETER_SUPPORTS_MOUSE, "0")
                .ret("0");

        // update screen routine

        string varYSize = "ySize", varXSize = "xSize";

        ASSGEN.addRoutine(UPDATE_MOUSE_SCREEN_ROUTINE, {ZRoutineArgument(varYSize), ZRoutineArgument(varXSize), ZRoutineArgument(varI)});

        ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "no_mouse");

        ASSGEN.eraseWindow("0")
                .eraseWindow("1")
                .store(GLOB_TABLE_MOUSE_LINKS_NEXT, "1")
                .store(GLOB_LINES_PRINTED, "1")
                .call_1n(CLEAR_MOUSE_TABLE_ROUTINE);

        /*ASSGEN.loadb("33", "0", varCharWidth)
                .sub(varCharWidth, "4", varCharWidth)
                .loadb("32", "0", varLineCount)
                .sub(varLineCount, "4", varLineCount);*/

        ASSGEN.getWindowProperty("0", "2", varYSize)
                .getWindowProperty("0", "3", varXSize)
                .windowStyle("0", "15", "1")
                .windowStyle("1", "15", "1")
                .putWindowProperty("0", "15", "-999")    // set lineCount to -999 -> interpreter will not show [[MORE]]
                .putWindowProperty("1", "15", "-999");

        ASSGEN.windowSize("1", varYSize, varXSize)
                .setMargins(to_string(MARGIN_LEFT), to_string(MARGIN_RIGHT), "0");

        ASSGEN.addLabel("loop")
                .newline()
                .add(varI, "1", varI)
                .jumpLess(varI + " " + to_string(MARGIN_TOP), "loop")
                .add(GLOB_LINES_PRINTED, varI, GLOB_LINES_PRINTED);


        ASSGEN.addLabel("no_mouse")
                .ret("0");

        // UPDATE_MOUSE_TABLE_BEFORE_ROUTINE

        ASSGEN.addRoutine(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE)
                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "no_mouse")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", STACK_POINTER) // line number of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, STACK_POINTER)
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, STACK_POINTER)
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .addLabel("no_mouse")
                .ret("0");

        // UPDATE_MOUSE_TABLE_AFTER_ROUTINE

        string varId = "passage_id";

        ASSGEN.addRoutine(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, {ZRoutineArgument(varId)})
                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "no_mouse")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "1", STACK_POINTER) // x position of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, STACK_POINTER)
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, varId)
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .addLabel("no_mouse")
                .ret("0");

        // clear mouse table routine

        ASSGEN.addRoutine(CLEAR_MOUSE_TABLE_ROUTINE, {ZRoutineArgument("i")})
                .addLabel("loop")
                .storew(TABLE_MOUSE_LINKS, "i", "0")
                .add("i", "1", "i")
                .jumpLess("i " + to_string(MAX_MOUSE_LINKS * 4), "loop");

        ASSGEN.store("i", "0")
                .addLabel("loop2")
                .storew(TABLE_MOUSE_CLICK_ARROWS, "i", "0")
                .add("i", "1", "i")
                .jumpLess("i 8", "loop2")
                .ret("0");

        // PAUSE_PRINTING_ROUTINE

        ASSGEN.addRoutine(PAUSE_PRINTING_ROUTINE, {ZRoutineArgument("result")})
                .add(GLOB_LINES_PRINTED, "1", GLOB_LINES_PRINTED)
                .loadb("32", 0, STACK_POINTER)
                .sub(STACK_POINTER, to_string(MARGIN_BOTTOM), STACK_POINTER)
                .jumpLess(GLOB_LINES_PRINTED + " sp", "end")             // jump if last line of screen is not reached
                .store(GLOB_PRINT_ARROW_UP_AT_END, "1")
                .call_1n(ROUTINE_PRINT_ARROW_DOWN)
                .call_vs(MOUSE_CLICK_ROUTINE, to_string(1), "result")
                .jumpLess("result 0", "noLinkClicked")                   // jump if no link clicked by mouse
                .store(GLOB_NEXT_PASSAGE_ID, "result")
                .jump("end")
                .addLabel("noLinkClicked")
                .call_1n(UPDATE_MOUSE_SCREEN_ROUTINE)
                .addLabel("end")
                .ret("0");

        // helper routine

        ASSGEN.addRoutine(PRINT_SPACES_ROUTINE, {ZRoutineArgument("count"), ZRoutineArgument("i")})
                .addLabel("loop")
                .print(" ")
                .add("i", "1", "i")
                .jumpLess("i count", "loop")
                .ret("0");
    }

    // print appropriate text formatting args
    {
        ASSGEN.addGlobal(TEXT_FORMAT_REVERSE_VIDEO)
                .addGlobal(TEXT_FORMAT_BOLD)
                .addGlobal(TEXT_FORMAT_ITALIC)
                .addGlobal(TEXT_FORMAT_FIXED_PITCH);

        string varFormatType = "formatType";
        string varCounter = "counter";
        string varResult = "result";
        string labelTextStyle = "CALC_TEXT_STYLE";
        string labelNotZero = "NOT_ZERO";
        string labelZeroOn = "ZERO_ON";
        string labelNotOne = "NOT_ONE";
        string labelOneOn = "One_ON";
        string labelNotTwo = "NOT_Two";
        string labelTwoOn = "Two_ON";
        string labelThreeOn = "THREE_ON";

        vector<ZRoutineArgument> args;
        args.push_back(
                ZRoutineArgument(varFormatType));    // This value will be set via call_vs TEXT_FORMAT_ROUTINE 1 -> sp
        args.push_back(ZRoutineArgument(varCounter));
        args.push_back(ZRoutineArgument(varResult));
        ASSGEN.addRoutine(TEXT_FORMAT_ROUTINE, args);

        ASSGEN.jumpEquals(string(varFormatType + " 0"), string("~" + labelNotZero))
                .jumpEquals(string(TEXT_FORMAT_REVERSE_VIDEO) + " 0", labelZeroOn)
                .store(TEXT_FORMAT_REVERSE_VIDEO, "0")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelZeroOn)
                .store(TEXT_FORMAT_REVERSE_VIDEO, "1")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelNotZero)
                .jumpEquals(string(varFormatType + " 1"), string("~" + labelNotOne))
                .jumpEquals(string(TEXT_FORMAT_BOLD) + " 0", labelOneOn)
                .store(TEXT_FORMAT_BOLD, "0")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelOneOn)
                .store(TEXT_FORMAT_BOLD, "1")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelNotOne)
                .jumpEquals(string(varFormatType + " 2"), string("~" + labelNotTwo))
                .jumpEquals(string(TEXT_FORMAT_ITALIC) + " 0", labelTwoOn)
                .store(TEXT_FORMAT_ITALIC, "0")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelTwoOn)
                .store(TEXT_FORMAT_ITALIC, "1")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelNotTwo)
                .jumpEquals(string(TEXT_FORMAT_FIXED_PITCH) + " 0", labelThreeOn)
                .store(TEXT_FORMAT_FIXED_PITCH, "0")
                .jump(labelTextStyle);

        ASSGEN.addLabel(labelThreeOn)
                .store(TEXT_FORMAT_FIXED_PITCH, "1");

        ASSGEN.addLabel(labelTextStyle)
                .mul(TEXT_FORMAT_REVERSE_VIDEO, "1", varResult)
                .mul(TEXT_FORMAT_BOLD, "2", varCounter)
                .add(varCounter, varResult, varResult)
                .mul(TEXT_FORMAT_ITALIC, "4", varCounter)
                .add(varCounter, varResult, varResult)
                .mul(TEXT_FORMAT_FIXED_PITCH, "8", varCounter)
                .add(varCounter, varResult, varResult)
                .setTextStyle(varResult)
                .ret("0");

    }

    // passage routines
    for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
        makePassageRoutine(*passage);
    }

    // label for link string routine
    {
        static const string STRING_ARG_NAME = "thestring";
        static const string linkLabelPrefix = "L_";
        ASSGEN.addRoutine(ROUTINE_PASSAGE_FOR_STRING, vector<ZRoutineArgument>{ ZRoutineArgument(STRING_ARG_NAME) });

        int i=0;
        for(auto link = foundLinks.begin(); link != foundLinks.end(); ++link) {
            string linkName = LINK_PREFIX + to_string(i);

            ASSGEN.jumpEquals(ASSGEN.makeArgs({STRING_ARG_NAME, linkName}), linkLabelPrefix + linkName);
            i++;
        };

        // error handling
        ASSGEN.print("could not find target passage for string ")
                .printAddr(STRING_ARG_NAME)
                .print(" at address ")
                .print_num(STRING_ARG_NAME);

        i=0;
        for(auto link = foundLinks.begin(); link != foundLinks.end(); ++link) {
            string targetId = isPreviousMacro(link->getTarget()) ? GLOB_PREVIOUS_PASSAGE_ID : to_string(passageName2id.at(link->getTarget()));
            string linkName = LINK_PREFIX + to_string(i);
            ASSGEN.addLabel(linkLabelPrefix + linkName);

            ASSGEN.ret(targetId);
            i++;
        }
    }

    out << Utils::getStackLib();
}


void TweeCompiler::visit(const Text& host) {
    ASSGEN.print(host.getContent());
}
void TweeCompiler::visit(const Formatting& host) {
    switch (host.getFormat()) {
        case Format::UNDERLINED:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("0"), STACK_POINTER);
            break;
        case Format::BOLD:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("1"), STACK_POINTER);
            break;
        case Format::ITALIC:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("2"), STACK_POINTER);
            break;
        case Format::MONOSPACE:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("3"), STACK_POINTER);
            break;
        default:
            LOG_DEBUG << "Unknown text formatting";
            throw TweeDocumentException("Unsupported text formatting");
    }
}

void TweeCompiler::visit(const Link& host) {
    // TODO: catch invalid link
    pair<string, string> labels = makeLabels("routineClicked");

    string id;
    string target = host.getTarget();

    if(isPreviousMacro(host.getTarget())) {
        id = GLOB_PREVIOUS_PASSAGE_ID;
    } else {
        try {
            id = to_string(passageName2id.at(target));
        } catch (const out_of_range &outOfRange) {
            throw TweeDocumentException("invalid link target: " + target);
        }
    }


    string linkName = LINK_PREFIX + to_string(linkCount++);
    ASSGEN.addString(linkName, host.getAltName());

    foundLinks.push_back(Link(host));
    ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.first)
            .call_1n(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE)
            .addLabel(labels.first);

    ASSGEN.pushStack(TABLE_LINKED_PASSAGES,
                     linkName,
                     LABEL_ROUTINE_FAIL_HANDLER, true);

    ASSGEN.setTextStyle("1");           // reversed video
    ASSGEN.print(host.getAltName());
    ASSGEN.setTextStyle("0");           // reversed video

    ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.second)
            .call_vn(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, id)
            .addLabel(labels.second);
}

void TweeCompiler::visit(const Newline& host) {
    pair<string, string> labels = makeLabels("routineClicked");
    ASSGEN.newline()
            .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.first)
            .call_1n(PAUSE_PRINTING_ROUTINE)
            .jumpEquals(GLOB_NEXT_PASSAGE_ID + " -1", labels.first)
            .ret("0")
            .addLabel(labels.first);
}

void TweeCompiler::visit(const PrintMacro& host) {
    if (Previous *previous = dynamic_cast<Previous *>(host.getExpression().get())) {
        ASSGEN.call_vs(ROUTINE_NAME_FOR_PASSAGE, GLOB_PREVIOUS_PASSAGE_ID, STACK_POINTER);
    } else {
        evalExpression(optimizeExpression(host.getExpression().get()));
        ASSGEN.print_num(STACK_POINTER);
    }
}

void TweeCompiler::visit(const DisplayMacro& host) {
    string targetPassage = host.getPassage();
    int targetId;
    try {
        targetId = passageName2id.at(targetPassage);
    } catch(const out_of_range& outOfRange) {
        throw TweeDocumentException(string("display target passage \"") + targetPassage +
                                    string("\" does not exist"));
    }

    ASSGEN.call_vn(ROUTINE_PASSAGE_BY_ID, to_string(targetId));
}

void TweeCompiler::visit(const SetMacro& host) {
    LOG_DEBUG << "generate SetMacro assembly code";

    BinaryOperation *binaryOperation = nullptr;
    if ((binaryOperation = dynamic_cast<BinaryOperation *>(host.getExpression().get()))
        && binaryOperation->getOperator() == BinOps::TO) {
        if (const Variable *var = dynamic_cast<const Variable *>(binaryOperation->getLeftSide().get())) {
            evalAssignment(binaryOperation);
            ASSGEN.pop();
        } else {
            throw TweeDocumentException("lhs of set macro is not a variable");
        }
    } else {
        throw TweeDocumentException("set macro didn't contain an assignment");
    }
}

void TweeCompiler::visit(const IfMacro& host) {
    ifContexts.push(makeNextIfContext());
    evalExpression(optimizeExpression(host.getExpression().get()));
    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({STACK_POINTER, "1"}), makeIfCaseLabel(ifContexts.top()));
}

void TweeCompiler::visit(const ElseMacro& host) {
    if (ifContexts.empty()) {
        throw TweeDocumentException("else macro encountered without preceding if macro");
    }
    ASSGEN.jump(makeIfEndLabel(ifContexts.top()));
    ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
    ifContexts.top().caseCount++;
}

void TweeCompiler::visit(const ElseIfMacro& host) {
    if (ifContexts.empty()) {
        throw TweeDocumentException("else if macro encountered without preceding if macro");
    }
    ASSGEN.jump(makeIfEndLabel(ifContexts.top()));
    ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
    ifContexts.top().caseCount++;
    evalExpression(optimizeExpression(host.getExpression().get()));
    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({STACK_POINTER, "1"}), makeIfCaseLabel(ifContexts.top()));
}

void TweeCompiler::visit(const EndIfMacro& host) {
    if (ifContexts.empty()) {
        throw TweeDocumentException("endif macro encountered without preceding if macro");
    }

    ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
    ASSGEN.addLabel(makeIfEndLabel(ifContexts.top()));

    ifContexts.pop();
}


void TweeCompiler::makePassageRoutine(const Passage &passage) {
    auto &bodyParts = passage.getBody().getBodyParts();

    // declare passage routine
    ASSGEN.addRoutine(routineNameForPassage(passage), {ZRoutineArgument("min"), ZRoutineArgument("saveUndo")});

    ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "noMouse");

    // save game often to be able to restore multiple times
    ASSGEN.saveUndo(STACK_POINTER).saveUndo(STACK_POINTER).saveUndo(STACK_POINTER).saveUndo(STACK_POINTER).saveUndo(STACK_POINTER).saveUndo(STACK_POINTER);

    // update screen
    ASSGEN.call_1n(UPDATE_MOUSE_SCREEN_ROUTINE)
            .store(GLOB_PRINT_ARROW_UP_AT_END, "0");

    ASSGEN.addLabel("noMouse");

    ASSGEN.println(string("***** ") + passage.getHead().getName() + string(" *****"));

    //  print passage contents
    for (auto it = bodyParts.begin(); it != bodyParts.end(); it++) {
        BodyPart *bodyPart = it->get();
        bodyPart->accept(*this);
    }

    // unclosed if-macro
    if (ifContexts.size() > 0) {
        throw TweeDocumentException("unclosed if macro");
    }


    ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "onlyOnePage")
            .jumpEquals(GLOB_PRINT_ARROW_UP_AT_END + " 0", "onlyOnePage")
            .call_1n(ROUTINE_PRINT_ARROW_UP)
            .addLabel("onlyOnePage");

    ASSGEN.ret("0");

    ASSGEN.addLabel(LABEL_ROUTINE_FAIL_HANDLER).ret("-1");
}

IfContext TweeCompiler::makeNextIfContext() {
    IfContext context;
    context.number = ifCount++;
    return context;
}

Expression *TweeCompiler::optimizeExpression(Expression *expression) {

    if (Const<int> *constant = dynamic_cast<Const<int> *>(expression)) {
        return constant;
    } else if (Const<bool> *constant = dynamic_cast<Const<bool> *>(expression)) {
        return constant;
    }
    else if (Variable *variable = dynamic_cast<Variable *>(expression)) {
        return variable;
    } else if (UnaryOperation *unOp = dynamic_cast<UnaryOperation *>(expression)) {
        Expression *unOpexpr = optimizeExpression(unOp->getExpression().get());
        switch (unOp->getOperator()) {
            case UnOps::NOT:
                if (Const<bool> *constant = dynamic_cast<Const<bool> *>(unOpexpr)) {
                    if (constant->getValue()) {
                        return new Const<bool>(false);
                    } else {
                        return new Const<bool>(true);
                    }
                } else {
                    return new UnaryOperation(unOp->getOperator(), unOp->getExpression().get());
                }
                break;
            case UnOps::PLUS:
                if (Const<int> *constant = dynamic_cast<Const<int> *>(unOpexpr)) {
                    return constant;
                }
                break;
            case UnOps::MINUS:
                if (Const<int> *constant = dynamic_cast<Const<int> *>(unOpexpr)) {
                    return new Const<int>(-constant->getValue());
                }
                break;
            default:
                LOG_WARNING << "unsupported operator";
                return expression;
        }
    } else if (BinaryOperation *binOp = dynamic_cast<BinaryOperation *>(expression)) {
        Expression *lExpr = optimizeExpression(binOp->getLeftSide().get());
        Expression *rExpr = optimizeExpression(binOp->getRightSide().get());

        if (binOp->getOperator() == BinOps::TO) {
            if (BinaryOperation *rrExpr = dynamic_cast<BinaryOperation *>(rExpr)) {
                if (rrExpr->getOperator() == BinOps::TO) {
                    Variable *varL = dynamic_cast<Variable *>(lExpr);
                    Variable *varR = dynamic_cast<Variable *>(rrExpr->getLeftSide().get());
                    if (rrExpr->getOperator() == BinOps::TO) {
                        if (Variable *varRR = dynamic_cast<Variable *>(rrExpr->getRightSide().get())) {
                            if (varRR->getName() == varL->getName()) {
                                return new BinaryOperation(BinOps::TO, varR, rrExpr->getRightSide().get());
                            }
                        }
                    }
                    if (varL->getName() == varR->getName()) {
                        return new BinaryOperation(BinOps::TO, varL, rrExpr->getRightSide().get());
                    }
                }
            }
            return new BinaryOperation(BinOps::TO, lExpr, rExpr);
        }

        if (Const<int> *lConst = dynamic_cast<Const<int> *>(lExpr)) {
            if (Const<int> *rConst = dynamic_cast<Const<int> *>(rExpr)) {
                switch (binOp->getOperator()) {
                    case BinOps::ADD:
                        return new Const<int>(lConst->getValue() + rConst->getValue());
                    case BinOps::SUB:
                        return new Const<int>(lConst->getValue() - rConst->getValue());
                    case BinOps::MUL:
                        return new Const<int>(lConst->getValue() * rConst->getValue());
                    case BinOps::DIV:
                        if (rConst) {
                            return new Const<int>(lConst->getValue() / rConst->getValue());
                        } else {
                            throw TweeDocumentException("div by zero!");
                        }
                    case BinOps::MOD:
                        if (rConst) {
                            return new Const<int>(lConst->getValue() % rConst->getValue());
                        } else {
                            throw TweeDocumentException("mod by zero!");
                        }
                    case BinOps::LT:
                        return new Const<bool>((lConst < rConst));
                    case BinOps::LTE:
                        return new Const<bool>((lConst <= rConst));
                    case BinOps::GT:
                        return new Const<bool>((lConst > rConst));
                    case BinOps::GTE:
                        return new Const<bool>((lConst >= rConst));
                    case BinOps::IS:
                        return new Const<bool>((lConst != rConst));
                    case BinOps::NEQ:
                        return new Const<bool>((lConst == rConst));
                    default:
                        LOG_WARNING << "unsupported operator";
                        return expression;
                }
            } else {
                return new BinaryOperation(binOp->getOperator(), lExpr, rExpr);
            }

        }
        if (Const<bool> *lConst = dynamic_cast<Const<bool> *>(lExpr)) {
            if (Const<bool> *rConst = dynamic_cast<Const<bool> *>(rExpr)) {
                switch (binOp->getOperator()) {
                    case BinOps::AND:
                        return new Const<bool>(lConst->getValue() && rConst->getValue());

                    case BinOps::OR:
                        return new Const<int>(lConst->getValue() || rConst->getValue());
                    default:
                        LOG_WARNING << "unsupported operator";
                        return expression;
                }
            }
        } else {
            return new BinaryOperation(binOp->getOperator(), lExpr, rExpr);
        }

    } else if (Visited *visited = dynamic_cast<Visited *>(expression)) {
        return visited;
    } else if (Turns *turns = dynamic_cast<Turns *>(expression)) {
        return turns;
    } else if (Random *random = dynamic_cast<Random *>(expression)) {
        Expression *start = optimizeExpression(random->getStart().get());
        Expression *end = optimizeExpression(random->getEnd().get());
        return new Random(start, end);
    } else if (Previous *previous = dynamic_cast<Previous * > (expression)) {
        return previous;
    }

    LOG_WARNING << "didn't optimize anything";
    return expression;
}

void TweeCompiler::evalAssignment(BinaryOperation *expression) {
    if (expression->getOperator() == BinOps::TO) {
        if (Variable *variable = (Variable *) (expression->getLeftSide().get())) {
            std::string variableName = variable->getName();
            variableName = variableName.erase(0, 1); //remove the $ symbol

            if (!Utils::contains<std::string>(globalVariables, variableName)) {
                globalVariables.insert(variableName);
                ASSGEN.addGlobal(variableName);
            }

            evalExpression(optimizeExpression(expression->getRightSide().get()));

            ASSGEN.load(STACK_POINTER, variableName);
            ASSGEN.push(variableName);
        } else {
            throw TweeDocumentException("left side of set assignment was not a variable");
        }
    }
}

void TweeCompiler::evalExpression(Expression *expression) {
    std::pair<std::string, std::string> labels;

    if (Const<int> *constant = dynamic_cast<Const<int> *>(expression)) {
        ASSGEN.push(std::to_string(constant->getValue()));
    } else if (Const<bool> *constant = dynamic_cast<Const<bool> *>(expression)) {
        ASSGEN.push(std::to_string(constant->getValue()));
    } else if (Variable *variable = dynamic_cast<Variable *>(expression)) {

        std::string prunedVarName = variable->getName().substr(1);

        if (Utils::contains<std::string>(globalVariables, prunedVarName)) {
            ASSGEN.push(prunedVarName);
        } else {
            globalVariables.insert(prunedVarName);
            ASSGEN.addGlobal(prunedVarName);
            ASSGEN.push(prunedVarName);
        }

    } else if (Visited *visited = dynamic_cast<Visited *>(expression)) {
        LOG_DEBUG << visited->to_string();
        size_t passageCount = visited->getPassageCount();
        if (passageCount == 0) {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, GLOB_CURRENT_PASSAGE_ID, STACK_POINTER);
        } else if (passageCount == 1) {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)]), STACK_POINTER);
        } else {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)]), "min");

            for (size_t i = 1; i < passageCount; i++) {
                string label = "NO_NEW_MIN_FOUND_" + to_string(i);
                string nextPassageVisitedCount = to_string(passageName2id[visited->getPassage(i)] + 1);

                ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, nextPassageVisitedCount, STACK_POINTER)
                        .jumpLess("sp min", "~" + label)
                        .loadw(TABLE_VISITED_PASSAGE_COUNT, nextPassageVisitedCount, "min")
                        .addLabel(label);
            }

            ASSGEN.push("min");

        }

    } else if (Turns *turns = dynamic_cast<Turns *>(expression)) {
        LOG_DEBUG << turns->to_string();
        ASSGEN.load(GLOB_TURNS_COUNT, STACK_POINTER);
    } else if (Random *random = dynamic_cast<Random *>(expression)) {
        LOG_DEBUG << random->to_string();
        std::string afterRandom = makeLabels("random").second;
        // check if a > b, act accordingly
        labels = makeLabels("randomAGTB");
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.jumpLowerEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.second);
        //  a > b
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.add(STACK_POINTER, "1", STACK_POINTER);
        ASSGEN.sub(STACK_POINTER, STACK_POINTER, STACK_POINTER);
        ASSGEN.random(STACK_POINTER, STACK_POINTER);
        evalExpression(random->getStart().get());
        ASSGEN.sub(STACK_POINTER, "1", STACK_POINTER)
                .add(STACK_POINTER, STACK_POINTER, STACK_POINTER)
                .jump(afterRandom);

        ASSGEN.addLabel(labels.second);
        // check if a < b, act accordingly
        labels = makeLabels("randomALTB");
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.jumpEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.second);
        //  a < b
        evalExpression(random->getEnd().get());
        evalExpression(random->getStart().get());
        ASSGEN.add(STACK_POINTER, "1", STACK_POINTER);
        ASSGEN.sub(STACK_POINTER, STACK_POINTER, STACK_POINTER);
        ASSGEN.random(STACK_POINTER, STACK_POINTER);
        evalExpression(random->getEnd().get());
        ASSGEN.sub(STACK_POINTER, "1", STACK_POINTER);
        ASSGEN.add(STACK_POINTER, STACK_POINTER, STACK_POINTER);
        ASSGEN.jump(afterRandom);

        ASSGEN.addLabel(labels.second);
        // a == b, simply return a
        evalExpression(random->getEnd().get());
        ASSGEN.addLabel(afterRandom);
    } else if (BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expression)) {

        if (binaryOperation->getOperator() == BinOps::TO) {
            evalAssignment(binaryOperation);

        } else {
            evalExpression(binaryOperation->getRightSide().get());
            evalExpression(binaryOperation->getLeftSide().get());

            switch (binaryOperation->getOperator()) {
                case BinOps::ADD:
                    ASSGEN.add(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::SUB:
                    ASSGEN.sub(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::MUL:
                    ASSGEN.mul(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::DIV:
                    ASSGEN.div(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::MOD:
                    ASSGEN.mod(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::AND:
                    ASSGEN.mul(STACK_POINTER, STACK_POINTER, STACK_POINTER);
                    break;
                case BinOps::OR:
                    labels = makeLabels("or");
                    ASSGEN.jumpNotEquals(std::string(STACK_POINTER) + " " + "0", labels.first)
                            .push("0")
                            .jumpNotEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::LT:
                    labels = makeLabels("lower");
                    ASSGEN.jumpLower(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::LTE:
                    labels = makeLabels("lowerEquals");
                    ASSGEN.jumpLowerEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::GT:
                    labels = makeLabels("greater");
                    ASSGEN.jumpGreater(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::GTE:
                    labels = makeLabels("greaterEquals");
                    ASSGEN.jumpGreaterEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::IS:
                    labels = makeLabels("is");
                    ASSGEN.jumpEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                case BinOps::NEQ:
                    labels = makeLabels("neq");
                    ASSGEN.jumpNotEquals(std::string(STACK_POINTER) + " " + std::string(STACK_POINTER), labels.first)
                            .push("0")
                            .jump(labels.second)
                            .addLabel(labels.first)
                            .push("1")
                            .addLabel(labels.second);
                    break;
                default:
                    //TODO: handle this
                    break;

            }
        }
    } else if (UnaryOperation *unOp = dynamic_cast<UnaryOperation *>(expression)) {
        evalExpression(unOp->getExpression().get());
        switch (unOp->getOperator()) {
            case UnOps::NOT:
                ASSGEN.sub("1", STACK_POINTER, STACK_POINTER);
                break;
            case UnOps::PLUS:
                break;
            case UnOps::MINUS:
                ASSGEN.mul("-1", STACK_POINTER, STACK_POINTER);
                break;
            default:
                throw TweeDocumentException("unsupported operator");
        }
    }
}

std::pair<std::string, std::string> TweeCompiler::makeLabels(std::string prefix) {

    std::string resultLabel = prefix.append("_");
    std::string afterLabel = "after_";
    resultLabel.append(std::to_string(labelCount));
    afterLabel.append(std::to_string(labelCount));
    ++labelCount;

    return std::make_pair(resultLabel, afterLabel);

}