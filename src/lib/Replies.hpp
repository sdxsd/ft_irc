#ifndef REPLIES_HPP
#define REPLIES_HPP

#define RPL_WELCOME(source, nickname) "001 " + source + " :Welcome " + nickname + " to the ft_irc network"
#define RPL_YOURHOST(source, serverName, version) "002 " + source + " :Your host is " + serverName + " running version " + version
#define RPL_CREATED(source, date) "003 " + source + " :This server was created at " + date
#define RPL_MYINFO(source, serverName, version, userModes, channelModes) "004 " + source + " :" + serverName + " " + version + " available usermodes= " + userModes + " available channel options= " + channelModes
#define RPL_NAMREPLY(source, channel, users) "353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel) "366 " + source + " " + channel + " :End of /NAMES list."
#define RPL_NICK(source, nickname) ":" + source + " NICK :" + nickname
#define RPL_JOIN(source, channel) ":" + source + " JOIN :" + channel
#define RPL_PART(source, channel) ":" + source + " PART :" + channel
#define RPL_PRIVMSG(source, target, msg) ":" + source + " PRIVMSG " + target + " " + msg
#define RPL_NOTICE(source, target, msg) ":" + source + " NOTICE " + target + " " + msg
#define RPL_QUIT(source, msg) ":" + source + " QUIT :Quit: " + msg
#define RPL_INVITE(source, target, channel) ":" + source + " INVITE " + target + " " + channel
#define RPL_KICK(source, channel, target, reason) ":" + source + " KICK " + channel + " " + target + " " + reason
#define RPL_MODE(source, channel, modes) ":" + source + " MODE " + channel + " " + modes
#define RPL_TOPIC(source, channel, topic) ":" + source + " TOPIC " + channel + " " + topic
#define RPL_PING(source, token) ":" + source + " PONG :" + token
#define RPL_TRYAGAIN(source, nickname) "263 " + source + " :Nickname " + nickname + " is already in use, please try again"
#define ERR_UNKNOWNCOMMAND(source, command) "421 " + source + " " + command + " :Unknown command"
#define ERR_NEEDMOREPARAMS(source, command) "461 " + source + " " + command + " :Not enough parameters"
#define ERR_NOTREGISTERED(source) "451 " + source + " :You have not registered"
#define ERR_ALREADYREGISTERED(source) "462 " + source + " :You may not reregister"
#define ERR_PASSWDMISMATCH(source) "464 " + source + " :Password incorrect"
#define ERR_NONICKNAMEGIVEN(source) "431 " + source + " :Nickname not given"
#define ERR_ERRONEUSNICKNAME(source, nickname) "432 " + source + " " + nickname + " :Erroneous nickname"
#define ERR_NICKNAMEINUSE(source, nickname) "433 " + source + " " + nickname + " :Nickname is already in use"
#define ERR_TOOMANYCHANNELS(source, channel) "405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(source, channel) "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_BADCHANNELKEY(source, channel) "475 " + source + " " + channel + " :Cannot clientJoinChannel channel (+k)"
#define ERR_BADCHANMASK(source, channel) ":" + source + " 476 " + channel + " :Invalid Channel Mask"
#define ERR_NOSUCHCHANNEL(source, channel) "403 " + source + " " + channel + " :No such channel"
#define ERR_NOSUCHNICK(source, nickname) "401 " + source + " " + nickname + " :No such nickname"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) "441 " + source + " " + nickname + " " + channel + " :They aren't on that channel"
#define ERR_CHANOPRIVSNEEDED(source, channel) "482 " + source + " " + channel + " :You're not channel operator"
#define ERR_CHANNELISFULL(source, channel) "471 " + source + " " + channel + " :Cannot clientJoinChannel channel (+l)"
#define ERR_CANNOTSENDTOCHAN(source, channel) "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_INVITEONLYCHAN(source, channel) "473 " + source + " " + channel + " :Channel is invite only, couldnt join"
#define ERR_BANNEDFROMCHAN(source, channel) "474 "  + source + " " + channel + " :You are banned from this server, creep"

#endif
