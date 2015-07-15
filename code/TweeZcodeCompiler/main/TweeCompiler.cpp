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

#include <Passage/Body/Link.h>
#include <Passage/Body/Text.h>
#include <Passage/Body/Newline.h>
#include <Passage/Body/Macros/SetMacro.h>

#include <Passage/Body/Expressions/BinaryOperation.h>
#include <Passage/Body/Expressions/Variable.h>
#include <Passage/Body/Expressions/UnaryOperation.h>

#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>

#include "Passage/Body/Expressions/Visited.h"
#include "Passage/Body/Expressions/Turns.h"
#include "Passage/Body/Expressions/Random.h"
#include "Passage/Body/Expressions/Previous.h"

#include "Passage/Body/Macros/Display.h"
#include "Passage/Body/Macros/Print.h"
#include <Passage/Body/Macros/SetMacro.h>

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
        ROUTINE_MAIN = "main",
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

//#define ZAS_DEBUG

#ifndef ASSGEN
#define ASSGEN (*_assgen)
#endif

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

string labelForPassage(Passage &passage) {
    stringstream ss;
    string passageName = passage.getHead().getName();
    maskString(passageName);
    ss << "L_" << passageName;
    return ss.str();
}

string makeIfCaseLabel(const IfContext &context) {
    return ELSE_LABEL_PREFIX + to_string(context.number) + "_" + to_string(context.caseCount);
}

string makeIfEndLabel(const IfContext &context) {
    return ENDIF_LABEL_PREFIX + to_string(context.number);
}


string makeUserInputRoutine() {
    return ".FUNCT display_links i, selectcount\n"
            "   loop_start:\n"
            "   loadb LINKED_PASSAGES i -> sp\n"
            "   jz sp ?passage_not_set\n"
            "   storeb USERINPUT_LOOKUP selectcount i\n"
            "   add selectcount 1 -> sp\n"
            "   add selectcount 49 -> sp\n"
            "   je INTERPRETER_SUPPORTS_MOUSE 1 ?mouse_is_supported\n"
            "   print_char sp\n"
            "   print \": \"\n"
            "   call_vs print_name_for_passage i -> sp\n"
            "   new_line\n"
            "   mouse_is_supported:\n"
            "   add selectcount 1 -> selectcount\n"
            "\n"
            "   passage_not_set:\n"
            "\n"
            "    add i 1 -> i\n"
            "    jl i PASSAGES_COUNT ?loop_start\n"
            "\n"
            "    jz selectcount ?~links_available\n"
            "    ret -1\n"
            "links_available:\n"
            "    je INTERPRETER_SUPPORTS_MOUSE 1 ?mouse_supported\n"
            "    read_char 1 -> USER_INPUT\n"
            "    sub USER_INPUT 48 -> sp\n"
            "    sub sp 1 -> sp\n"

            // TODO: Does work but I don know why. Was only:
            //          loadb USERINPUT_LOOKUP sp -> sp
            //       before

            "    loadb USERINPUT_LOOKUP sp -> sp\n"
            "    new_line"
            // "    print \" remove this output: \""
            // "    print_num sp \n"
            "    jump ?fetch_userinput_lookup\n"

            "mouse_supported:\n"
            "    push PRINT_ARROW_UP_AT_END\n"
            "    call_vs mouseClick sp -> sp\n"
            "fetch_userinput_lookup:\n"

            "\n"
            "\t add TURNS 1 -> TURNS\n"
            "   ret sp"
            "\n";
}

