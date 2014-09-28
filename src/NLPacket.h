//
// NLPacket.h
// Networking library packet class
//
// Copyright 1997 by Howard Berkey
//

#ifndef H_NLPACKET
#define H_NLPACKET

#include <sys/types.h>

class NLPacket
{
public:
	NLPacket(size_t size = 0);
	~NLPacket();
	
	void insert(char);
	void insert(unsigned char);
	void insert(short);
	void insert(unsigned short);
	void insert(int);
	void insert(unsigned int);
	void insert(long);
	void insert(unsigned long);
	void insert(float);
	void insert(double);
	void insert(const char *);
	void insert(const void *, size_t);

    //
    // This is changed because of a bug in the IRIX CC that
    // screws up references
    //
	void remove(char &);
	void remove(unsigned char &);
	void remove(short &);
	void remove(unsigned short &);
	void remove(int &);
	void remove(unsigned int &);
	void remove(long &);
	void remove(unsigned long &);
	void remove(float &);
	void remove(double &);
	void remove(char *, size_t);
	void remove(void *, size_t);
 
	inline unsigned char *getData();
	inline size_t getSize();
	inline size_t getBytesRemaining();
	
protected:
	void p_growToHold(size_t addsize);
	bool p_verifySizeRemaining(size_t size);
	
	size_t m_packSize;
	size_t m_trueSize;
	size_t m_removePos;
	unsigned char *m_data;
	static const size_t kGrowBy; 
	
	NLPacket &operator=(const NLPacket &);
	NLPacket(const NLPacket &);
};

inline unsigned char *NLPacket::getData() 
{
	return m_data;
}



inline size_t NLPacket::getSize()
{
	return m_packSize;
}

inline size_t NLPacket::getBytesRemaining()
{
	return m_packSize - m_removePos;
}

#endif
