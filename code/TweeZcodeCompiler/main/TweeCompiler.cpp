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
        LINK_PREFIX = "LINK_",

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
            "   read_char 1 -> USER_INPUT\n"
            "   sub USER_INPUT 48 -> sp\n"
            "   sub sp 1 -> sp\n"
            "   \n"
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
            "    loadb USERINPUT_LOOKUP sp -> sp\n"
            "    new_line\n"
            "    jump ?fetch_userinput_lookup\n"

            "mouse_supported:\n"
            "    push PRINT_ARROW_UP_AT_END\n"
            "    call_vs mouseClick sp -> sp\n"
            "fetch_userinput_lookup:\n"

            "\n"
            "   add TURNS 1 -> TURNS\n"
            "   \n"
            "   call_vs __stack_at LINKED_PASSAGES " << to_string(LINKED_PASSAGES_SIZE) << " sp -> sp\n"
            "   call_vs passage_id_for_string_link sp -> sp\n"
            "   pop_stack linkedcount LINKED_PASSAGES\n"
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

    out << makeUserInputRoutine() << makeClearTablesRoutine();

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
}

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

void TweeCompiler::visit(const Text& host) {
    ASSGEN.print(host.getContent());
}
void TweeCompiler::visit(const Formatting& host) {
    switch (host.getFormat()) {
        case Format::UNDERLINED:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("0"), "sp");
            break;
        case Format::BOLD:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("1"), "sp");
            break;
        case Format::ITALIC:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("2"), "sp");
            break;
        case Format::MONOSPACE:
            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, std::string("3"), "sp");
            break;
        default:
            LOG_DEBUG << "Unknown text formatting";
            throw TweeDocumentException("Unsupported text formatting");
    }
}

void TweeCompiler::visit(const Link& host) {
    // TODO: catch invalid link
    pair<string, string> labels = makeLabels("routineClicked");

    if (isPreviousMacro(host.getTarget())) {
        // TODO: re-enable support for this
        //ASSGEN.storeb(TABLE_LINKED_PASSAGES, GLOB_PREVIOUS_PASSAGE_ID, "1");
        ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.first)
                .call_1n(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE)
                .addLabel(labels.first);

        ASSGEN.storeb(TABLE_LINKED_PASSAGES, GLOB_PREVIOUS_PASSAGE_ID, "1");

        ASSGEN.setTextStyle("1");           // reversed video
        ASSGEN.print(host.getAltName());
        ASSGEN.setTextStyle("0");           // reversed video

        ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.second)
                .call_vn(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, GLOB_PREVIOUS_PASSAGE_ID)
                .addLabel(labels.second);
    } else {
        int id;
        string target = host.getTarget();

        try {
            id = passageName2id.at(target);
        } catch(const out_of_range& outOfRange) {
            throw TweeDocumentException("invalid link target: " + target);
        }


        string linkName = LINK_PREFIX + to_string(linkCount++);
        ASSGEN.addString(linkName, (host.getAltName() == "") ? host.getTarget() : host.getAltName());

        ASSGEN.pushStack(TABLE_LINKED_PASSAGES,
                         linkName,
                         LABEL_ROUTINE_FAIL_HANDLER, true);
        foundLinks.push_back(Link(host));
        ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.first)
                .call_1n(UPDATE_MOUSE_TABLE_BEFORE_ROUTINE)
                .addLabel(labels.first);

        ASSGEN.storeb(TABLE_LINKED_PASSAGES, id, "1");

        ASSGEN.setTextStyle("1");           // reversed video
        ASSGEN.print(host.getAltName());
        ASSGEN.setTextStyle("0");           // reversed video

        ASSGEN.jumpEquals(GLOB_INTERPRETER_SUPPORTS_MOUSE + " 0", labels.second)
                .call_vn(UPDATE_MOUSE_TABLE_AFTER_ROUTINE, to_string(id))
                .addLabel(labels.second);
    }
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
        ASSGEN.call_vs(ROUTINE_NAME_FOR_PASSAGE, GLOB_PREVIOUS_PASSAGE_ID, "sp");
    } else {
                evalExpression(host.getExpression().get());
                ASSGEN.print_num("sp");
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
    evalExpression(host.getExpression().get());
    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "1"}), makeIfCaseLabel(ifContexts.top()));
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
    evalExpression(host.getExpression().get());
    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "1"}), makeIfCaseLabel(ifContexts.top()));
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
    ASSGEN.saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp").saveUndo("sp");

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
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)]), "sp");
        } else {
            ASSGEN.loadw(TABLE_VISITED_PASSAGE_COUNT, to_string(passageName2id[visited->getPassage(0)]), "min");

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

        } else {
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