string makeClearTablesRoutine() {
    return ".FUNCT reset_tables i\n"
            "   loop_start:\n"
            "   storeb LINKED_PASSAGES i 0\n"
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
    ifContexts = stack<IfContext>();
    ifCount = 0;

    {
        int i = 0;
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            passageName2id[passage->getHead().getName()] = i;
            i++;
        }
    }

    // check if start passage is contained
    try {
        passageName2id.at("Start");
    } catch (const out_of_range &outOfRange) {
        throw TweeDocumentException("Twee document doesn't contain a start passage");
    }

    // tables needed for routine linking
    ASSGEN.addByteArray(TABLE_LINKED_PASSAGES, (unsigned) passages.size());
    ASSGEN.addByteArray(TABLE_USERINPUT_LOOKUP, (unsigned) passages.size());

    ASSGEN.addWordArray(TABLE_VISITED_PASSAGE_COUNT, (unsigned) passages.size());
    ASSGEN.addWordArray(TABLE_CURSOR, 2);
    ASSGEN.addWordArray(TABLE_MOUSE_CLICK, 20);


    ASSGEN.addWordArray(TABLE_MOUSE_LINKS, MAX_MOUSE_LINKS * 4 * 5); // maximum of MAX_MOUSE_LINKS links per page of a passage possible
    ASSGEN.addWordArray(TABLE_MOUSE_CLICK_ARROWS, 2 * 4);



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

                // TODO: bold and italic
                //.setTextStyle("6")
                .print(INTRO_TITLE)
                //.setTextStyle("4")
                .newline()
                .newline()

                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 1", "mouseSupported")
                .print(INTRO_NO_MOUSE)
                .newline()
                .newline()
                .newline()
                .print(INTRO_NEXT_NO_MOUSE)
                .read_char(varKey)
                .ret("0");

        ASSGEN.addLabel("mouseSupported")
                .read_char(varKey)          // workaround for wrong jump offset calculation
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

    out << makeUserInputRoutine() << makeClearTablesRoutine();

    // call passage by id routine
    {
        const string idLocal = "id";
        const string passageCount = "passage_count";
        ASSGEN.addRoutine(ROUTINE_PASSAGE_BY_ID, vector<ZRoutineArgument>({ZRoutineArgument(idLocal), ZRoutineArgument(passageCount)}))
                .store(GLOB_PREVIOUS_PASSAGE_ID, GLOB_CURRENT_PASSAGE_ID)
                .store(GLOB_CURRENT_PASSAGE_ID, idLocal)
                .add(GLOB_CURRENT_PASSAGE_ID, "1", GLOB_CURRENT_PASSAGE_ID);
        
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
            ASSGEN.addLabel(labelForPassage(*it)).call_vs(routineNameForPassage(*it), nullopt, "sp").ret("0");
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

        ASSGEN.loadb("33", 0, "sp")                   // max chars per line
                .getWindowProperty("0", "3", "sp")    // window width in pixels
                .sub("sp", to_string(MARGIN_LEFT), "sp")
                .sub("sp", to_string(MARGIN_RIGHT), "sp")
                .div("sp", "sp", varCharWidth)
                .add(varCharWidth, "1", varCharWidth);

        ASSGEN.addLabel("MOUSE_CLICK_LOOP")
                .mouseWindow("-1")
                .read_char(varKey)
                .mouseWindow("1");

        ASSGEN.readMouse(TABLE_MOUSE_CLICK)
                .loadw(TABLE_MOUSE_CLICK, "0", varYMouse)
                .loadw(TABLE_MOUSE_CLICK, "1", varXMouse);

        //ASSGEN.newline().print_num(varXMouse).print("/").print_num(varYMouse).newline();

        ASSGEN.div(varYMouse, varFontSize, varYLineClick)
                .add(varYLineClick, "1", varYLineClick)
                .div(varXMouse, varCharWidth, varXMouseClick);


       /* ASSGEN.setCursor(varYMouse, varXMouse, "0")
                .getCursor(TABLE_CURSOR)
                .loadb(TABLE_CURSOR, "0", "sp")
                .print("curent cursor line: ")
                .print_num("sp")
                .newline()
                .loadb(TABLE_CURSOR, "1", "sp")
                .print("current cursor width: ")
                .print_num("sp")
                .newline();*/

        /*ASSGEN.newline();
        ASSGEN.print("yline click: ");
        ASSGEN.print_num(varYLineClick);
        ASSGEN.newline();
        ASSGEN.print("xline click: ");
        ASSGEN.print_num(varXMouseClick);
        ASSGEN.newline();
        ASSGEN.print("GLOB_TABLE_MOUSE_LINKS_NEXT: ");
        ASSGEN.print_num(GLOB_TABLE_MOUSE_LINKS_NEXT);
        ASSGEN.newline();*/

        /*ASSGEN.jumpEquals(varClickArrows + " 1", "beeeeep");
        ASSGEN.store(varMouseTableIndex, "0");
        ASSGEN.addLabel("fu")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, varMouseTableIndex, "sp")
                .print_num("sp")
                .newline().add(varMouseTableIndex, "1", varMouseTableIndex)
                .jumpLess(varMouseTableIndex + " 8", "fu");
        ASSGEN.addLabel("beeeeep");*/

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
                .read_char("sp")                                         // read_char only workaround here for wrong jump offset -> is not executed
                .jumpEquals(varClickArrows + " 0", "MOUSE_CLICK_LOOP")   // are arrows active?
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "1", "sp")              // test here with x value because arrows are always drawn on right sight
                .jumpEquals("sp 0", "ARROW_DOWN")

                .loadw(TABLE_MOUSE_CLICK_ARROWS, "0", "sp")       // lineTop
                .jumpLess(varYLineClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "1", "sp")       // width left
                .jumpLess(varXMouseClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "2", "sp")       // line bottom
                .jumpGreater(varYLineClick + " sp", "ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "3", "sp")       // width right
                .jumpGreater(varXMouseClick + " sp", "ARROW_DOWN")
                .restoreUndo("sp");                               // restores game to start this passage again

        ASSGEN.addLabel("ARROW_DOWN")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "5", "sp")
                .jumpEquals("sp 0", "MOUSE_CLICK_LOOP")

                .loadw(TABLE_MOUSE_CLICK_ARROWS, "4", "sp")              // lineTop
                .jumpLess(varYLineClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "5", "sp")              // width left
                .jumpLess(varXMouseClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "6", "sp")              // line bottom
                .jumpGreater(varYLineClick + " sp", "MOUSE_CLICK_LOOP")
                .loadw(TABLE_MOUSE_CLICK_ARROWS, "7", "sp")              // width right
                .jumpGreater(varXMouseClick + " sp", "MOUSE_CLICK_LOOP")
                .ret("-1");                                              // signals that no link was clicked


        /*ASSGEN.push(varYCursor)
                .sub("sp", "2", "sp")
                .jumpGreater(varYLineClick + " sp", "MOUSE_CLICK_LOOP");

        ASSGEN.push(varYCursor)
                .sub("sp", "1", "sp")
                .sub("sp", varSelectableLinks, "sp")
                .store(varFirstLinkLine, "sp")
                .jumpLess(varYLineClick + " " + varFirstLinkLine, "MOUSE_CLICK_LOOP");

        ASSGEN.push(varYLineClick)
                .sub("sp", varFirstLinkLine, "sp");

        ASSGEN.newline().newline();

        ASSGEN.ret("sp");*/


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
                .loadw(TABLE_CURSOR, "0", "sp") // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "0", "sp")
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "1", "sp")
                .print("/ \\ ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", "sp") // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "2", "sp")
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "3", "sp");

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
                .loadw(TABLE_CURSOR, "0", "sp") // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "4", "sp")
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "5", "sp")
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" V  ")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", "sp") // line number of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "6", "sp")
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_CLICK_ARROWS, "7", "sp");

        ASSGEN.setWindow("0")
                .ret("0");


        // support mouse routine

        ASSGEN.addRoutine(SUPPORTS_MOUSE_ROUTINE);

        ASSGEN.loadb("23", "0", "sp")  // flags in header, bit 5 needs to be true
                .lor("sp", "11011111", "sp")
                .jumpZero("sp", "NOT_SUPPORTED");


        ASSGEN.loadb("50", "0", "sp")  // revision number first part, needs to be 1
                .land("sp", "00000001", "sp")
                .jumpZero("sp", "NOT_SUPPORTED");


        ASSGEN.loadb("50", "1", "sp")  // revision number second part, needs to be 1
                .land("sp", "00000001", "sp")
                .jumpZero("sp", "NOT_SUPPORTED");

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

        /*ASSGEN.setWindow("1");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print("/ \\ ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ");

        ASSGEN.newline()
                .call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ");

        ASSGEN.store(varLinesPrinted, "3")
                .addLabel("loop")
                .newline()
                .add(varLinesPrinted, "1", varLinesPrinted)
                .jumpLess(varLinesPrinted + " " + varLineCount, "loop");

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" |  ")
                .newline();

        ASSGEN.call_vn(PRINT_SPACES_ROUTINE, varCharWidth)
                .print(" V  ");

        ASSGEN.setWindow("0");*/


        ASSGEN.addLabel("no_mouse")
                .ret("0");

        // UPDATE_MOUSE_TABLE_BEFORE_ROUTINE

        ASSGEN.addRoutine(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE)
                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "no_mouse")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "0", "sp") // line number of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, "sp")
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, "sp")
                .add(GLOB_TABLE_MOUSE_LINKS_NEXT, "1", GLOB_TABLE_MOUSE_LINKS_NEXT)
                .addLabel("no_mouse")
                .ret("0");

        // UPDATE_MOUSE_TABLE_AFTER_ROUTINE

        string varId = "passage_id";

        ASSGEN.addRoutine(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, {ZRoutineArgument(varId)})
                .jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", "no_mouse")
                .getCursor(TABLE_CURSOR)
                .loadw(TABLE_CURSOR, "1", "sp") // x position of cursor
                .storew(TABLE_MOUSE_LINKS, GLOB_TABLE_MOUSE_LINKS_NEXT, "sp")
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
                .loadb("32", 0, "sp")
                .sub("sp", to_string(MARGIN_BOTTOM), "sp")
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
        args.push_back(ZRoutineArgument(varCounter));
        args.push_back(ZRoutineArgument(varResult));
        args.push_back(
                ZRoutineArgument(varFormatType));    // This value will be set via call_vs TEXT_FORMAT_ROUTINE 1 -> sp
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
}

