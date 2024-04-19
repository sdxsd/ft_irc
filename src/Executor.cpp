

Executor::Executor(Env& e) : e(e) {
	this->funcMap["CAP"] 		= &Executor::run_CAP;
	this->funcMap["PASS"] 		= &Executor::run_PASS;
	this->funcMap["NICK"] 		= &Executor::run_NICK;
	this->funcMap["USER"] 		= &Executor::run_USER;
	this->funcMap["MODE"] 		= &Executor::run_MODE;
	this->funcMap["PING"] 		= &Executor::run_PING;
	this->funcMap["PRIVMSG"] 	= &Executor::run_PRIVMSG;
	this->funcMap["NOTICE"] 	= &Executor::run_NOTICE;
	this->funcMap["JOIN"] 		= &Executor::run_JOIN;
	this->funcMap["KICK"] 		= &Executor::run_KICK;
	this->funcMap["PART"] 		= &Executor::run_PART;
	this->funcMap["INVITE"] 	= &Executor::run_INVITE;
	this->funcMap["TOPIC"] 		= &Executor::run_TOPIC;
	this->funcMap["QUIT"] 		= &Executor::run_QUIT;

	this->argCount["CAP"] 		= {1, 1};
	this->argCount["PASS"] 		= {1, 1};
	this->argCount["NICK"] 		= {1, 1};
	this->argCount["USER"] 		= {4, -1};
	this->argCount["MODE"] 		= {1, -1};
	this->argCount["PING"] 		= {1, 1};
	this->argCount["PRIVMSG"] 	= {2, -1};
	this->argCount["NOTICE"]	= {2, -1};
	this->argCount["JOIN"] 		= {1, 2};
	this->argCount["KICK"] 		= {1, -1};
	this->argCount["PART"] 		= {1, -1};
	this->argCount["INVITE"] 	= {1, 2};
	this->argCount["TOPIC"] 	= {1, -1};
	this->argCount["QUIT"] 		= {0, -1};
	Msg("Executor - constructor(env &e)", "CLASS");
}