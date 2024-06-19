#ifndef REPLIES_HPP
#define REPLIES_HPP

#define RPL_WELCOME(source, nickname) "001 " + source + " :Welcome " + nickname + " to the ft_irc network\r\n"
#define RPL_YOURHOST(source, serverName, version) "002 " + source + " :Your host is " + serverName + " running version " + version + "\r\n"
#define RPL_CREATED(source, date) "003 " + source + " :This server was created at " + date + "\r\n"
#define RPL_MYINFO(source, serverName, version, userModes, channelModes) "004 " + source + " :" + serverName + " " + version + " available usermodes= " + userModes + " available channel options= " + channelModes + "\r\n"
#define RPL_NAMREPLY(source, channel, users) "353 " + source + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(source, channel) "366 " + source + " " + channel + " :End of /NAMES list" + "\r\n"
#define RPL_NICK(source, nickname) ":" + source + " NICK " + nickname + "\r\n"
#define RPL_JOIN(source, channel) ":" + source + " JOIN :" + channel + "\r\n"
#define RPL_PART(source, channel, reason) ":" + source + " PART " + channel + " " + (reason.empty() ? "." : reason) + "\r\n"
#define RPL_PRIVMSG(source, target, msg) ":" + source + " PRIVMSG " + target + " " + msg + "\r\n"
#define RPL_NOTICE(source, target, msg) ":" + source + " NOTICE " + target + " " + msg + "\r\n"
#define RPL_QUIT(source, msg) ":" + source + " :QUIT: " + msg + "\r\n"
#define RPL_INVITING(user_id, client, nick, channel) user_id + " 341 " + client + " " + nick + " #" + channel + "\r\n"
#define RPL_INVITE(user_id, invited, channel) user_id + " INVITE " + invited + " #" + channel + "\r\n"
#define RPL_KICK(source, channel, target, reason) ":" + source + " KICK " + channel + " " + target + " " + reason + "\r\n"
#define RPL_MODE(source, channel, modes) ":" + source + " MODE " + channel + " " + modes + "\r\n"
#define RPL_TOPIC(source, channel, topic) ":" + source + " TOPIC " + channel + " " + topic + "\r\n"
#define RPL_NOTOPIC(client, channel) ":localhost 331 " + client + " " + channel + " :No topic is set\r\n"
#define RPL_PING(source, token) ":" + source + " PONG " + token + "\r\n"
#define RPL_TRYAGAIN(source, nickname) "263 " + source + " :Nickname " + nickname + " is already in use, please try again\r\n"
#define RPL_WHOREPLY(client, channel, username, host, server, nick, realname) "352 " + client + " " + channel + " " + username + " " + host + " " + server + " " + nick + " H " + realname + "\r\n"
#define RPL_ENDOFWHO(client, target) "315 " + client + " " + target + " :End of WHO list\r\n"
#define RPL_ENDOFBANLIST(source, channel) "368 " + source + " " + channel + " :End of channel ban list\r\n"
#define RPL_YOUREOPER(client) "381 " + client + " :You are now an IRC operator\r\n"
#define RPL_CHANNELMODEIS(client, channel, mode) "324 " + client + " " + channel + " " + mode + "\r\n"
#define ERR_UNKNOWNCOMMAND(source, command) "421 " + source + " " + command + " :Unknown command\r\n"
#define ERR_NEEDMOREPARAMS(source, command) "461 " + source + " " + command + " :Not enough parameters\r\n"
#define ERR_NOTREGISTERED(source) "451 " + source + " :You have not registered\r\n"
#define ERR_ALREADYREGISTERED(source) "462 " + source + " :You may not reregister\r\n"
#define ERR_PASSWDMISMATCH(source) "464 " + source + " :Password incorrect\r\n"
#define ERR_NONICKNAMEGIVEN(source) "431 " + source + " :Nickname not given\r\n"
#define ERR_UMODEUNKNOWNFLAG(client) "501 " + client + " :Unknown MODE flag\r\n"
#define ERR_ERRONEUSNICKNAME(source, nickname) "432 " + source + " " + nickname + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(source, nickname) "433 " + source + " " + nickname + " :Nickname is already in use\r\n"
#define ERR_TOOMANYCHANNELS(source, channel) "405 " + source + " " + channel + " :You have joined too many channels\r\n"
#define ERR_NOTONCHANNEL(source, channel) "442 " + source + " " + channel + " :You're not on that channel\r\n"
#define ERR_BADCHANNELKEY(source, channel) "475 " + source + " " + channel + " :Cannot join channel (+k)\r\n"
#define ERR_BADCHANMASK(source, channel) ":" + source + " 476 " + channel + " :Invalid Channel Mask\r\n"
#define ERR_NOSUCHCHANNEL(source, channel) "403 " + source + " " + channel + " :No such channel\r\n"
#define ERR_NOSUCHNICK(source, nickname) "401 " + source + " " + nickname + " :No such nickname\r\n"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) "441 " + source + " " + nickname + " " + channel + " :They aren't on that channel\r\n"
#define ERR_USERONCHANNEL(source, nick, channel) ":localhost 443 " + source + " " + nick + " " + channel + " :Is connected to channel.\r\n"
#define ERR_CHANOPRIVSNEEDED(source, channel) "482 " + source + " " + channel + " :You're not channel operator\r\n"
#define ERR_CHANNELISFULL(source, channel) "471 " + source + " " + channel + " :Cannot clientJoinChannel channel (+l)\r\n"
#define ERR_CANNOTSENDTOCHAN(source, channel) "404 " + source + " " + channel + " :Cannot send to channel\r\n"
#define ERR_INVITEONLYCHAN(source, channel) "473 " + source + " " + channel + " :Channel is invite only, couldnt join\r\n"
#define ERR_BANNEDFROMCHAN(source, channel) "474 " + source + " " + channel + " :You are banned from this server, creep\r\n"
#define ERR_ENDOFBANLIST(source, channel) "368 " + source + " " + channel + " :End of channel ban list\r\n"
#define ERR_INVALIDKEY(client, channel) "525 " + client + " " + channel + " :Key is not well-formed\r\n"

#endif