bool isPreviousMacro(string link) {
    string previousFunc = "previous()";

    while (link.size() > previousFunc.size() && link.at(0) == ' ') {
        link = link.substr(1);
    }
    while (link.size() > previousFunc.size() && link.at(link.size() - 1) == ' ') {
        link = link.substr(0, link.size() - 1);
    }

    return link == previousFunc;
}

void TweeCompiler::makePassageRoutine(const Passage &passage) {
    auto &bodyParts = passage.getBody().getBodyParts();

    // declare passage routine
    ASSGEN.addRoutine(routineNameForPassage(passage), {ZRoutineArgument("min"), ZRoutineArgument("saveUndo")});

    // save game often to be able to restore multiple times
    ASSGEN.saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp");

    // update screen
    ASSGEN.call_1n(UPDATE_MOUSE_SCREEN_ROUTINE)
            .store(GLOB_PRINT_ARROW_UP_AT_END, "0");

    ASSGEN.println(string("***** ") + passage.getHead().getName() + string(" *****"));

    //  print passage contents
    for (auto it = bodyParts.begin(); it != bodyParts.end(); it++) {
        BodyPart *bodyPart = it->get();
        if (Text *text = dynamic_cast<Text *>(bodyPart)) {
            ASSGEN.print(text->getContent());
        } else if (Link *link = dynamic_cast<Link *>(bodyPart)) {
            // TODO: catch invalid link

            if (isPreviousMacro(link->getTarget())) {
                ASSGEN.storeb(TABLE_LINKED_PASSAGES, GLOB_PREVIOUS_PASSAGE_ID, "1");
            } else {
                int id;
                string target = link->getTarget();
                try {
                    id = passageName2id.at(target);
                } catch(const out_of_range& outOfRange) {
                    throw TweeDocumentException("invalid link target: " + target);
                }

                ASSGEN.storeb(TABLE_LINKED_PASSAGES, id, "1");
                ASSGEN.call_1n(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE);

                ASSGEN.print("|||");
                ASSGEN.print(link->getTarget());
                ASSGEN.print("|||");

                ASSGEN.call_vn(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, to_string(id));
            }
        } else if (Newline *newLine = dynamic_cast<Newline *>(bodyPart)) {
            pair<string, string> labels = makeLabels("routineClicked");
            ASSGEN.newline()
                    .call_1n(PAUSE_PRINTING_ROUTINE)
                    .jumpEquals(GLOB_NEXT_PASSAGE_ID + " -1", labels.first)
                    .ret("0")
                    .addLabel(labels.first);
        } else if (Display *display = dynamic_cast<Display *>(bodyPart)) {
            string targetPassage = display->getPassage();
            int targetId;
            try {
                targetId = passageName2id.at(targetPassage);
            } catch(const out_of_range& outOfRange) {
                throw TweeDocumentException(string("display target passage \"") + targetPassage +
                                                    string("\" does not exist"));
            }

            ASSGEN.call_vn(ROUTINE_PASSAGE_BY_ID, to_string(targetId));
        } else if (Print *print = dynamic_cast<Print *>(bodyPart)) {
            if (Previous *previous = dynamic_cast<Previous *>(print->getExpression().get())) {
                ASSGEN.call_vs(ROUTINE_NAME_FOR_PASSAGE, GLOB_PREVIOUS_PASSAGE_ID, "sp");
            } else {
                evalExpression(print->getExpression().get());
                ASSGEN.print_num("sp");
            }
        } else if (IfMacro *ifMacro = dynamic_cast<IfMacro *>(bodyPart)) {
            ifContexts.push(makeNextIfContext());
            evalExpression(ifMacro->getExpression().get());
            ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "1"}), makeIfCaseLabel(ifContexts.top()));
        } else if (ElseIfMacro *elseIfMacro = dynamic_cast<ElseIfMacro *>(bodyPart)) {
            if (ifContexts.empty()) {
                throw TweeDocumentException("else if macro encountered without preceding if macro");
            }
            ASSGEN.jump(makeIfEndLabel(ifContexts.top()));
            ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
            ifContexts.top().caseCount++;
            evalExpression(elseIfMacro->getExpression().get());
            ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "1"}), makeIfCaseLabel(ifContexts.top()));
        } else if (ElseMacro *elseMacro = dynamic_cast<ElseMacro *>(bodyPart)) {
            if (ifContexts.empty()) {
                throw TweeDocumentException("else macro encountered without preceding if macro");
            }
            ASSGEN.jump(makeIfEndLabel(ifContexts.top()));
            ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
            ifContexts.top().caseCount++;
        } else if (EndIfMacro *endifMacro = dynamic_cast<EndIfMacro *>(bodyPart)) {
            if (ifContexts.empty()) {
                throw TweeDocumentException("endif macro encountered without preceding if macro");
            }

            ASSGEN.addLabel(makeIfCaseLabel(ifContexts.top()));
            ASSGEN.addLabel(makeIfEndLabel(ifContexts.top()));

            ifContexts.pop();
        } else if (SetMacro *op = dynamic_cast<SetMacro *>(bodyPart)) {
            LOG_DEBUG << "generate SetMacro assembly code";

            BinaryOperation *binaryOperation = nullptr;
            if ((binaryOperation = dynamic_cast<BinaryOperation *>(op->getExpression().get()))
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
    }

    // unclosed if-macro
    if (ifContexts.size() > 0) {
        throw TweeDocumentException("unclosed if macro");
    }

    ASSGEN.jumpEquals(GLOB_PRINT_ARROW_UP_AT_END + " 0", "onlyOnePage")
            .call_1n(ROUTINE_PRINT_ARROW_UP)
            .addLabel("onlyOnePage");

    ASSGEN.ret("0");
}

