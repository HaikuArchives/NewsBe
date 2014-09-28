//
// NLException.h
//
// Copyright 1997 by Howard Berkey
//

#ifndef H_NLEXCEPTION
#define H_NLEXCEPTION

#include <stdio.h>
#include <string.h>

class NLDebug
{
public:
    static inline void on() {m_on = true;}
    static inline void off() {m_on = false;}
    static inline bool isOn() {return m_on;}
    static void print(const char *msg);
    static void dump(const char *data, size_t size, const char *title);
    
private:
    static bool m_on;
};



class NLException
{
public:
	char m_file[255];
	int m_line;
	char m_where[255];
	char m_what[255];
	void *m_arg;
	
	inline NLException(char *file, int line, char *where, char *what, void *arg);
	inline void print(char *s);
};

inline NLException::NLException(char *file, int line, char *where, char *what, void *arg)
{
	memset(m_file, 0, sizeof(m_file));
	memset(m_where, 0, sizeof(m_where));
	memset(m_what, 0, sizeof(m_what));
		
	if(file != 0)
		strncpy(m_file, file, sizeof(m_file) - 1);
	m_line = line;
	if(where != 0)
		strncpy(m_where, where, sizeof(m_where) - 1);
	if(what != 0)
		strncpy(m_what, what, sizeof(m_what) - 1);
	m_arg = arg;
}

#define NL_THROW(x, y) throw NLException(__FILE__, __LINE__, (x), (y), 0)
#define NL_THROW_ARG(x, y, z) throw NLException(__FILE__, __LINE__, (x), (y), (z))

inline void NLException::print(char *s)
{
	sprintf(s, "......\nNetworking Library Exception\n%s\n( File \"%s\"; Line %d) : %s\n......\n",
	m_where, m_file, m_line, m_what);
	printf("......\nNetworking Library Exception\n%s\n( File \"%s\"; Line %d) : %s\n......\n",
	m_where, m_file, m_line, m_what);	
}

#endif
