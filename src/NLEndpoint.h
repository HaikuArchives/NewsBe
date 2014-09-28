//
// NLEndpoint.h
//
// Copyright 1997 by Howard Berkey
//

#ifndef H_NLENDPOINT
#define H_NLENDPOINT

#include "NLPacket.h"
#include "NLAddress.h"
#include <errno.h>
#include <string.h>

class NLEndpoint
{
public:
	typedef enum {
		datagram = SOCK_DGRAM,
		stream = SOCK_STREAM
	} e_type;
	
	typedef enum {
		debugopt = SO_DEBUG,
		reuseopt = SO_REUSEADDR
	} e_options;
	
	typedef enum {
		sol_socket = SOL_SOCKET
	} e_optlevel;
	
	typedef enum {
		msg_oob = MSG_OOB
	} e_flags;
	
	typedef enum {
		proto_udp = IPPROTO_UDP,
		proto_tcp = IPPROTO_TCP,
		proto_icmp = IPPROTO_ICMP
	} e_proto;
	
	NLEndpoint(NLEndpoint::e_type proto = NLEndpoint::stream);
	virtual ~NLEndpoint();
	
	void setProtocol(NLEndpoint::e_type);
	
	int setOption(NLEndpoint::e_options opt, NLEndpoint::e_optlevel lev, const void *data, unsigned int datasize);
	int setNonBlock(bool on = true);
	int setReuseAddr(bool on = true);

	
	inline const NLAddress &getAddr() const;
	inline const NLAddress &getPeer() const;
	inline int getSocket() const;
	
	virtual void close();
	
	virtual void bind(const NLAddress &addr);
	virtual void bind(int port = 0);
	
	virtual void  connect(const NLAddress &addr);
	virtual void  connect(const char *addr, int port);
	
	virtual void listen(int backlog = 5);
	
	virtual NLEndpoint *clone(NLEndpoint *);
	virtual NLEndpoint *accept(long timeout = -1);
	
	inline void throwOnSendError(bool t = true);
	inline void throwOnRecvError(bool t = true);
	inline int getLastError() const;
	inline char *getLastErrorStr() const;
	
	virtual size_t send(const void *buf, size_t size, int flags = 0);
	virtual size_t send(NLPacket &pack, int flags = 0);
	virtual size_t sendto(const void *buf, size_t size, const NLAddress &to, int flags = 0);
	virtual size_t sendto(NLPacket &pack, const NLAddress &to, int flags = 0);
	
	inline void setRecvTimeout(long l);
	virtual size_t recv(void *buf, size_t size, int flags = 0);
	virtual size_t recv(NLPacket &pack, size_t size, int flags = 0);
	virtual size_t recvfrom(void *buf, size_t size, NLAddress &from, int flags = 0);
	virtual size_t recvfrom(NLPacket &pack, size_t size, NLAddress &from, int flags = 0);
	
	virtual bool isDataPending(long timeout = 0);

protected:

	static unsigned long s_inval;
	static unsigned long s_open;
	static unsigned long s_bound;
	static unsigned long s_connected;
	
	virtual void p_resetSocket();
	
	NLAddress m_addr;
	NLAddress m_peer;
	
	bool m_throwsend;
	bool m_throwrecv;
	int m_lasterr;
	
	int m_socket;
	unsigned long m_state;
	long m_timeout;
	e_type m_proto;
};



inline const NLAddress &NLEndpoint::getAddr() const
{
	return m_addr;
}


inline const NLAddress &NLEndpoint::getPeer() const
{
	return m_peer;
}


inline int NLEndpoint::getSocket() const
{
	return m_socket;
}

inline void NLEndpoint::setRecvTimeout(long l)
{
	m_timeout = l;
}

inline int NLEndpoint::getLastError() const
{
	return m_lasterr;
}


inline char *NLEndpoint::getLastErrorStr() const
{
	return strerror(m_lasterr);
}


inline void NLEndpoint::throwOnSendError(bool t)
{
	m_throwsend = t;
}


inline void NLEndpoint::throwOnRecvError(bool t)
{
	m_throwrecv = t;
}



#endif

