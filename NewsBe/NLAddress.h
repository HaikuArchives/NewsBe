

#ifndef H_NLADDRESS
#define H_NLADDRESS

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN 64
#endif


class NLAddress 
{
public:
	typedef enum {
		addr_any = INADDR_ANY,
		addr_bcast = INADDR_BROADCAST,
		addr_loop = INADDR_LOOPBACK
	} e_addr;
	
	typedef enum {
		af_inet = AF_INET
	} e_fam;

//
//	well-known ports, for those without getservbyname()
//
	typedef enum {
		tcpReserved = 0,
		tcpMux = 1,
		tcpRJE = 5,
		tcpEcho = 7,
		tcpDiscard = 9,
		tcpSystat = 11,
		tcpDaytime = 13,
		tcpNetstat = 15,
		tcpQotd = 17,
		tcpCharGen = 19,
		tcpFtpData = 20,
		tcpFtp = 21,
		tcpTelnet = 23,
		tcpSmtp = 25,
		tcpTime = 37,
		tcpName = 42,
		tcpWhois = 43,
		tcpNameserver = 53,
		tcpPrivateRje = 77,
		tcpFinger = 79,
		tcpHttp = 80,
		tcpDCP = 93,
		tcpSupDup = 95,
		tcpHostnames = 101,
		tcpIsoTsap = 102,
		tcpX400 = 103,
		tcpX400snd = 104,
		tcpSunRpc = 111,
		tcpAuth = 113,
		tcpUucpPath = 117,
		tcpNntp = 119,
		tcpPwdGen = 129,
		tcpNetBiosSSN = 139
	} e_tcpports;
	
	typedef enum {
		udpReserved = 0,
		udpEcho = 7,
		udpDiscard = 9,
		udpSystat = 11,
		udpDaytime = 13,
		udpNetstat = 15,
		udpQotd = 17,
		udpCharGen = 19,
		udpTime = 37,
		udpName = 42,
		udpWhois = 43,
		udpNameserver = 53,
		udpBootPS = 67,
		udpBootPC = 68,
		udpTftp = 69,
		udpSunRpc = 111,
		udpNtp = 123,
		udpSnmp = 161,
		udpSnmpTrap = 162,
		udpBiff = 512,
		udpWho = 513,
		udpSyslog = 514,
		udpTimed = 525
	} e_udpports;

	typedef	enum {
		icmpEchoReply = 0,
		icmpDestUnreachable = 3,
		icmpSourceQuench = 4,
		icmpRedirect = 5,
		icmpEchoRequest = 8,
		icmpTimeout = 11,
		icmpParamProblem = 12,
		icmpTimestampRequest = 13,
		icmpTimestampReply = 14,
		icmpAddrMaskRequest = 17,
		icmpAddrMaskReply = 18
	} e_icmptypes;
	
	typedef enum {
		maxhostnamelen = MAXHOSTNAMELEN		
	} e_sizes;
		
	NLAddress(const char *hostname = 0, unsigned short port = 0);
	NLAddress(struct sockaddr_in &sa);
	NLAddress(in_addr addr, int port = 0);
	NLAddress(NLAddress::e_addr addr, int port = 0 );

	void set(const char *hostname = 0, unsigned short port = 0);
	void set(struct sockaddr_in &sa);
	void set(in_addr addr, int port = 0);
	void set(NLAddress::e_addr addr=NLAddress::addr_any, int port = 0);
		
	void get(char *hostname = 0, unsigned short *port = 0) const;
	void get(struct sockaddr_in &sa) const;
	void get(in_addr &addr, unsigned short *port = 0) const;


protected:
	void p_set(in_addr ia, unsigned short port);
	struct sockaddr_in m_sa;
};



#endif
