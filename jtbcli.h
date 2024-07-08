#include "jtbstr.h"

namespace JTB {

class ClCommand {
    std::array<char, 128> buffer {};
    Str cmd {};
    FILE* pipe {};
    bool empty { true };
    std::pair<Str, int> result { "", 0 };
    void readAndClosePipe() {
	while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
	    result.first += buffer.data();
	}
	result.second = pclose(pipe);
    }

public:
    ClCommand() = default;
    ClCommand(const Str& cmd): cmd(cmd), pipe(popen((cmd.concat(" 2>&1")).c_str(), "r")) {
	if (!pipe) {
	    Str errormsg { "popen() failed when constructing a JTB::ClCommand with cmd: "  };
	    errormsg += cmd;
	    throw std::runtime_error(errormsg.c_str());
	}
	readAndClosePipe();
	empty = false;
    }

    void sendCommand(const Str& command) {
	cmd = command;
	pipe = popen(command.concat(" 2>&1").c_str(), "r");
	if (!pipe) {
	    Str errormsg { "popen() failed when running sendCommand() on a JTB::ClCommand with cmd: "  };
	    errormsg += command;
	    throw std::runtime_error(errormsg.c_str());
	}
	readAndClosePipe();
	empty = false;
    }


    const std::pair<Str, int>& getResult() const {
	if (empty) throw std::runtime_error("Called getResult() on an empty JTB::ClCommand!");
	return result;
    }
};

}