IfContext TweeCompiler::makeNextIfContext() {
    IfContext context;
    context.number = ifCount++;
    return context;
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

            evalExpression(expression->getRightSide().get());

            ASSGEN.load("sp", variableName);
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
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, GLOB_CURRENT_PASSAGE_ID, "sp");
        } else if (passageCount == 1) {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)] + 1), "sp");
        } else {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)] + 1), "min");

            for (size_t i = 1; i < passageCount; i++) {
                string label = "NO_NEW_MIN_FOUND_" + to_string(i);
                string nextPassageVisitedCount = to_string(passageName2id[visited->getPassage(i)] + 1);

                ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, nextPassageVisitedCount, "sp")
                        .jumpLess("sp min", "~" + label)
                        .loadw(TABLE_VISITED_PASSAGE_COUNT, nextPassageVisitedCount, "min")
                        .addLabel(label);
            }

            ASSGEN.push("min");

        }
    } else if (Turns *turns = dynamic_cast<Turns *>(expression)) {
        LOG_DEBUG << turns->to_string();
        ASSGEN.load(GLOB_TURNS_COUNT, "sp");
    } else if (Random *random = dynamic_cast<Random *>(expression)) {
        LOG_DEBUG << random->to_string();
        std::string afterRandom = makeLabels("random").second;
    // check if a > b, act accordingly
        labels = makeLabels("randomAGTB");
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.jumpLowerEquals(std::string("sp") + " " + std::string("sp"), labels.second);
    //  a > b
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.add("sp", "1", "sp");
        ASSGEN.sub("sp", "sp", "sp");
        ASSGEN.random("sp", "sp");
        evalExpression(random->getStart().get());
        ASSGEN.sub("sp", "1", "sp");
        ASSGEN.add("sp", "sp", "sp");
        ASSGEN.jump(afterRandom);

        ASSGEN.addLabel(labels.second);
    // check if a < b, act accordingly
        labels = makeLabels("randomALTB");
        evalExpression(random->getStart().get());
        evalExpression(random->getEnd().get());
        ASSGEN.jumpEquals(std::string("sp") + " " + std::string("sp"), labels.second);
    //  a < b
        evalExpression(random->getEnd().get());
        evalExpression(random->getStart().get());
        ASSGEN.add("sp", "1", "sp");
        ASSGEN.sub("sp", "sp", "sp");
        ASSGEN.random("sp", "sp");
        evalExpression(random->getEnd().get());
        ASSGEN.sub("sp", "1", "sp");
        ASSGEN.add("sp", "sp", "sp");
        ASSGEN.jump(afterRandom);

        ASSGEN.addLabel(labels.second);
    // a == b, simply return a
        evalExpression(random->getEnd().get());
        ASSGEN.addLabel(afterRandom);
    } else if (BinaryOperation *binaryOperation = dynamic_cast<BinaryOperation *>(expression)) {

        if (binaryOperation->getOperator() == BinOps::TO) {
            evalAssignment(binaryOperation);
        }

        TweeCompiler::evalExpression(binaryOperation->getRightSide().get());
        TweeCompiler::evalExpression(binaryOperation->getLeftSide().get());

        switch (binaryOperation->getOperator()) {
            case BinOps::ADD:
                ASSGEN.add("sp", "sp", "sp");
                break;
            case BinOps::SUB:
                ASSGEN.sub("sp", "sp", "sp");
                break;
            case BinOps::MUL:
                ASSGEN.mul("sp", "sp", "sp");
                break;
            case BinOps::DIV:
                ASSGEN.div("sp", "sp", "sp");
                break;
            case BinOps::MOD:
                ASSGEN.mod("sp", "sp", "sp");
                break;
            case BinOps::AND:
                ASSGEN.land("sp", "sp", "sp");
                break;
            case BinOps::OR:
                ASSGEN.lor("sp", "sp", "sp");
                break;
            case BinOps::TO:
                // TODO: check if this is right
                ASSGEN.load("sp", "sp");
                break;
            case BinOps::LT:
                labels = makeLabels("lower");
                ASSGEN.jumpLower(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::LTE:
                labels = makeLabels("lowerEquals");
                ASSGEN.jumpLowerEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::GT:
                labels = makeLabels("greater");
                ASSGEN.jumpGreater(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::GTE:
                labels = makeLabels("greaterEquals");
                ASSGEN.jumpGreaterEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::IS:
                labels = makeLabels("is");
                ASSGEN.jumpEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::NEQ:
                labels = makeLabels("neq");
                ASSGEN.jumpNotEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            default:
                //TODO: handle this
                break;

        }
    } else if (UnaryOperation *unOp = dynamic_cast<UnaryOperation *>(expression)) {
        TweeCompiler::evalExpression(unOp->getExpression().get());
        switch (unOp->getOperator()) {
            case UnOps::NOT:
                ASSGEN.lnot("sp", "sp");
                break;
            case UnOps::PLUS:
                ASSGEN.push("0");
                ASSGEN.add("sp", "sp", "sp");
                break;
            case UnOps::MINUS:
                ASSGEN.push("0");
                ASSGEN.sub("sp", "sp", "sp");
                break;
            default:
                // TODO: handle this
                break;
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